# STL Status Chart

This branch generates the STL's [Status Chart][].

# Getting Started: Repo

1. Install [Node.js][] 14.9.0 or newer.
    + You can accept all of the installer's default options.
2. Open a new Command Prompt.
    + You can run `node --version` to verify that Node.js was successfully installed.
3. Change to a directory where you'd like a clone of this branch.
4. `git clone --branch gh-pages --config push.default=upstream https://github.com/microsoft/STL.git chart`
    + This will clone into a subdirectory named `chart`; you can choose a different name.
    + `--config push.default=upstream` sets a repository-local option, which will prevent `git push` from failing with
    "fatal: The upstream branch of your current branch does not match the name of your current branch."
5. `cd chart`
6. Replace `octocat` with your GitHub username: `set GH_USER=octocat`
    + This is just to simplify the following instructions.
7. `git remote add --fetch %GH_USER% https://github.com/%GH_USER%/STL.git`
    + This will add your fork as a remote, and then fetch from it.
8. `git push %GH_USER% gh-pages`
    + If you created your fork before the Status Chart was added, this will copy the `gh-pages` branch into your fork.
    + If you created your fork after the Status Chart was added, this will update the `gh-pages` branch in your fork.
9. `git checkout -b my-pages --track %GH_USER%/gh-pages`
    + This will create a branch named `my-pages`; you can choose a different name.
    + You can run `git branch -vv` to see what this has achieved. The remote branches (in the official repo and your
    fork) need to be named `gh-pages` in order to be published via GitHub Pages. Your local `gh-pages` branch tracks
    the official repo, while your local `my-pages` branch tracks your fork.
10. `npm ci`
    + This will download the dependencies listed in `package.json` and locally install them to a `node_modules`
    subdirectory.

# Getting Started: Personal Access Token

GitHub's GraphQL API requires authentication:

1. Go to your [Personal Access Tokens][] on GitHub.
2. Click "Generate new token".
3. Name it "STL Status Chart" or anything else you'd like.
4. Select `repo` scope.
5. Click "Generate token". Keep this page open.
6. In your `chart` repo, create a file named `.env` containing:
    ```
    SECRET_GITHUB_PERSONAL_ACCESS_TOKEN=12ab34cd
    ```
7. Replace `12ab34cd` with the hexadecimal personal access token that you just generated.
    + The token is unique, so it's used without your username.
8. Save the `.env` file.
9. Close the page displaying your personal access token.
10. Clear your clipboard.
11. Verify that `git status` reports "nothing to commit, working tree clean".
    + This indicates that `.env` is being properly ignored.

**The `.env` file now contains a password-equivalent secret - treat it with respect.**

# Updating The Chart

* Run `npm update` to check for updated dependencies. If it finds any, it'll download and locally install them, and
it'll update `package.json` and `package-lock.json` accordingly. `git add` and `git commit` those changes.
* Update `weekly_table.js` by adding a new row.
    + We update it every Friday, although nothing bad will happen if we skip a week or update it on a different day.
    + `vso` is the number of Active work items under the STL's Area Path.
    + `libcxx` is the number of skipped tests in `tests/libcxx/skipped_tests.txt`, excluding "Missing STL Features".
    To determine this number:
        1. Copy the file's contents.
        2. Delete the "Missing STL Features" section.
        3. Sort the remaining lines.
        4. Find the last occurrence of `#`, so you can delete all of the empty lines and comments.
        5. Count the remaining lines.
* Run `node gather_stats.js` to regenerate `daily_table.js` and `monthly_table.js`.
    + This regenerates the files from scratch, but the diff should be small because the data is stable and the process
    is deterministic.
    + It's possible for previous values to change, e.g. if an issue is relabeled, but dramatic changes without
    corresponding generator changes should be investigated.
* Open `index.html` to preview your changes locally.
    + If you've changed how the Status Chart uses Chart.js, open F12 Developer Tools, click on the Console tab, and
    refresh the page to verify that no warnings/errors are displayed.
* After pushing your `my-pages` branch, wait a moment for GitHub Pages to publish, then you can view your changes at
    `https://%GH_USER%.github.io/STL/`.
    + When creating a PR, it's helpful to include that as a "live preview" link.

# License

Copyright (c) Microsoft Corporation.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[Node.js]: https://nodejs.org/en/
[Personal Access Tokens]: https://github.com/settings/tokens
[Status Chart]: https://microsoft.github.io/STL/
