# Contributing

This project welcomes contributions and suggestions. Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

## Types of Contributions

### Submitting a Pull Request

The STL repo has many open issues that track work which needs to be completed.
If you're unsure of where to start, you may want to:

* look for pinned issues, or
* check issues under the labels [`good first issue`][label:"good first issue"],
  [`high priority`][label:"high priority"], or [`help wanted`][label:"help wanted"].

### Reviewing a Pull Request

We love code reviews from contributors! Reviews from other contributors can often accelerate the reviewing process
by helping a PR reach a more finished state before maintainers review the changes. As a result, such a PR may require
fewer maintainer review iterations before reaching a "Ready to Merge" state.

To gain insight into our Code Review process, you can check out:

* pull requests which are [undergoing review][review:changes-requested],
* [Advice for Reviewing Pull Requests][wiki:advice-for-reviewing], and
* [Code Review Videos][wiki:videos].

## PR Checklist

Before submitting a pull request, please ensure:

1. Any non-standard identifiers in product code (including local variables, exposition-only members, etc.)
   begin with an underscore and a capital letter, as per [\[lex.name\]/3.1][].

2. Your changes don't affect the ABI for any type or function that a user may use (including adding or removing
   non-static data members, adding or removing virtual member functions, changing whether a type is an aggregate
   or trivially copyable, etc.).

3. Your changes are written from scratch using only acceptable sources:
   * this repository,
   * the C++ Working Draft (including any cited standards),
   * other WG21 papers (excluding reference implementations outside of proposed standard wording),
   * LWG issues, or
   * a project listed in [NOTICE.txt][] (make sure to cite the project in the PR description!).

If your changes are derived from any other project, you _must_ mention it in the pull request description,
so we can determine whether the license is compatible and whether any other steps need to be taken.

# Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

[label:"good first issue"]:
   https://github.com/microsoft/STL/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22
[label:"high priority"]: https://github.com/microsoft/STL/issues?q=is%3Aopen+is%3Aissue+label%3A%22high+priority%22
[label:"help wanted"]: https://github.com/microsoft/STL/issues?q=is%3Aopen+is%3Aissue+label%3A%22help+wanted%22
[review:changes-requested]: https://github.com/microsoft/STL/pulls?q=is%3Apr+is%3Aopen+review%3Achanges-requested
[wiki:advice-for-reviewing]: https://github.com/microsoft/STL/wiki/Advice-for-Reviewing-Pull-Requests
[NOTICE.txt]: https://github.com/microsoft/STL/blob/main/NOTICE.txt
[wiki:videos]: https://github.com/microsoft/STL/wiki/Code-Review-Videos
[\[lex.name\]/3.1]: https://eel.is/c++draft/lex.name#3.1
