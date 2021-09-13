#ifndef LIBP_INTERNAL_FUNCTION_REGISTER_HPP_GUARD
#define LIBP_INTERNAL_FUNCTION_REGISTER_HPP_GUARD

#include <cassert>
#include <cstddef>
#include <sstream>
#include <utility>
#include <vector>

namespace libp { inline namespace internal {

    template<class T>
    struct get_arity : get_arity<decltype(&T::operator())> { };

    template<class R, class... Args>
    struct get_arity<R(*)(Args...)> : std::integral_constant<std::size_t, sizeof...(Args)> { };

    template<
        class FunctionType,
        std::size_t IndexDimension = get_arity<FunctionType>::value
    >
    class FunctionRegister {
        using IndexType = typename std::vector<FunctionType>::size_type;

        public:
            FunctionRegister():
                argument_index_sizes{},
                default_function{}
            { }

            FunctionRegister(
                const std::array<IndexType, IndexDimension>& argument_index_sizes_in,
                FunctionType default_function_in = FunctionType{}
            ):
                argument_index_sizes(argument_index_sizes_in),
                default_function(std::move(default_function_in)),
                functions(
                    [&]() {
                        IndexType functions_size = 1;
                        for (auto s : argument_index_sizes) { functions_size *= s; }
                        return functions_size;
                    }(),
                    default_function
                )
            { }

            template<class F>
            void register_function(const std::array<IndexType, IndexDimension>& argument_indices, F&& fn) {
                expand_storage_if_out_of_bounds(argument_indices);
                functions[get_function_index(argument_index_sizes, argument_indices)] = std::forward<F>(fn);
            }

            auto execute_function(const std::array<IndexType, IndexDimension>& argument_indices) {
                auto function_index = get_function_index(argument_index_sizes, argument_indices);
                if (function_index < functions.size()) {
                    return functions[function_index]();
                } else {
                    return default_function();
                }
            }

            template<class... Args>
            auto execute_function(const std::array<IndexType, IndexDimension>& argument_indices, Args&&... args) {
                auto function_index = get_function_index(argument_index_sizes, argument_indices);
                if (function_index < functions.size()) {
                    return functions[function_index](std::forward<Args>(args)...);
                } else {
                    return default_function(std::forward<Args>(args)...);
                }
            }

        private:
            std::array<IndexType, IndexDimension> argument_index_sizes;
            FunctionType default_function;
            std::vector<FunctionType> functions;

            void expand_storage_if_out_of_bounds(const std::array<IndexType, IndexDimension>& argument_indices) {
                auto get_storage_supply = [&](IndexType j) {
                    IndexType argument_index_sizes_j = std::max<IndexType>(argument_index_sizes[j], 1);
                    while(argument_indices[j] >= argument_index_sizes_j) {
                        // Increase storage supply by a factor of two along each dimension
                        // each time an increase is needed, to avoid having to expand often.
                        argument_index_sizes_j *= 2;
                    }
                    return argument_index_sizes_j;
                };

                // Less work needs to be done if only the first argument index size changes,
                // so check all the other argument indices first.
                for (IndexType i = 1; i < IndexDimension; ++i) {
                    if (argument_indices[i] >= argument_index_sizes[i]) {
                        std::array<IndexType, IndexDimension> new_argument_index_sizes;
                        for (IndexType j = 0; j != IndexDimension; ++j) {
                            new_argument_index_sizes[j] = get_storage_supply(j);
                        }
                        expand_storage(new_argument_index_sizes);
                        return;
                    }
                }

                if (argument_indices[0] >= argument_index_sizes[0]) {
                    expand_storage(get_storage_supply(0));
                }
            }

            void expand_storage(std::array<IndexType, IndexDimension> new_argument_index_sizes) {
                std::vector<FunctionType> new_functions = functions;

                IndexType new_functions_size = 1;
                for (auto nais : new_argument_index_sizes) { new_functions_size *= nais; }
                new_functions.resize(new_functions_size, default_function);

                auto old_functions_size = functions.size();
                for (std::size_t i = 0; i != old_functions_size; ++i) {
                    IndexType old_functions_index = old_functions_size - 1 - i;
                    
                    auto new_functions_index = get_function_index(
                        new_argument_index_sizes,
                        get_argument_indices(argument_index_sizes, old_functions_index)
                    );

                    std::swap(new_functions[old_functions_index], new_functions[new_functions_index]);
                }

                std::swap(functions, new_functions);
                std::swap(argument_index_sizes, new_argument_index_sizes);
            }

            // An increase in the number of indices on the first argument does not change
            // the map between argument indices and the function index, so we do not
            // need to move functions to their new storage locations in this case.
            void expand_storage(IndexType new_argument_0_index_size) {
                IndexType new_functions_size = new_argument_0_index_size;
                for (std::size_t i = 1; i < IndexDimension; ++i) {
                    new_functions_size *= argument_index_sizes[i];
                }
                functions.resize(new_functions_size, default_function);
                argument_index_sizes[0] = new_argument_0_index_size;
            }

            IndexType get_function_index(
                const std::array<IndexType, IndexDimension>& argument_index_sizes,
                const std::array<IndexType, IndexDimension>& argument_indices
            ) {
                IndexType function_index = 0;
                IndexType multiplier = 1;
                for (std::size_t i = 0; i != IndexDimension; ++i) {
                    function_index += multiplier*argument_indices[IndexDimension-1-i];
                    multiplier *= argument_index_sizes[IndexDimension-1-i];
                }
                return function_index;
            }

            std::array<IndexType, IndexDimension> get_argument_indices(
                const std::array<IndexType, IndexDimension>& argument_index_sizes,
                IndexType function_index
            ) {
                IndexType divider = 1;
                for (std::size_t i = 0; i != IndexDimension; ++i) { divider *= argument_index_sizes[i]; }

                IndexType remainder = function_index;

                std::array<IndexType, IndexDimension> argument_indices;
                for (std::size_t i = 0; i != IndexDimension; ++i) {
                    divider /= argument_index_sizes[i];
                    IndexType ai = remainder/divider;
                    remainder -= ai*divider;
                    argument_indices[i] = ai;
                }

                return argument_indices;
            }
    };

}}

#endif

