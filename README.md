# STL Status Chart

This branch generates the STL's [Status Chart][].

# Getting Started

## Repo

1. Install [Node.js][] 20.0.0 or newer.
    + You can accept all of the installer's default options.
2. Open a new Command Prompt.
    + You can run `node --version` to verify that Node.js was successfully installed.
3. Change to a directory where you'd like a clone of this branch.
4. `git clone --branch gh-pages --single-branch --config push.default=upstream https://github.com/microsoft/STL.git chart`
    + This will clone into a subdirectory named `chart`; you can choose a different name.
    + `--config push.default=upstream` sets a repository-local option, which will prevent `git push` from failing with
    "fatal: The upstream branch of your current branch does not match the name of your current branch."
5. `cd chart`
6. Set the environment variable `GH_USER` to your GitHub username: `set GH_USER=octocat` (Replace `octocat` with your
    GitHub username.)
    + This is just to simplify the following instructions.
7. `git remote add --track gh-pages --fetch %GH_USER% https://github.com/%GH_USER%/STL.git`
    + This will add your fork as a remote, and then fetch from it.
    + If you created your fork before the Status Chart was added, you can omit `--track gh-pages`.
8. `git push %GH_USER% gh-pages`
    + If you created your fork before the Status Chart was added, this will copy the `gh-pages` branch into your fork.
    + If you created your fork after the Status Chart was added, this will update the `gh-pages` branch in your fork.
9. `git checkout -b my-pages --track %GH_USER%/gh-pages`
    + This will create a branch named `my-pages`; you can choose a different name.
    + You can run `git branch -vv` to see what this has achieved. By default, the remote branches (in the official repo
    and your fork) need to be named `gh-pages` in order to be published via GitHub Pages. Your local `gh-pages` branch
    tracks the official repo, while your local `my-pages` branch tracks your fork.
10. `npm ci`
    + This will download the dependencies listed in `package.json` and locally install them to a `node_modules`
    subdirectory.

## GitHub Pages

Enable GitHub Pages for your fork:

1. Click the Settings tab.
2. Click the Pages vertical tab.
3. Set the source branch to `gh-pages`.
    + The default folder `/ (root)` is correct.
4. Click Save.

## Personal Access Token

GitHub's GraphQL API requires authentication:

1. Go to your [Personal Access Tokens][] on GitHub.
2. Click "Generate new token".
3. Name it "STL Status Chart" or anything else you'd like.
4. Don't select any scopes. (This will be a "public access" token.)
5. Click "Generate token". Keep this page open.
6. In your `chart` repo, create a file named `.env` containing:
    ```
    SECRET_GITHUB_PERSONAL_ACCESS_TOKEN=ghp_abcdABCD0123wxyzWXYZ6789
    ```
7. Replace `ghp_abcdABCD0123wxyzWXYZ6789` with the personal access token that you just generated.
    + The token is unique, so it's used without your username.
    + The [prefix][token-formats] will be `ghp_`.
8. Save the `.env` file.
9. Close the page displaying your personal access token.
10. Clear your clipboard.
11. Verify that `git status` reports "nothing to commit, working tree clean".
    + This indicates that `.env` is being properly ignored.

**The `.env` file now contains a secret - treat it with respect.**

# Updating The Chart

* Run `npm update` to check for updated dependencies. If it finds any, it'll download and locally install them, and
it'll update `package.json` and `package-lock.json` accordingly. `git add` and `git commit` those changes.
    + `npm update` won't install new major versions. To do that, run
    `npm install [package1]@latest [package2]@latest [...]` for all of the packages listed
    as `dependencies` in `package.json` (e.g. `@octokit/graphql@latest dotenv@latest`).
    + Ensure that the `importmap` in `index.html` remains synchronized to the same versions.
* Update other dependencies in `index.html` (e.g. Primer CSS and es-module-shims).
* Update `weekly_table.ts` by adding a new row.
    + We update it every Friday, although nothing bad will happen if we skip a week or update it on a different day.
    + `vso` is the number of Active work items under the STL's Area Path.
    + `libcxx` is the number of skipped/failing tests in `tests/libcxx/expected_results.txt`, excluding
    "Missing STL Features".
    To determine this number:
        1. Copy the file's contents.
        2. Delete the "Missing STL Features" section.
        3. Sort the remaining lines.
        4. Find the last occurrence of `#`, so you can delete all of the empty lines and comments.
        5. Count the remaining lines.
* Update `video_table.ts` if any [code review videos][] have been recorded or uploaded.
* Run `npm run gather` to compile `gather_stats.ts` and then regenerate `daily_table.ts` and `monthly_table.ts`.
    + This regenerates the files from scratch, but the diff should be small because the data is stable and the process
    is deterministic.
    + It's possible for previous values to change, e.g. if an issue is relabeled, but dramatic changes without
    corresponding generator changes should be investigated.
    + Automated updates (controlled by `.github/workflows/update-status-chart.yml` in the main repo)
    will regenerate these files, so you generally don't need to manually update them in PRs.
* Run `npm run make` to compile `status_chart.ts` and then bundle it and the tables into `built/status_chart.mjs`.
    + Automated updates will also regenerate this file. However, you'll need to manually update
    `built/status_chart.mjs` if you're making synchronized changes to `index.html`.
* Run `npm run view` to preview your changes locally.
    + Directly opening `index.html` won't work due to how JavaScript modules behave.
    + If you've changed how the Status Chart uses [Chart.js][], open F12 Developer Tools, click on the Console tab, and
    refresh the page to verify that no warnings/errors are displayed.
* After pushing your `my-pages` branch, wait a moment for GitHub Pages to publish, then you can view your changes at
    `https://%GH_USER%.github.io/STL/`.
    + When creating a PR, it's helpful to include that as a "live preview" link.
* If you're manually updating generated files, remember to merge `gh-pages` into your `my-pages` branch and regenerate
    the files before completing your PR.

# License

Copyright (c) Microsoft Corporation.

SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

[Chart.js]: https://www.chartjs.org/
[code review videos]: https://github.com/microsoft/STL/wiki/Code-Review-Videos
[Node.js]: https://nodejs.org/en/
[Personal Access Tokens]: https://github.com/settings/tokens
[Status Chart]: https://microsoft.github.io/STL/
[token-formats]: https://github.blog/2021-04-05-behind-githubs-new-authentication-token-formats/
