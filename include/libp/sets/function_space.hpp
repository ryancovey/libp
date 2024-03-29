#ifndef LIBP_SETS_FUNCTION_SPACE_HPP_GUARD
#define LIBP_SETS_FUNCTION_SPACE_HPP_GUARD

#include <cstddef>
#include <utility>
#include <vector>

namespace libp {

    template<class Set>
    concept SetConcept = requires(Set A, Set B) {
        !A;
        Set::empty();
        Set::universal();
        Set::nan();
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
        // to the two children. Child index 0
        // represents the null set of functions from
        // the domain to the codomain, and child index 1
        // represents the set of all such functions.

        CylinderSet(
            Domain domain_subset_in,
            Codomain codomain_subset_in,
            std::size_t y_in,
            std::size_t n_in
        ):
            domain_subset(std::move(domain_subset_in)),
            codomain_subset(std::move(codomain_subset_in)),
            y(y_in),
            n(n_in)
        { }

        Domain domain_subset;
        Codomain codomain_subset;
        std::size_t y; // is in set child index
        std::size_t n; // is not in set child index
    };

    template<SetConcept Domain, SetConcept Codomain>
    class FunctionSpace {
        template<SetConcept D, SetConcept C>
        friend class FunctionSpace;

        public:
            FunctionSpace(Domain domain_in = Domain::universal(), Codomain codomain_in = Codomain::universal()):
                domain(std::move(domain_in)),
                codomain(std::move(codomain_in)),
                tree(empty_tree()),
                disjoint_domain_subsets{domain}
            { }

            static FunctionSpace<Domain, Codomain> empty(
                Domain domain_in = Domain::universal(),
                Codomain codomain_in = Codomain::universal()
            ) {
                return {std::move(domain_in), std::move(codomain_in)};
            }

            static FunctionSpace<Domain, Codomain> universal(
                Domain domain = Domain::universal(),
                Codomain codomain = Codomain::universal()
            ) {
                return {
                    domain,
                    codomain,
                    universal_tree(domain, codomain)
                };
            }

            static FunctionSpace<Domain, Codomain> for_all(
                Domain x_in_here,
                Codomain fx_in_here,
                Domain domain = Domain::universal(),
                Codomain codomain = Codomain::universal()
            ) {
                return {
                    domain,
                    codomain,
                    {
                        empty_cylinder(),
                        universal_cylinder(),
                        CylinderSet(x_in_here, fx_in_here, 1, 0)
                    }
                };
            }

            static FunctionSpace<Domain, Codomain> there_exists(
                Domain x_in_here,
                Codomain fx_in_here,
                Domain domain = Domain::universal(),
                Codomain codomain = Codomain:universal()
            ) {
                return {
                    domain,
                    codomain,
                    {
                        CylinderSet(domain, Codomain::empty(), 0, 0),
                        CylinderSet(Domain::empty(), codomain, 1, 1),
                        CylinderSet(x_in_here, x_in_here.issingleton() ? , 0, 1)
                    }
                };
            }

        private:
            Domain domain;
            Codomain codomain;
            std::vector<CylinderSet<Domain, Codomain>> tree;
            std::vector<std::size_t> ancestors; // only needed when unioning or intersecting, move inside relevant function?
            std::vector<Domain> disjoint_domain_subsets;

            FunctionSpace(
                Domain domain_in,
                Codomain codomain_in,
                std::vector<CylinderSet<Domain, Codomain>> tree_in
            ):
                domain(std::move(domain_in)),
                codomain(std::move(codomain_in)),
                tree(std::move(tree_in))
            { }

            static CylinderSet<Domain, Codomain> empty_cylinder(Domain domain, const Codomain& codomain) {
                return {std::move(domain), Codomain::empty(), 0, 0};
            }

            CylinderSet<Domain, Codomain> empty_cylinder(void) const {
                return empty_cylinder(domain, codomain);
            }

            bool is_empty_cylinder(const CylinderSet<Domain, Codomain>& cylinder) {
                !isdisjoint(cylinder.domain_subset, domain) && isdisjoint(cylinder.codomain_subset, codomain);
            }

            static CylinderSet<Domain, Codomain> universal_cylinder(const Domain& domain, Codomain codomain) {
                return {Domain::empty(), codomain, 1, 1};
            }

            CylinderSet<Domain, Codomain> universal_cylinder(void) const {
                return universal_cylinder(domain, codomain);
            }

            bool is_universal_cylinder(const CylinderSet<Domain, Codomain>& cylinder) const {
                isdisjoint(cylinder.domain_subset, domain) && cylinder.codomain_subset >= codomain;
            }

            static std::vector<CylinderSet<Domain, Codomain>> empty_tree(const Domain& domain, const Codomain& codomain) {
                return {
                    empty_cylinder(domain, codomain),
                    universal_cylinder(domain, codomain),
                    empty_cylinder(domain, codomain)
                };
            }

            std::vector<CylinderSet<Domain, Codomain>> empty_tree(void) const {
                return empty_tree(domain, codomain);
            }

            static std::vector<CylinderSet<Domain, Codomain>> universal_tree(const Domain& domain, const Codomain& codomain) {
                return {
                    empty_cylinder(domain, codomain),
                    universal_cylinder(domain, codomain),
                    universal_cylinder(domain, codomain)
                };
            }

            std::vector<CylinderSet<Domain, Codomain>> universal_tree(void) const {
                return universal_tree(domain, codomain);
            }

            void split_node(std::size_t  )

            void split_node(std::size_t i) {
                auto remainder = tree.at(i).domain_subset;
                // for (std::size_t j = )
            }
    }

}

#endif
