#include <tuple_arithmetic.hpp>
#include <xsimd/xsimd.hpp>
#include <array>

#pragma once

namespace picinterp {

namespace internal {
static inline constexpr std::array<uint64_t, 21> factorial {
        1ull,                  1ull,                  2ull,
        6ull,                  24ull,                 120ull,
        720ull,                5040ull,               40320ull,
        362880ull,             3628800ull,            39916800ull,
        479001600ull,          6227020800ull,         87178291200ull,
        1307674368000ull,      20922789888000ull,     355687428096000ull,
        6402373705728000ull,   121645100408832000ull, 2432902008176640000ull
};

template<typename T>
static inline constexpr T powi(T v, size_t N) {
    if (N == 0)
        return T(1);
    else if (N == 1)
        return v;
    else {
        auto half = powi(v, N/2);
        if (N % 2 == 0)
            return half * half;
        else
            return half * half * v;
    }
}

template<size_t D, size_t Np>
static inline constexpr auto cartesian_prod_impl() {
    std::array<std::array<int, D>, tpa::powi<D>(Np)> result;
    for (size_t d = 0; d < D; ++d) {
        const auto nseg = powi(Np, d);
        const auto seg_len = powi(Np, D-d-1);
        for (auto n = 0; n < Np; ++n) {
            for (auto s = 0; s < nseg; ++s) {
                for (auto i = 0; i < seg_len; ++i) {
                    result[(s*Np+n)*seg_len + i][d] = n;
                }
            }
        }
    }
    return result;
}

template<size_t D, size_t Np>
static inline constexpr auto cartesian_prod = cartesian_prod_impl<D, Np>();


// SIMD functions
template<typename T, size_t N>
FORCE_INLINE auto to_simd(const std::array<T, N>& arr) {
    using simd_t = xsimd::make_sized_batch_t<T, N>;
    if constexpr (std::is_void_v<simd_t>)
        return arr;
    else
        return simd_t::load_aligned(arr.data());
}

template<typename T, typename A>
FORCE_INLINE auto to_simd(const xsimd::batch<T, A>& arr) { return arr; }

template<typename T, size_t N>
FORCE_INLINE auto to_simdu(const std::array<T, N>& arr) {
    using simd_t = xsimd::make_sized_batch_t<T, N>;
    if constexpr (std::is_void_v<simd_t>)
        return arr;
    else
        return simd_t::load_unaligned(arr.data());
}

template<typename T, typename A>
FORCE_INLINE auto to_simdu(const xsimd::batch<T, A>& arr) { return arr; }

template<typename T, size_t N, typename T1>
FORCE_INLINE auto store(std::array<T, N>& arr, T1&& val) {
    if constexpr (xsimd::is_batch<std::remove_cvref_t<T1>>::value)
        val.store_aligned(arr.data());
    else
        tpa::assign(arr, std::forward<T1>(val));
}

} // namespace internal
} // namespace picinterp
