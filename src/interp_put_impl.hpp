#include "../include/picinterp_cb.hpp"
#include <string>

#include "utils.hpp"

template<size_t D, typename T>
class ParallelCopyArray {
    public:
        using Ref = decltype(std::declval<py::array_t<T>&>().template mutable_unchecked<D>());
        ParallelCopyArray(int threads, py::array_t<T>& src) : m_src(src) {
            std::vector<size_t> src_shape(D);
            size_t size = 1;
            for (auto i = 0; i < D; ++i) {
                src_shape[i] = m_src.shape(i);
                size *= src_shape[i];
            }
            for (auto i = 0; i < threads; ++i) {
                m_data.emplace_back(src_shape);
                m_refs.emplace_back(m_data[i].template mutable_unchecked<D>());
            }
            for (auto& arr : m_data) {
                T* ptr = arr.mutable_data();
                for (auto i = 0; i < size; ++i)
                    ptr[i] = T(0);
            }
        }

        Ref& operator[](size_t i) {
            return m_refs[i];
        }

        void gather() {
            auto src_uc = m_src.template mutable_unchecked<D>();
            std::vector<size_t> src_shape(D);
            for (auto i = 0; i < D; ++i)
                src_shape[i] = m_src.shape(i);
            IndexGeneratorP<size_t, D> ig(src_shape);
            const size_t threads = m_refs.size();
#if defined(_OPENMP)
#pragma omp parallel for schedule(static)
#endif
            for (auto i = 0; i < ig.size(); ++i) {
                auto idx = ig[i];
                for (auto t = 0; t < threads; ++t) {
                    std::apply(src_uc, idx) += std::apply(m_refs[t], idx);
                }
            }
        }

    private:
        py::array_t<T>& m_src;
        std::vector<py::array_t<T>> m_data;
        std::vector<Ref> m_refs;
};

template<size_t O, size_t D, typename Float, size_t N = simd_size_of<Float>, typename Int = int_type_of_t<Float>>
auto interp_put_py_impl(
        std::vector<py::array_t<Float>>& targets,
        const std::vector<py::array_t<Float>>& coords,
        const std::vector<py::array_t<Float>>& weights,
        const std::vector<Float>& base, const std::vector<Float>& scale) {
    //
    if (coords.size() != D)
        throw std::invalid_argument("Number of provided coords does not match dimension: " + std::to_string(coords.size()) + " vs " + std::to_string(D));
    auto src_shape = get_same_shape(targets);
    if (src_shape.size() != D)
        throw std::invalid_argument("Dimension of source array mismatch with interp dimension!");
    auto coord_shape = get_same_shape(coords);

    if (weights.size() != targets.size())
        throw std::invalid_argument("Number of values and targets mismatch");
    auto weight_shape = get_same_shape(weights);
    if (weights.size() != coord_shape.size())
        throw std::invalid_argument("Values dim mismatch with coords dim");
    for (auto i = 0; i < weights.size(); ++i)
        if (weight_shape[i] != coord_shape[i])
            throw std::invalid_argument("Values shape mismatch with coords shape");

    //
    std::array<Float, D> lims;
    for (size_t i = 0; i < D; ++i)
        lims[i] = src_shape[i];

    //
    std::vector<NdArrayWrapper<Float>> uc_coords;
    std::vector<NdArrayWrapper<Float>> uc_weights;

    for (auto i = 0; i < D; ++i)
        uc_coords.emplace_back(coords[i]);
    for (auto i = 0; i < weights.size(); ++i)
        uc_weights.emplace_back(weights[i]);

    //
    std::array<Float, D> base_arr, scale_arr;
    for (size_t i = 0; i < D; ++i) {
        base_arr[i] = base[i];
        scale_arr[i] = scale[i];
    }

    //
    int threads = omp_get_max_threads();
    std::vector<ParallelCopyArray<D, Float>> ptargets;
    for (auto i = 0; i < targets.size(); ++i)
         ptargets.emplace_back(threads, targets[i]);

    IndexGenerator ig(coord_shape);
    size_t num_aligned = ig.size() / N;
    size_t num_unaligned = ig.size() % N;

    using Interp = picinterp_cb::InterpolatorV<N, D, O, Float, Int>;
    using Interpu = picinterp_cb::InterpolatorV<1, D, O, Float, Int>;

    const size_t num_w = weights.size();

    // Aligned interp
#if defined(_OPENMP)
#   pragma omp parallel
#endif
    {
        std::vector<std::array<Float, N>> weight(num_w);
        std::array<std::vector<size_t>, N> idx;
        for (auto& _idx : idx) _idx.resize(ig.ndim());
        const int tid = omp_get_thread_num();
#if defined(_OPENMP)
#   pragma omp for schedule(guided)
#endif
        for (auto i = 0; i < num_aligned; ++i) {
            std::array<std::array<Float, N>, D> data;
            for (auto n = 0; n < N; ++n)
                ig.get(i*N+n, idx[n]);
            for (auto iw = 0; iw < num_w; ++iw)
                for (auto n = 0; n < N; ++n)
                    weight[iw][n] = uc_weights[iw][idx[n]];
            for (auto d = 0; d < D; ++d)
                for (auto n = 0; n < N; ++n)
                    data[d][n] = uc_coords[d][idx[n]];
            tpa::assign(data, (data - base_arr) * scale_arr);
            Interp interp(data, lims);
            for (auto iw = 0; iw < num_w; ++iw)
                interp.scatter(ptargets[iw][tid], weight[iw]);
        }
    }

    // Unaligned interp
    std::array<std::array<Float, 1>, D> data_u;
    std::vector<std::array<Float, 1>> weight_u(num_w);
    std::vector<size_t> idx(ig.ndim());
    for (size_t i = num_aligned*N; i < ig.size(); ++i) {
        ig.get(i, idx);
        for (auto d = 0; d < D; ++d)
            data_u[d][0] = uc_coords[d][idx];
        for (auto iw = 0; iw < num_w; ++iw)
            weight_u[iw][0] = uc_weights[iw][idx];
        tpa::assign(data_u, (data_u - base_arr) * scale_arr);
        Interpu interp(data_u, lims);
        for (auto iw = 0; iw < num_w; ++iw)
            interp.scatter(ptargets[iw][0], weight_u[iw]);
    }

    for (auto& pt : ptargets) pt.gather();
}
