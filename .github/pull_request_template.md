Description
===========



Checklist
=========

Be sure you've read README.md and understand the scope of this repo.

If you're unsure about a box, leave it unchecked. A maintainer will help you.

- [ ] Identifiers in product code changes are properly `_Ugly` as per
  https://eel.is/c++draft/lex.name#3.1 or there are no product code changes.
- [ ] The STL builds successfully and all tests have passed (must be manually
  verified by an STL maintainer before automated testing is enabled on GitHub,
  leave this unchecked for initial submission).
- [ ] These changes introduce no known ABI breaks (adding members, renaming
  members, adding virtual functions, changing whether a type is an aggregate
  or trivially copyable, etc.).
- [ ] These changes were written from scratch using only this repository,
  the C++ Working Draft (including any cited standards), other WG21 papers
  (excluding reference implementations outside of proposed standard wording),
  and LWG issues as reference material. If they were derived from a project
  that's already listed in NOTICE.txt, that's fine, but please mention it.
  If they were derived from any other project (including Boost and libc++,
  which are not yet listed in NOTICE.txt), you *must* mention it here,
  so we can determine whether the license is compatible and what else needs
  to be done.
