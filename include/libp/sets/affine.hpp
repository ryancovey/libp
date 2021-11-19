#ifndef LIBP_SETS_AFFINE_HPP_GUARD
#define LIBP_SETS_AFFINE_HPP_GUARD

#include <ostream>
#include <type_traits>
#include <utility>
#include <libp/internal/constants.hpp>
#include <libp/internal/utf8.hpp>
#include <libp/sets/measurable_set.hpp>

namespace libp {

    // If B = Affine<decltype(A), decltype(m), decltype(c)>(A, m, c), then
    // y is in B if and only if there exists x in A such that y = m*x + c.

    template<class, class, class, bool>
    class Affine;

    template<
        class T,
        class M,
        class C,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<T>>::value,
            bool
        > = true
    >
    class AffineBase : public MeasurableSetCRTP<Affine<T, M, C>> {
        public:
            AffineBase() = delete;

            AffineBase(T set_in): set(std::move(set_in)) { }

            AffineBase(T set_in, M m_in):
                set(std::move(set_in)),
                m(std::move(m_in))
            { }

            AffineBase(T set_in, M m_in, C c_in):
                set(std::move(set_in)),
                m(std::move(m_in)),
                c(std::move(c_in))
            { }

            T set;
            M m = zero<M>();
            C c = zero<C>();
    }

    template<
        class T,
        class M1,
        class C1,
        class M2,
        class C2,
        bool B
    >
    class AffineBase<Affine<T, M1, C1>, M2, C2> {
        static_assert(
            false,
            "Affine<Affine<.,.,.>,.,.> typed sets are not supported, since an affine transformation of "
            "an affine transformation of a set of type T is an affine transformation of a set of type T, "
            "(that is, affine transformations are closed) and so an affine transformation of a set of "
            "type Affine<T,.,.> should be of type Affine<T,.,.>."
        );
    }


    template<class T, class M, class C>
    class Affine final : public AffineBase<T, M, C> {
        using AffineBase::AffineBase;
    };

    // Produce specialisations of affine for different set types as follows:
    // 
    // template<class M, class C>
    // class Affine<SomeSetType, M, C> : public AffineBase<SomeSetType, M, C> {
    //     public:
    //         using AffineBase::AffineBase;
    //
    //         static void register_type(void) {
    //             ...
    //         }
    //
    //         static void register_type(std::size_t i) {
    //             ...
    //         }
    // };
    //
    // If specialised constructors are required, omit using AffineBase::AffineBase,
    // and define all required constructors, including those that would have otherwise
    // been "inherited" from AffineBase.

    template<
        class L,
        class R,
        std::enable_if_t<
            !std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator*(L&& lhs, R&& rhs) {
        return Affine<decltype(rhs), decltype(lhs), decltype(lhs)>(
            std::forward<R>(rhs),
            std::forward<L>(lhs),
            zero<decltype(lhs)>()
        );
    }

    template<
        class L,
        class R,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            !std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator*(L&& lhs, R&& rhs) {
        return std::forward<R>(rhs)*std::forward<L>(lhs);
    }

    template<
        class L,
        class R,
        std::enable_if_t<
            !std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator+(L&& lhs, R&& rhs) {
        return Affine<decltype(rhs), decltype(lhs), decltype(lhs)>(
            std::forward<R>(rhs),
            one<decltype(lhs)>(),
            std::forward<L>(lhs)
        );
    }

    template<
        class L,
        class R,
        std::enable_if_t<
            std::is_base_of<MeasurableSetImpl, std::decay_t<L>>::value &&
            !std::is_base_of<MeasurableSetImpl, std::decay_t<R>>::value,
            bool
        > = true
    >
    auto operator+(L&& lhs, R&& rhs) {
        return std::forward<R>(rhs) + std::forward<L>(lhs);
    }
    
    // By default, assume for the purposes of operator* and operator+ that M and C
    // are scalars. Specialise later for matrix and tensor types.

    template<class L, class T, class M, class C>
    auto operator*(L&& lhs, const Affine<T, M, C>& rhs) {
        auto prod_m = lhs*rhs.m;
        auto prod_c = std::forward<L>(lhs)*rhs.c
        return Affine<T, decltype(prod_m), decltype(prod_c)>(
            rhs.set,
            std::move(prod_m),
            std::move(prod_c)
        );
    }

    template<class L, class T, class M, class C>
    auto operator*(L&& lhs, Affine<T, M, C>&& rhs) {
        auto prod_m = lhs*std::move(rhs.m);
        auto prod_c = std::forward<L>(lhs)*std::move(rhs.c);
        return Affine<T, decltype(prod_m), decltype(prod_c)>(
            std::move(rhs.set),
            std::move(prod_m),
            std::move(prod_c)
        );
    }

    template<class T, class M, class C, class R>
    auto operator*(const Affine<T, M, C>& lhs, R&& rhs) {
        return std::forward<R>(rhs)*lhs;
    }

    template<class T, class M, class C, class R>
    auto operator*(Affine<T, M, C>&& lhs, R&& rhs) {
        return std::forward<R>(rhs)*std::move(lhs);
    }

    template<class L, class T, class M, class C>
    auto operator+(L&& lhs, const Affine<T, M, C>& rhs) {
        return Affine<T, M, decltype(lhs + rhs.c)>(
            rhs.set,
            rhs.m,
            std::forward<L>(lhs) + rhs.c
        );
    }

    template<class L, class T, class C, class M>
    auto operator+(L&& lhs, Affine<T, M, C>&& rhs) {
        return Affine<T, M, decltype(lhs + rhs.c)>(
            std::move(rhs.set),
            std::move(rhs.m),
            std::forward<CL>(lhs) + std::move(rhs.c)
        );
    }

    template<class T, class M, class C, class R>
    auto operator+(const Affine<T, M, C>& lhs, R&& rhs) {
        return std::forward<R>(rhs) + lhs;
    }

    template<class T, class ML, class CL, class CR>
    auto operator+(Affine<T, ML, CL>&& lhs, CR&& rhs) {
        return std::forward<R>(rhs) + std::move(lhs);
    }

    template<class T, class M, class C>
    std::ostream& operator<<(std::ostream& os, const Affine<T, M, C>& affine) {
        if (affine.m == zero<M>()) {
            os << '{' << affine.c << '}';
        } else {
            os << affine.m << ' ' << utf8::times << ' ' << affine.set
            if (affine.c != zero<C>()) {
                os << " + " << affine.c;
            }
        }

        return os;
    }

}

#endif

