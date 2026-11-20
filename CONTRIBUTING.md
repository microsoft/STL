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

Before submitting a pull request, please ensure that:

* Any AI-generated code has been clearly disclosed in your PR description.
  We strongly discourage AI-generated changes to product code because the STL
  is a highly unusual codebase with complex correctness and performance
  requirements imposed by the Standard, and highly unusual code patterns that
  don't look like typical codebases. Our intensive code review process
  (to avoid bugs) is intentionally a bottleneck, so we ask that you be
  considerate of the time and effort that we'll need to spend, by starting
  with changes that you fully understand. Test code has weaker requirements,
  so AI-generated changes for tests can be acceptable if properly disclosed.

* Your PR description explains your intent behind the changes.
  We discourage AI-generated PR descriptions because we can read the changes,
  and we have access to the same AI tools that you have, so we can get
  an AI-generated summary if we want.

* Identifiers in product code changes are properly `_Ugly` as per
  [\[lex.name\]/4.1][standard:ugly] or there are no product code changes.

* These changes introduce no known ABI breaks (adding members, renaming
  members, adding virtual functions, changing whether a type is an aggregate
  or trivially copyable, etc.).

* Your changes are written from scratch using only this repository, the C++
  Working Draft (including any cited standards), other WG21 papers (excluding
  reference implementations outside of proposed standard wording), and LWG
  issues as reference material. If your changes are derived from a project
  that's already listed in [NOTICE.txt][], that's fine, but please mention it.
  If your changes are derived from any other project, you *must* mention it
  here, so we can determine whether the license is compatible and what else
  needs to be done.

# Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).

See [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) for more information.

[label:"good first issue"]:
   https://github.com/microsoft/STL/issues?q=is%3Aopen+is%3Aissue+label%3A%22good+first+issue%22
[label:"high priority"]: https://github.com/microsoft/STL/issues?q=is%3Aopen+is%3Aissue+label%3A%22high+priority%22
[label:"help wanted"]: https://github.com/microsoft/STL/issues?q=is%3Aopen+is%3Aissue+label%3A%22help+wanted%22
[review:changes-requested]: https://github.com/microsoft/STL/pulls?q=is%3Apr+is%3Aopen+review%3Achanges-requested
[wiki:advice-for-reviewing]: https://github.com/microsoft/STL/wiki/Advice-for-Reviewing-Pull-Requests
[NOTICE.txt]: https://github.com/microsoft/STL/blob/main/NOTICE.txt
[wiki:videos]: https://github.com/microsoft/STL/wiki/Code-Review-Videos
[standard:ugly]: https://eel.is/c++draft/lex.name#4.1
