#include <tuple_arithmetic.hpp>
#include <tuple_math.hpp>
#include <type_traits>
#include <cmath>
#include <array>
#include <iostream>
#include <utility>
#include <xsimd/xsimd.hpp>

#include "utils.hpp"

#pragma once

namespace picinterp_cb {

namespace internal {
using namespace picinterp::internal;
}

template<size_t N, size_t D, size_t O, typename Float, typename Int>
struct InterpWeightIndex {
    static constexpr size_t num_points = N,
                            dimension = D,
                            order = O;
    using float_t = Float;
    using int_t = Int;

    static constexpr size_t interp_pts = order + 1;

    using Scalar = std::array<float_t, num_points>;
    using IScalar = std::array<int_t, num_points>;
    using Mask = std::array<bool, num_points>;
    using idx_t = std::array<IScalar, dimension>;
    using pos_t = std::array<Scalar, dimension>;

    // shape: (interp_pts, dimension, num_points)
    alignas(sizeof(Float)*N) std::array<pos_t, interp_pts> weights;
    alignas(sizeof(Int)*N) std::array<idx_t, interp_pts> indices;
    Mask mask;

    InterpWeightIndex() = default;

    template<tpa::tuple_like Pos, tpa::tuple_like UpperLim>
    InterpWeightIndex(Pos&& pos, UpperLim&& lim) {
        init(
                std::forward<Pos>(pos),
                std::forward<UpperLim>(lim)
            );
    }

    template<tpa::tuple_like Pos, tpa::tuple_like UpperLim>
    FORCE_INLINE void init(Pos&& pos, UpperLim&& lim) {
        tpa::assign(weights, 0);
        if constexpr (order % 2 == 0)
            get_weight_index_even(std::forward<Pos>(pos));
        else
            get_weight_index_odd(std::forward<Pos>(pos));

        TP_USE_OPS;
        tpa::assign(mask, true);
        for (auto d = 0; d < D; ++d)
            for (auto n = 0; n < N; ++n)
                mask[n] &= indices[0][d][n] >= 0 and indices[interp_pts-1][d][n] < lim[d];
        tpa::assign(indices,
                tpa::select(
                    tpa::repeat_as(tpa::repeat_as(mask, indices[0]), indices),
                    indices, 0));
    }

    private:
        // Odd order
        template<int l, int dim, int m, int k>
        FORCE_INLINE auto get_weight_odd_dmk(auto dx) {
            constexpr float_t sign = (2*l-k)%2 == 0 ? 1 : -1;
            constexpr int i = m + l - 1;
            const auto base = (m + k - l) - dx;
            const auto idx = (2*l) * tpa::powi<2*l-1>(base) / internal::factorial[k] / internal::factorial[2*l-k];
            internal::store(weights[i][dim], internal::to_simd(weights[i][dim]) + sign * idx);
        }

        template<int l, int dim, int m, int k=l-m+1>
        FORCE_INLINE auto get_weight_odd_dm(auto dx) {
            if constexpr (k < 2*l + 1) {
                get_weight_odd_dmk<l, dim, m, k>(dx);
                get_weight_odd_dm<l, dim, m, k+1>(dx);
            }
        }

        template<int l, int dim, int m=-l+1>
        FORCE_INLINE auto get_weight_odd_d(auto dx) {
            if constexpr (m < l+1) {
                get_weight_odd_dm<l, dim, m>(dx);
                get_weight_odd_d<l, dim, m+1>(dx);
            }
        }

        template<int l, int dim=0>
        FORCE_INLINE auto get_weight_odd(auto&& dxs) {
            if constexpr (dim < dimension) {
                auto dx = internal::to_simdu(std::get<dim>(dxs));
                get_weight_odd_d<l, dim>(dx);
                get_weight_odd<l, dim+1>(std::forward<decltype(dxs)>(dxs));
            }
        }

        template<tpa::tuple_like Pos>
        FORCE_INLINE auto get_weight_index_odd(Pos&& pos) {
            constexpr int l = (order + 1) / 2;
            const auto center = tpa::floor(pos);
            const auto dx = pos - center;
            for (int i = -l+1; i < l+1; ++i)
                tpa::assign(indices[i+l-1], center + i);
            get_weight_odd<l>(dx);
        }


        // Even order
        template<int l, int dim, int m, int k>
        FORCE_INLINE auto get_weight_even_dmk(auto dx) {
            constexpr float_t sign = (2*l-k-1)%2 == 0 ? 1 : -1;
            constexpr int i = m + l - 1;
            const auto base = (m + k - l + 0.5) - dx;
            const auto idx = (2*l-1) * tpa::powi<2*l-2>(base) / internal::factorial[k] / internal::factorial[2*l-k-1];
            internal::store(weights[i][dim], internal::to_simd(weights[i][dim]) + sign * idx);
        }

