#ifndef LIBP_INTERNAL_FUNCTION_REGISTER_HPP_GUARD
#define LIBP_INTERNAL_FUNCTION_REGISTER_HPP_GUARD

#include <cassert>
#include <cstddef>
#include <utility>
#include <vector>

namespace libp { inline namespace internal {

    template<class R, class... Args>
    struct get_arity<R(*)(Args...)> : std::integral_constant<std::size_t, sizeof...(Args)> { };

    template<class T>
    struct get_arity : get_arity<decltype(&T::operator())> { };

    template<
        class FunctionType,
        std::size_t Arity = get_arity<FunctionType>::value
    >
    class FunctionRegister {
        using IndexType = typename std::vector<FunctionType>::size_type

        public:
            FunctionRegister(): argument_index_sizes{} { }

            explicit FunctionRegister(const std::array<IndexType, Arity>& argument_index_sizes_in):
                functions([&]() {
                    IndexType functions_size = 1;
                    for (auto s : argument_index_sizes_in) { functions_size *= s; }
                    return s;
                }()),
                argument_index_sizes(argument_index_sizes_in)
            { }

            template<class F>
            void register_function(F&& fn, const std::array<IndexType, Arity>& argument_indices) {
                expand_storage_if_out_of_bounds(argument_indices);
                auto function_index = get_function_index(argument_index_sizes, argument_indices);
                assert(functions[function_index] == FunctionType{});
                functions[function_index] = std::forward<F>(fn);
            }

            template<class F, class... Args>
            auto execute_function(const std::array<IndexType, Arity>& argument_indices, Args&&... args) {
                auto function_index = get_function_index(argument_index_sizes, argument_indices);
                assert(function_index < functions.size());
                return functions[function_index](std::forward<Args>(args)...);
            }

        private:
            std::vector<FunctionType> functions;
            std::array<IndexType, Arity> argument_index_sizes;

            void expand_storage_if_out_of_bounds(const std::array<IndexType, Arity>& argument_indices) {
                for (IndexType i = 0; i != Arity; ++i) {
                    if (argument_indices[i] >= argument_index_sizes[i]) {
                        std::array<IndexType, Arity> new_argument_index_sizes;
                        for (IndexType j = 0; j != Arity; ++j) {
                            new_argument_index_sizes[j] = std::max(argument_indices[j] + 1, argument_index_sizes[i]);
                        }
                        expand_storage(new_argument_index_sizes);
                        return;
                    }
                }
            }

            void expand_storage(const std::array<IndexType, Arity>& new_argument_index_sizes) {
                std::vector<FunctionType> new_functions = functions;

                IndexType new_functions_size = 1;
                for (auto nais : new_argument_index_sizes) { new_functions_size *= nais; }
                new_functions.resize(new_functions_size);

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

            IndexType get_function_index(
                const std::array<IndexType, Arity>& argument_index_sizes,
                const std::array<IndexType, Arity>& argument_indices
            ) {
                IndexType function_index = 0;
                IndexType multiplier = 1;
                for (std::size_t i = 0; i != Arity; ++i) {
                    function_index += multiplier*argument_indices[Arity-1-i];
                    multiplier *= argument_index_sizes[Arity-1-i];
                }
                return function_index;
            }

            std::array<IndexType, Arity> get_argument_indices(
                const std::array<IndexType, Arity>& argument_index_sizes,
                IndexType function_index
            ) {
                IndexType divider = 1;
                for (std::size_t i = 0; i != Arity; ++i) { divider *= argument_index_sizes[i]; }

                IndexType remainder = function_index;

                std::array<IndexType, Arity> argument_indices;
                for (std::size_t i = 0; i != Arity; ++i) {
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

