Please note that acceptance of community PRs will be delayed while we are
bringing our test and CI systems online. For more information, see the
README.md.

# Description

# Checklist:

- [ ] I understand README.md.
- [ ] If this is a feature addition, that feature has been voted into the C++
  Working Draft.
- [ ] Any code files edited have been processed by clang-format 8.0.1.
  (The version is important because clang-format's behavior sometimes changes.)
- [ ] Identifiers in any product code changes are properly `_Ugly` as per
  https://eel.is/c++draft/lex.name#3.1 .
- [ ] Identifiers in test code changes are *not* `_Ugly`.
- [ ] Test code includes the correct headers as per the Standard, not just
  what happens to compile.
- [ ] The STL builds and test harnesses have passed (must be manually verified
  by an STL maintainer before CI is online, leave this unchecked for initial
  submission).
- [ ] This change introduces no known ABI breaks (adding members, renaming
  members, adding virtual functions, changing whether a type is an aggregate or
  trivially copyable, etc.). If unsure, leave this box unchecked and ask a
  maintainer for help.
