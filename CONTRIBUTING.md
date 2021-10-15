# Contributing

This project welcomes contributions and suggestions. Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

# Contributions that we appreciate

## Submit a pull request that aligns with our goals and priorities

 * Look for pinned issues
 * Check issues under the labels `high priority` and `help wanted`
 * New contributors may also look into issues labeled `good first issue`

## Review a pull request

We welcome reviews from contributors! Especially if this review happens before maintainers' review.
To get an idea about our Code Review process, see:
 * Ongoing reviews in [Pull Requests List](https://github.com/microsoft/STL/pulls)
 * [Advice for Reviewing Pull Requests](https://github.com/microsoft/STL/wiki/Advice-for-Reviewing-Pull-Requests).
 * [Code Review Videos](https://github.com/microsoft/STL/wiki/Code-Review-Videos!)

# PR Checklist

Before submitting a pull request, please ensure that:

* Identifiers in product code changes are properly `_Ugly` as per
  https://eel.is/c++draft/lex.name#3.1 or there are no product code changes.

* These changes introduce no known ABI breaks (adding members, renaming
  members, adding virtual functions, changing whether a type is an aggregate
  or trivially copyable, etc.).

* Your changes are written from scratch using only this repository, the C++
  Working Draft (including any cited standards), other WG21 papers (excluding
  reference implementations outside of proposed standard wording), and LWG
  issues as reference material. If your changes are derived from a project
  that's already listed in NOTICE.txt, that's fine, but please mention it.
  If your changes are derived from any other project, you *must* mention it
  here, so we can determine whether the license is compatible and what else
  needs to be done.

# Code of Conduct

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.
