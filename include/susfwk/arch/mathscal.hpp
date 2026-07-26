// susfwk-arch-mathscal.hpp
//
/*
* =====---------------- mathscal.hpp - complex mathematical functions with a fraction without simd ----------------=====
*
* Part of the SUSFramework3 Project, under the MIT License
* See LICENSE.txt for license information.
* SPDX-License-Identifier: MIT
*
* =====------------------------------------------------------------------------------------------------------------=====
*/
#ifndef _SUSFWK_ARCH_MATHSCAL_
#define _SUSFWK_ARCH_MATHSCAL_

#include "susfwk/utils/core.hpp"
#include "susfwk/utils/base_math.hpp"
#include "susfwk/arch/incasimd.hpp"

namespace sus {

    // -------------------------------------------------------------------

    // Rounding up
    template<sus::floating_t T>
    SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T ceil(T x) {
        #if defined(SUS_ARCH_X86)
        if constexpr (is_same_v<T, f32>)
            return _mm_cvtss_f32(_mm_ceil_ss(_mm_setzero_ps(), _mm_set_ss(x)));
        else
            return _mm_cvtsd_f64(_mm_ceil_sd(_mm_setzero_pd(), _mm_set_sd(x)));
        #else

        #endif // !SUS_ARCH_X86
    }
    // Rounding down
    template<sus::floating_t T>
    SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T floor(T x) {
        #if defined(SUS_ARCH_X86)
        if constexpr (is_same_v<T, f32>)
            return _mm_cvtss_f32(_mm_floor_ss(_mm_setzero_ps(), _mm_set_ss(x)));
        else
            return _mm_cvtsd_f64(_mm_floor_sd(_mm_setzero_pd(), _mm_set_sd(x)));
        #else

        #endif // !SUS_ARCH_X86
    }
    // Rounding according to the rules
    template<sus::floating_t T>
    SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T round(T x) {
        #if defined(SUS_ARCH_X86)
        if constexpr (is_same_v<T, f32>)
            return _mm_cvtss_f32(_mm_round_ss(_mm_setzero_ps(), _mm_set_ss(x), _MM_FROUND_TO_NEAREST_INT));
        else
            return _mm_cvtsd_f64(_mm_round_sd(_mm_setzero_pd(), _mm_set_sd(x), _MM_FROUND_TO_NEAREST_INT));
        #else

        #endif // !SUS_ARCH_X86
    }

	// -------------------------------------------------------------------

    namespace math::details {

        template<sus::floating_t T>
        inline constexpr T sin_c1 = static_cast<T>(1.0 / 1.0);
        template<sus::floating_t T>
        inline constexpr T sin_c3 = static_cast<T>(-1.0 / 6.0);
        template<sus::floating_t T>
        inline constexpr T sin_c5 = static_cast<T>(1.0 / 120.0);
        template<sus::floating_t T>
        inline constexpr T sin_c7 = static_cast<T>(-1.0 / 5040.0);
        template<sus::floating_t T>
        inline constexpr T sin_c9 = static_cast<T>(1.0 / 362880.0);
        template<sus::floating_t T>
        inline constexpr T sin_c11 = static_cast<T>(-1.0 / 39916800.0);
        template<sus::floating_t T>
        inline constexpr T sin_c13 = static_cast<T>(1.0 / 6227020800.0);

    }

    // Mathematical sine function
    template<sus::floating_t T>
	SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T sin(T x) {
        x *= (1 / (sus::math::pi<T> * 2));
        auto q = sus::floor(x + T(0.5));
        x = (x - q) * (sus::math::pi<T> * 2);
        auto quadrant = static_cast<sus::float_to_int_t<T>>(q) & 3;
        auto sign = (quadrant & 2) ? T(-1.0) : T(1.0);
        auto arg = (quadrant & 1) ? (sus::math::pi<T> / 2) -x : x;
        x = arg * arg;
        auto x2 = x * x;
        auto x3 = x2 * x;
        auto x4 = x3 * x;
        auto result = sus::math::details::sin_c1<T>;
        result += x * sus::math::details::sin_c3<T>;
        result += x2 * sus::math::details::sin_c5<T>;
        result += x3 * sus::math::details::sin_c7<T>;
        result += x4 * sus::math::details::sin_c9<T>;
        if constexpr (sus::is_same_v<T, sus::f64>) {
            auto x5 = x4 * x;
            auto x6 = x5 * x;
            result += x5 * sus::math::details::sin_c11<T>;
            result += x6 * sus::math::details::sin_c13<T>;
        }
        return sign * arg * result;
	}
    // Mathematical cosine function
    template<sus::floating_t T>
    SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T cos(T x) {
        return sin(x + (math::pi<T> / 2));
    }

	// -------------------------------------------------------------------

    namespace math::details {
        // The natural logarithm of two
        template<sus::floating_t T>
        inline constexpr T ln2 = static_cast<T>(0.693147180559945309417232121458176568);
        // The natural logarithm of ten
        template<sus::floating_t T>
        inline constexpr T ln10 = static_cast<T>(2.302585092994045684017991454684364208);
        // The base-10 logarithm of e
        template<sus::floating_t T>
        inline constexpr T log10e = static_cast<T>(0.434294481903251827651128918916605082);
        // Logarithm to the base 10 of two
        template<sus::floating_t T>
        inline constexpr T log10_2 = sus::math::details::ln2<T> / sus::math::details::ln10<T>;

