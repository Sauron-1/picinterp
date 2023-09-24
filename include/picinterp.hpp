#include <tuple_arithmetic.hpp>
#include <tuple_math.hpp>
#include <type_traits>
#include <cmath>
#include <array>
#include <iostream>
#include <xsimd/xsimd.hpp>

#include "utils.hpp"

#pragma once

namespace picinterp {

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
    using idx_t = std::array<IScalar, dimension>;
    using pos_t = std::array<Scalar, dimension>;

    // shape: (interp_pts, dimension, num_points)
    alignas(sizeof(Float)*N) std::array<pos_t, interp_pts> weights;
    alignas(sizeof(Int)*N) std::array<idx_t, interp_pts> indices;

    InterpWeightIndex() = default;

    template<tpa::tuple_like Pos>
    InterpWeightIndex(Pos&& pos) { init(std::forward<Pos>(pos)); }

    template<tpa::tuple_like Pos>
    FORCE_INLINE void init(Pos&& pos) {
        tpa::assign(weights, 0);
        if constexpr (order % 2 == 0)
            get_weight_index_even(std::forward<Pos>(pos));
        else
            get_weight_index_odd(std::forward<Pos>(pos));
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

/**
 * \deprecated
 */
template<size_t N, size_t D, size_t O, typename Float=double, typename Int=int64_t>
class Interpolator {
    public:
        template<tpa::tuple_like Pos>
        Interpolator(Pos&& pos) : wi(std::forward<Pos>(pos)) {}

        template<typename T>
            requires( internal::invocable_by_ints_v<T, Int, D> )
        auto gather(const T& src) {
            using Scalar = std::remove_cvref_t<decltype(std::apply(src, std::array<int, D>{}))>;
            std::array<Scalar, N> result;
            for (auto& r : result) r = Scalar(0);
            for (size_t n = 0; n < N; ++n) {
                for (size_t i = 0; i < cart_prod.size(); ++i) {
                    std::array<int, D> index;
                    Float weight = Float(1);
                    for (size_t j = 0; j < D; ++j) {
                        index[j] = wi.indices[cart_prod[i][j]][j][n];
                        weight *= wi.weights[cart_prod[i][j]][j][n];
                    }
                    result[n] += weight * std::apply(src, index);
                }
            }
            return result;
        }

        template<typename T, tpa::tuple_like S>
            requires( internal::invocable_by_ints_v<T, Int, D> )
        auto scatter(T& target, const S& _val) {
            const auto val = tpa::to_array(_val);
            for (size_t n = 0; n < N; ++n) {
                for (size_t i = 0; i < cart_prod.size(); ++i) {
                    std::array<int, D> index;
                    Float weight = Float(1);
                    for (size_t j = 0; j < D; ++j) {
                        index[j] = wi.indices[cart_prod[i][j]][j][n];
                        weight *= wi.weights[cart_prod[i][j]][j][n];
                    }
                    std::apply(target, index) += val[n] * weight;
                }
            }
        }

    private:
        InterpWeightIndex<N, D, O, Float, Int> wi;
        static constexpr auto cart_prod = internal::cartesian_prod<D, O+1>;
};


/**
 * Interpolator with vectorized gather/scatter. Automatically SIMD-vectorized.
 * @tparam N Number of points to interpolate.
 * @tparam D Dimension of the space.
 * @tparam O Order of the interpolator.
 * @tparam Float Floating point type.
 * @tparam Int Integer type.
 */
template<size_t N, size_t D, size_t O, typename Float=double, typename Int=int64_t>
class InterpolatorV {
    public:
        /**
         * Initialize interpolate at `pos`.
         * @param pos Position to interpolate at. Must be a tuple-like object with tuple size `D`, each element must be a tuple-like object or xsimd::batch of size `N`.
         */
        template<tpa::tuple_like Pos>
        InterpolatorV(Pos&& pos) {
            InterpWeightIndex<N, D, O, Float, Int> wi(std::forward<Pos>(pos));
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
        }

        /**
         * Gather values from `src` at the positions specified in the constructor.
         * @param src Source function. Must be callable with `D` integer coordinates and return a scalar with type `Float`.
         * @return Array of gathered values, type is `std::array<Float, N>`.
         */
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
            return result;
        }

        /**
         * Scatter values from `val` to the positions specified in the constructor.
         * @param target Target function. Must be callable with `D` integer coordinates and return an lvalue reference to a scalar with type `Float`.
         * @param val Values to scatter. Must be a tuple-like or xsimd::batch object with tuple size `N`, each element must be a scalar with type `Float`.
         */
        template<typename T, typename S>
            requires( internal::invocable_by_ints_v<T, Int, D> )
        auto scatter(T& target, const S& val) {
            constexpr size_t npt = cart_prod.size();
            alignas(sizeof(Float)*N) std::array<S, npt> vals;
            for (size_t i = 0; i < cart_prod.size(); ++i) {
                internal::store(vals[i], internal::to_simd(val) * internal::to_simd(weights[i]));
            }
            for (size_t n = 0; n < N; ++n) {
                for (size_t i = 0; i < cart_prod.size(); ++i) {
                    std::apply(target, indices[i][n]) += tpa::to_array(vals[i])[n];
                }
            }
        }

    private:
        static constexpr auto cart_prod = internal::cartesian_prod<D, O+1>;
        alignas(sizeof(Float)*N) std::array<std::array<Float, N>, cart_prod.size()> weights;
        std::array<std::array<std::array<Int, D>, N>, cart_prod.size()> indices;
};

} // namespace picinterp
