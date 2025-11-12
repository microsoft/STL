# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# This script generates the partial specializations of _Function_call in <functional>.

def specialization(cv: str, ref: str, ref_inv: str, noex: str, noex_val: str, callable: str) -> str:
    return f"""template <class _Rx, class... _Types>
class _Function_call<_Rx(_Types...) {cv} {ref} {noex}> : public _Function_base<_Rx, {noex_val}, _Types...> {{
public:
    template <class _Vt>
    using _VtInvQuals = {cv} _Vt {ref_inv};

    template <class _Vt>
    static constexpr bool _Is_callable_from = {callable};

    _Rx operator()(_Types... _Args) {cv} {ref} {noex} {{
        return this->_Get_invoke()(&this->_Data, _STD forward<_Types>(_Args)...);
    }}
}};
"""


def ref_permutations(cv: str, noex: str, noex_val: str, trait: str) -> str:
    return specialization(cv, "", "&", noex, noex_val, \
        f"{trait}<_Rx, {cv} _Vt, _Types...> && {trait}<_Rx, {cv} _Vt&, _Types...>") + "\n" \
        + specialization(cv, "&", "&", noex, noex_val, f"{trait}<_Rx, {cv} _Vt&, _Types...>") + "\n" \
        + specialization(cv, "&&", "&&", noex, noex_val, f"{trait}<_Rx, {cv} _Vt, _Types...>")


def cvref_permutations(noex: str, noex_val: str, trait: str) -> str:
    return ref_permutations("", noex, noex_val, trait) + "\n" \
        + ref_permutations("const", noex, noex_val, trait)


if __name__ == "__main__":
    print(cvref_permutations("", "false", "is_invocable_r_v") + "\n" \
        + "#ifdef __cpp_noexcept_function_type" + "\n" \
        + cvref_permutations("noexcept", "true", "is_nothrow_invocable_r_v") \
        + "#endif // defined(__cpp_noexcept_function_type)")
