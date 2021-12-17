# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

def specialization(cv, ref, ref_inv, noex, noex_val, callable):
    print(f"""template <class _Rx, class... _Types>
class _Move_only_function_call<_Rx(_Types...) {cv} {ref} {noex}>
    : public _Move_only_function_base<_Rx, {noex_val}, _Types...> {{
public:
    using result_type = _Rx;

    template <class _Vt>
    using _VtInvQuals = {cv} _Vt {ref_inv};

    template <class _Vt>
    static constexpr bool _Is_callable_from = {callable};

    _Rx operator()(_Types... _Args) {cv} {ref} {noex} {{
        return this->_Get_invoke()(this->_Data, _STD forward<_Types>(_Args)...);
    }}
}};""")

def ref_permutations(cv, noex, noex_val, trait):
    specialization(cv, "", "&", noex, noex_val, \
        f"{trait}<_Rx, {cv} _Vt, _Types...> && {trait}<_Rx, {cv} _Vt&, _Types...>")
    print("")
    specialization(cv, "&", "&", noex, noex_val, f"{trait}<_Rx, {cv} _Vt&, _Types...>")
    print("")
    specialization(cv, "&&", "&&", noex, noex_val, f"{trait}<_Rx, {cv} _Vt, _Types...>")

def cvref_permutations(noex, noex_val, trait):
    ref_permutations("", noex, noex_val, trait)
    print("")
    ref_permutations("const", noex, noex_val, trait)

cvref_permutations("", "false", "is_invocable_r_v")
print("")
print("#ifdef __cpp_noexcept_function_type")
cvref_permutations("noexcept", "true", "is_nothrow_invocable_r_v")
print("#endif // __cpp_noexcept_function_type")