        template<int l, int dim, int m, int k=l-m>
        FORCE_INLINE auto get_weight_even_dm(auto dx) {
            if constexpr (k < 2*l) {
                get_weight_even_dmk<l, dim, m, k>(dx);
                get_weight_even_dm<l, dim, m, k+1>(dx);
            }
        }

        template<int l, int dim, int m=-l+1>
        FORCE_INLINE auto get_weight_even_d(auto dx) {
            if constexpr (m < l) {
                get_weight_even_dm<l, dim, m>(dx);
                get_weight_even_d<l, dim, m+1>(dx);
            }
        }

        template<int l, int dim=0>
        FORCE_INLINE auto get_weight_even(auto&& dxs) {
            if constexpr (dim < dimension) {
                auto dx = internal::to_simdu(std::get<dim>(dxs));
                get_weight_even_d<l, dim>(dx);
                get_weight_even<l, dim+1>(std::forward<decltype(dxs)>(dxs));
            }
        }

        template<tpa::tuple_like Pos>
        FORCE_INLINE auto get_weight_index_even(Pos&& pos) {
            constexpr int l = order / 2 + 1;
            const auto center = tpa::round(pos);
            const auto dx = pos - center;
            for (int i = -l+1; i < l; ++i)
                tpa::assign(indices[i+l-1], center + i);
            get_weight_even<l>(dx);
        }

};


namespace internal {

template<typename T, typename Int, typename IS> struct invocable_by_ints_impl : std::false_type {};
template<typename T, typename Int, size_t...I> struct invocable_by_ints_impl<T, Int, std::index_sequence<I...>> : std::is_invocable<T, decltype(Int(I))...> {};

template<typename T, typename Int, size_t N> struct invocable_by_ints : invocable_by_ints_impl<T, Int, std::make_index_sequence<N>> {};
template<typename T, typename Int, size_t N> constexpr static inline bool invocable_by_ints_v = invocable_by_ints<T, Int, N>::value;

} // namespace internal


template<size_t N, size_t D, size_t O, typename Float=double, typename Int=int64_t>
class InterpolatorV {
    public:
        template<tpa::tuple_like Pos, tpa::tuple_like UpperLim>
        InterpolatorV(Pos&& pos, UpperLim&& lim) {
            InterpWeightIndex<N, D, O, Float, Int> wi(std::forward<Pos>(pos), std::forward<UpperLim>(lim));
            tpa::assign(weights, 1);
            for (size_t i = 0; i < cart_prod.size(); ++i) {
                for (size_t j = 0; j < D; ++j) {
                    for (size_t n = 0; n < N; ++n) {
                        indices[i][n][j] = wi.indices[cart_prod[i][j]][j][n];
                    }
                    internal::store(weights[i],
                            internal::to_simd(weights[i]) * internal::to_simd(wi.weights[cart_prod[i][j]][j]));
                }
            }
            mask = wi.mask;
        }

        template<typename T>
            requires( internal::invocable_by_ints_v<T, Int, D> )
        auto gather(const T& src) {
            using Scalar = std::remove_cvref_t<decltype(std::apply(src, std::array<int, D>{}))>;
            constexpr size_t npt = cart_prod.size();
            alignas(sizeof(Float)*N) std::array<Float, N> result;
            tpa::assign(result, 0);
            for (size_t i = 0; i < npt; ++i) {
                alignas(sizeof(Float)*N) std::array<Float, N> tmp;
                for (size_t n = 0; n < N; ++n)
                    tmp[n] = std::apply(src, indices[i][n]);
                const auto tmp1 = internal::to_simd(tmp);
                internal::store(result,
                        internal::to_simd(result) + internal::to_simd(weights[i]) * tmp1);
            }
            tpa::assign(result,
                    tpa::select(mask, result, std::nan("")));
            return result;
        }

        template<typename T, typename S>
            requires( internal::invocable_by_ints_v<T, Int, D> )
        auto scatter(T& target, const S& val) {
            constexpr size_t npt = cart_prod.size();
            alignas(sizeof(Float)*N) std::array<S, npt> vals;
            for (size_t i = 0; i < cart_prod.size(); ++i) {
                internal::store(vals[i], internal::to_simd(val) * internal::to_simd(weights[i]));
            }
            for (size_t n = 0; n < N; ++n) {
                if (mask[n])
                    for (size_t i = 0; i < cart_prod.size(); ++i) {
                        std::apply(target, indices[i][n]) += tpa::to_array(vals[i])[n];
                    }
            }
        }

    private:
        static constexpr auto cart_prod = internal::cartesian_prod<D, O+1>;
        alignas(sizeof(Float)*N) std::array<std::array<Float, N>, cart_prod.size()> weights;
        std::array<std::array<std::array<Int, D>, N>, cart_prod.size()> indices;
        std::array<bool, N> mask;
};

} // namespace picinterp_cb
