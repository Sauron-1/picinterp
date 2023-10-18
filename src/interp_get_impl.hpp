#include "../include/picinterp_cb.hpp"
#include <string>

#include "utils.hpp"

template<size_t O, size_t D, typename Float, size_t N = simd_size_of<Float>, typename Int = int_type_of_t<Float>>
auto interp_get_py_impl(const std::vector<py::array_t<Float>>& srcs, const std::vector<py::array_t<Float>>& coords, const std::vector<Float>& base, const std::vector<Float>& scale) {
    const size_t num_src = srcs.size();
    if (coords.size() != D)
        throw std::invalid_argument("Number of provided coords does not match dimension");
    auto src_shape = get_same_shape(srcs);
    if (src_shape.size() != D)
        throw std::invalid_argument("Dimension of source array mismatch with interp dimension!");
    auto coord_shape = get_same_shape(coords);

    std::array<Float, D> lims;
    for (size_t i = 0; i < D; ++i)
        lims[i] = src_shape[i];

    using uc_src_t = decltype(srcs[0].template unchecked<D>());
    std::vector<uc_src_t> uc_srcs;
    std::vector<NdArrayWrapper<Float>> uc_coords;

    std::array<Float, D> base_arr, scale_arr;
    for (size_t i = 0; i < D; ++i) {
        base_arr[i] = base[i];
        scale_arr[i] = scale[i];
    }

    for (auto i = 0; i < num_src; ++i)
        uc_srcs.emplace_back(srcs[i].template unchecked<D>());
    for (auto i = 0; i < D; ++i)
        uc_coords.emplace_back(coords[i]);

    // Construct result arrays
    std::vector<py::array_t<Float>> results;
    for (auto i = 0; i < num_src; ++i)
        results.emplace_back(coord_shape);
    std::vector<NdArrayWrapperMut<Float>> uc_results;
    for (auto i = 0; i < num_src; ++i)
        uc_results.emplace_back(results[i]);

    IndexGenerator ig(coord_shape);
    size_t num_aligned = ig.size() / N;
    size_t num_unaligned = ig.size() % N;

    using Interp = picinterp_cb::InterpolatorV<N, D, O, Float, Int>;
    using Interpu = picinterp_cb::InterpolatorV<1, D, O, Float, Int>;

    // Aligned interp
#if defined(_OPENMP)
#   pragma omp parallel
#endif
    {
        std::array<std::array<Float, N>, D> data;
        std::array<std::vector<size_t>, N> idx;
        for (auto& _idx : idx) _idx.resize(ig.ndim());
#if defined(_OPENMP)
#   pragma omp for schedule(guided)
#endif
        for (auto i = 0; i < num_aligned; ++i) {
            for (auto n = 0; n < N; ++n)
                ig.get(i*N+n, idx[n]);
            for (auto d = 0; d < D; ++d)
                for (auto n = 0; n < N; ++n)
                    data[d][n] = uc_coords[d][idx[n]];
            tpa::assign(data, (data - base_arr) * scale_arr);
            Interp interp(data, lims);
            for (auto is = 0; is < num_src; ++is) {
                auto result = interp.gather(uc_srcs[is]);
                for (auto n = 0; n < N; ++n)
                    uc_results[is][idx[n]] = result[n];
            }
        }
    }

    // Unaligned interp
    std::array<std::array<Float, 1>, D> data_u;
    std::vector<size_t> idx(ig.ndim());
    for (size_t i = num_aligned*N; i < ig.size(); ++i) {
        ig.get(i, idx);
        for (auto d = 0; d < D; ++d)
            data_u[d][0] = uc_coords[d][idx];
        tpa::assign(data_u, (data_u - base_arr) * scale_arr);
        Interpu interp(data_u, lims);
        for (auto is = 0; is < num_src; ++is) {
            auto result = interp.gather(uc_srcs[is]);
            uc_results[is][idx] = result[0];
        }
    }

    return results;
};
