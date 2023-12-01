#ifndef LIBP_SETS_FUNCTION_SPACE_HPP_GUARD
#define LIBP_SETS_FUNCTION_SPACE_HPP_GUARD

#include <cstddef>
#include <utility>

namespace libp {

    template<class Set>
    concept SetConcept = requires(Set A, Set B) {
        requires std::default_initializable<Set>;
        !A;
        A::empty();
        A && B;
        A || B;
        A == B;
        A != B;
    };

    template<SetConcept Domain, SetConcept Codomain>
    struct CylinderSet {
        // Represents the set of functions f such that
        // for all x in domain_subset, f(x) is in
        // codomain_subset. This is also the type for
        // a node in a binary tree with nodes stored in a
        // std::vector, so we have std::vector indices
        // to the parent and two children. Child index 0
        // represents the null set of functions from
        // the domain to the codomain, and child index 1
        // represents the set of all such functions.
        // Parent index 0 means the root node, and parent
        // index 1 is invalid.

        CylinderSet(
            Domain domain_subset_in,
            Codomain codomain_subset_in,
            std::size_t p_in,
            std::size_t n_in,
            std::size_t u_in
        ):
            domain_subset(std::move(domain_subset_in)),
            codomain_subset(std::move(codomain_subset_in)),
            
        { }

        Domain domain_subset;
        Codomain codomain_subset;
        std::size_t p = 0; // parent index
        std::size_t n = 0; // intersection child index
        std::size_t u = 0; // union child index
    };

}

#endif