        template<sus::floating_t T>
        inline constexpr T log_c1 = static_cast<T>(1.0 / 1.0);
        template<sus::floating_t T>
        inline constexpr T log_c2 = static_cast<T>(-1.0 / 2.0);
        template<sus::floating_t T>
        inline constexpr T log_c3 = static_cast<T>(1.0 / 3.0);
        template<sus::floating_t T>
        inline constexpr T log_c4 = static_cast<T>(-1.0 / 4.0);
        template<sus::floating_t T>
        inline constexpr T log_c5 = static_cast<T>(1.0 / 5.0);
        template<sus::floating_t T>
        inline constexpr T log_c6 = static_cast<T>(-1.0 / 6.0);
        template<sus::floating_t T>
        inline constexpr T log_c7 = static_cast<T>(1.0 / 7.0);
        template<sus::floating_t T>
        inline constexpr T log_c8 = static_cast<T>(-1.0 / 8.0);

        template<sus::floating_t T>
        inline constexpr T log2_c1 = log_c1<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c2 = log_c2<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c3 = log_c3<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c4 = log_c4<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c5 = log_c5<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c6 = log_c6<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c7 = log_c7<T> / ln2<T>;
        template<sus::floating_t T>
        inline constexpr T log2_c8 = log_c8<T> / ln2<T>;

        template<sus::floating_t T>
        inline constexpr T log10_c1 = log_c1<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c2 = log_c2<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c3 = log_c3<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c4 = log_c4<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c5 = log_c5<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c6 = log_c6<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c7 = log_c7<T> / ln10<T>;
        template<sus::floating_t T>
        inline constexpr T log10_c8 = log_c8<T> / ln10<T>;
    }

    // Mathematical log function
    template<auto base = 0, sus::floating_t T>
    SUS_ATTRIB_NODISCARD SUS_ATTRIB_CONST constexpr T log(T x) {
        if (sus::is_negative(x)) return (x == 0.0) ? sus::numeric_limits<T>::neg_infinity() : sus::numeric_limits<T>::quiet_NaN();
        if constexpr (base != 0 && base != 10 && base != 2) {
            return sus::log(x) / sus::log(T(base));
        }
        else {
            union { T f; sus::float_to_int_t<T> i; } u = { x };
            auto exp = (u.i >> (numeric_limits<T>::digits - 1)) - (numeric_limits<T>::max_exponent - 1);
            auto mantissa = u.i & numeric_limits<T>::MANTISSA_MASK;
            u.i = mantissa | numeric_limits<T>::ONE_BITS;
            auto t = u.f - T(1.0f);
            auto t2 = t * t;
            auto t3 = t2 * t;
            auto t4 = t3 * t;
            auto t5 = t4 * t;
            T poly;
            if constexpr (base == 0) {
                poly = sus::math::details::log_c1<T>;
                poly += t * sus::math::details::log_c2<T>;
                poly += t2 * sus::math::details::log_c3<T>;
                poly += t3 * sus::math::details::log_c4<T>;
                poly += t4 * sus::math::details::log_c5<T>;
                poly += t5 * sus::math::details::log_c6<T>;
            }
            else if constexpr (base == 2) {
                poly = sus::math::details::log2_c1<T>;
                poly += t * sus::math::details::log2_c2<T>;
                poly += t2 * sus::math::details::log2_c3<T>;
                poly += t3 * sus::math::details::log2_c4<T>;
                poly += t4 * sus::math::details::log2_c5<T>;
                poly += t5 * sus::math::details::log2_c6<T>;
            }
            else if constexpr (base == 10) {
                poly = sus::math::details::log10_c1<T>;
                poly += t * sus::math::details::log10_c2<T>;
                poly += t2 * sus::math::details::log10_c3<T>;
                poly += t3 * sus::math::details::log10_c4<T>;
                poly += t4 * sus::math::details::log10_c5<T>;
                poly += t5 * sus::math::details::log10_c6<T>;
            }
            if constexpr (sus::is_same_v<T, sus::f64>) {
                auto t6 = t5 * t;
                auto t7 = t6 * t;
                if constexpr (base == 0) {
                    poly += t6 * sus::math::details::log_c7<T>;
                    poly += t7 * sus::math::details::log_c8<T>;
                }
                else if constexpr (base == 2) {
                    poly += t6 * sus::math::details::log2_c7<T>;
                    poly += t7 * sus::math::details::log2_c8<T>;
                }
                else if constexpr (base == 10) {
                    poly += t6 * sus::math::details::log10_c7<T>;
                    poly += t7 * sus::math::details::log10_c8<T>;
                }
            }
            poly *= t;
            if constexpr (base == 0) poly += static_cast<T>(exp) * sus::math::details::ln2<T>;
            else if constexpr (base == 10) poly += static_cast<T>(exp) * sus::math::details::log10_2<T>;
            else poly += static_cast<T>(exp);
            return poly;
        }
    }

    // -------------------------------------------------------------------

}

#endif /* !_SUSFWK_ARCH_MATHSCAL_ */
