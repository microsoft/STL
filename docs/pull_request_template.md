# Description



# Checklist

If you're unsure about a box, leave it unchecked. A maintainer will help you.

If a box isn't applicable, add an explanation in **bold**.
For example: **(N/A: this is a bugfix, not a feature)**

- [ ] I understand README.md. I also understand that acceptance of
  community PRs will be delayed until the test and CI systems are online.
- [ ] If this is a feature addition, that feature has been voted into the
  C++ Working Draft.
- [ ] Identifiers in product code changes are properly `_Ugly` as per
  https://eel.is/c++draft/lex.name#3.1 .
- [ ] The STL builds successfully and all tests have passed (must be manually
  verified by an STL maintainer before CI is online, leave this unchecked for
  initial submission).
- [ ] These changes introduce no known ABI breaks (adding members, renaming
  members, adding virtual functions, changing whether a type is an aggregate
  or trivially copyable, etc.).
- [ ] These changes were written from scratch using only this repository and
  the C++ Working Draft as a reference (and any other cited standards).
  If they were derived from a project that's already listed in NOTICE.txt,
  that's fine, but please mention it. If they were derived from any other
  project (including Boost and libc++, which are not yet listed in
  NOTICE.txt), you *must* mention it here, so we can determine whether the
  license is compatible and what else needs to be done.
