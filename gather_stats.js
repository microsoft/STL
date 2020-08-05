// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

const fs = require('fs');

const cliProgress = require('cli-progress');
const dotenv = require('dotenv');
const { DateTime, Duration } = require('luxon');
const { graphql } = require('@octokit/graphql');

{
    const result = dotenv.config();

    if (result.error) {
        throw result.error;
    }

    if (process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN === undefined) {
        throw 'Missing SECRET_GITHUB_PERSONAL_ACCESS_TOKEN key in .env file.';
    }
}

async function retrieve_nodes() {
    const progress = {
        multi_bar: new cliProgress.MultiBar(
            {
                format: '{bar} {percentage}% | ETA: {eta}s | {value}/{total} {name}',
                autopadding: true,
                hideCursor: true,
            },
            cliProgress.Presets.shades_classic
        ),
        pr_bar: null,
        issue_bar: null,
    };

    try {
        const authorized_graphql = graphql.defaults({
            headers: { authorization: `token ${process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN}` },
        });

        const rate_limit = {
            spent: null,
            remaining: null,
            limit: null,
        };

        {
            const query_total_counts = fs.readFileSync('./query_total_counts.graphql', 'utf8');

            const {
                rateLimit,
                repository: {
                    pullRequests: { totalCount: total_prs },
                    issues: { totalCount: total_issues },
                },
            } = await authorized_graphql(query_total_counts);

            rate_limit.spent = rateLimit.cost;
            rate_limit.limit = rateLimit.limit;

            progress.pr_bar = progress.multi_bar.create(total_prs, 0, { name: 'PRs received' });
            progress.issue_bar = progress.multi_bar.create(total_issues, 0, { name: 'issues received' });
        }

        let pr_nodes = [];
        let issue_nodes = [];

        const variables = {
            query: fs.readFileSync('./query_prs_and_issues.graphql', 'utf8'),
            want_prs: true,
            pr_cursor: null,
            want_issues: true,
            issue_cursor: null,
        };

        while (variables.want_prs || variables.want_issues) {
            const {
                rateLimit,
                repository: { pullRequests, issues },
            } = await authorized_graphql(variables);

            rate_limit.spent += rateLimit.cost;
            rate_limit.remaining = rateLimit.remaining;

            if (variables.want_prs) {
                variables.want_prs = pullRequests.pageInfo.hasNextPage;
                variables.pr_cursor = pullRequests.pageInfo.endCursor;
                pr_nodes = pr_nodes.concat(pullRequests.nodes);
                progress.pr_bar.update(pr_nodes.length);
            }

            if (variables.want_issues) {
                variables.want_issues = issues.pageInfo.hasNextPage;
                variables.issue_cursor = issues.pageInfo.endCursor;
                issue_nodes = issue_nodes.concat(issues.nodes);
                progress.issue_bar.update(issue_nodes.length);
            }
        }

        return { pr_nodes: pr_nodes, issue_nodes: issue_nodes, rate_limit: rate_limit };
    } finally {
        progress.multi_bar.stop();
    }
}

function warn_if_pagination_needed(outer_nodes, field, message) {
    if (outer_nodes.length === 0) {
        return;
    }

    let max = outer_nodes[0];

    for (const node of outer_nodes) {
        if (node[field].totalCount > max[field].totalCount) {
            max = node;
        }
    }

    const retrieved = max[field].nodes.length;
    const total_count = max[field].totalCount;

    if (retrieved < total_count) {
        console.log(message(retrieved, total_count, max.number));
    }
}

function read_trimmed_lines(filename) {
    // Excludes empty lines and comments beginning with '#'.
    return fs
        .readFileSync(filename, 'utf8')
        .split('\n')
        .map(line => line.trim())
        .filter(line => line.length > 0 && line[0] !== '#');
}

function warn_about_duplicates(array, message) {
    const uniques = new Set();
    const duplicates = new Set();

    for (const elem of array) {
        if (!uniques.has(elem)) {
            uniques.add(elem);
        } else if (!duplicates.has(elem)) {
            duplicates.add(elem);
            console.log(message(elem));
        }
    }
}

function read_usernames() {
    const maintainer_list = read_trimmed_lines('./usernames_maintainers.txt');
    const contributor_list = read_trimmed_lines('./usernames_contributors.txt');

    warn_about_duplicates(maintainer_list, username => `WARNING: Duplicate maintainer "${username}".`);
    warn_about_duplicates(contributor_list, username => `WARNING: Duplicate contributor "${username}".`);

    const maintainer_set = new Set(maintainer_list);
    const contributor_set = new Set(contributor_list);

    warn_about_duplicates(
        [...maintainer_set, ...contributor_set],
        username => `WARNING: Maintainer "${username}" is also listed as a contributor.`
    );

    return {
        maintainers: maintainer_set,
        contributors: contributor_set,
    };
}

function transform_pr_nodes(pr_nodes) {
    warn_if_pagination_needed(
        pr_nodes,
        'reviews',
        (retrieved, total, number) => `WARNING: Retrieved ${retrieved}/${total} reviews for PR #${number}.`
    );

    const { maintainers, contributors } = read_usernames();

    return pr_nodes.map(pr_node => {
        const maintainer_reviews = pr_node.reviews.nodes
            .filter(review_node => {
                if (review_node.author === null) {
                    return false; // Assume that deleted users were contributors.
                }

                const username = review_node.author.login;

                const is_maintainer = maintainers.has(username);

                if (!is_maintainer && !contributors.has(username)) {
                    contributors.add(username); // Assume that unknown users are contributors.
                    console.log(`WARNING: Unknown user "${username}" reviewed PR #${pr_node.number}.`);
                }

                return is_maintainer;
            })
            .map(review_node => DateTime.fromISO(review_node.submittedAt));

        return {
            number: pr_node.number,
            opened: DateTime.fromISO(pr_node.createdAt),
            closed: DateTime.fromISO(pr_node.closedAt ?? '2100-01-01'),
            merged: DateTime.fromISO(pr_node.mergedAt ?? '2100-01-01'),
            reviews: maintainer_reviews,
        };
    });
}

function calculate_wait(when, opened, reviews) {
    let latest_feedback = opened;

    for (const review of reviews) {
        if (latest_feedback < review && review < when) {
            latest_feedback = review;
        }
    }

    return when.diff(latest_feedback);
}

function transform_issue_nodes(issue_nodes) {
    warn_if_pagination_needed(
        issue_nodes,
        'labels',
        (retrieved, total, number) => `WARNING: Retrieved ${retrieved}/${total} labels for issue #${number}.`
    );
    return issue_nodes.map(node => {
        const labels = node.labels.nodes.map(label => label.name);
        return {
            number: node.number,
            opened: DateTime.fromISO(node.createdAt),
            closed: DateTime.fromISO(node.closedAt ?? '2100-01-01'),
            labeled_cxx20: labels.includes('cxx20'),
            labeled_lwg: labels.includes('LWG') && !labels.includes('vNext') && !labels.includes('blocked'),
            labeled_bug: labels.includes('bug'),
        };
    });
}

function write_daily_table(script_start, all_prs, all_issues) {
    const progress_bar = new cliProgress.SingleBar(
        {
            format: '{bar} {percentage}% | ETA: {eta}s | {value}/{total} days analyzed',
            autopadding: true,
            hideCursor: true,
        },
        cliProgress.Presets.shades_classic
    );

    try {
        let str = '// Copyright (c) Microsoft Corporation.\n';
        str += '// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception\n';
        str += '\n';
        const generated_file_warning_comment = '// Generated file - DO NOT EDIT manually!\n';
        str += generated_file_warning_comment;
        str += 'const daily_table = [\n';

        const begin = DateTime.fromISO('2019-09-05' + 'T23:00:00-07');

        progress_bar.start(Math.ceil(script_start.diff(begin).as('days')), 0);

        for (let when = begin; when < script_start; when = when.plus({ days: 1 })) {
            const one_month_ago = when.minus({ months: 1 });
            let num_merged = 0;
            let num_pr = 0;
            let num_cxx20 = 0;
            let num_lwg = 0;
            let num_issue = 0;
            let num_bug = 0;
            let combined_pr_age = Duration.fromObject({});
            let combined_pr_wait = Duration.fromObject({});

            for (const pr of all_prs) {
                if (one_month_ago < pr.merged && pr.merged < when) {
                    ++num_merged;
                }

                if (when < pr.opened || pr.closed < when) {
                    // This PR wasn't active; do nothing.
                } else {
                    ++num_pr;
                    combined_pr_age = combined_pr_age.plus(when.diff(pr.opened));
                    combined_pr_wait = combined_pr_wait.plus(calculate_wait(when, pr.opened, pr.reviews));
                }
            }

            for (const issue of all_issues) {
                if (when < issue.opened || issue.closed < when) {
                    // This issue wasn't active; do nothing.
                } else if (issue.labeled_cxx20) {
                    // Avoid double-counting C++20 Features and GitHub Issues.
                    ++num_cxx20;
                } else if (issue.labeled_lwg) {
                    // Avoid double-counting LWG Resolutions and GitHub Issues.
                    ++num_lwg;
                } else {
                    ++num_issue;

                    if (issue.labeled_bug) {
                        ++num_bug;
                    }
                }
            }

            const avg_age = num_pr === 0 ? 0 : combined_pr_age.as('days') / num_pr;
            const avg_wait = num_pr === 0 ? 0 : combined_pr_wait.as('days') / num_pr;
            const sum_age = combined_pr_age.as('months');
            const sum_wait = combined_pr_wait.as('months');

            str += '    { ';
            str += [
                `date: '${when.toISODate()}'`,
                `merged: ${num_merged}`,
                `pr: ${num_pr}`,
                `cxx20: ${num_cxx20}`,
                `lwg: ${num_lwg}`,
                `issue: ${num_issue}`,
                `bug: ${num_bug}`,
                `avg_age: ${Number.parseFloat(avg_age).toFixed(2)}`,
                `avg_wait: ${Number.parseFloat(avg_wait).toFixed(2)}`,
                `sum_age: ${Number.parseFloat(sum_age).toFixed(2)}`,
                `sum_wait: ${Number.parseFloat(sum_wait).toFixed(2)}`,
                '},\n',
            ].join(', ');

            progress_bar.increment();
        }

        str += '];\n';
        str += generated_file_warning_comment;

        fs.writeFileSync('./daily_table.js', str);
    } finally {
        progress_bar.stop();
    }
}

async function async_main() {
    try {
        const script_start = DateTime.local();

        const { pr_nodes, issue_nodes, rate_limit } = await retrieve_nodes();

        const all_prs = transform_pr_nodes(pr_nodes);
        const all_issues = transform_issue_nodes(issue_nodes);

        write_daily_table(script_start, all_prs, all_issues);

        const script_finish = DateTime.local();

        console.log(`Spent: ${rate_limit.spent} points`);
        console.log(`Remaining: ${rate_limit.remaining}/${rate_limit.limit} points`);
        console.log(`Time: ${Number.parseFloat(script_finish.diff(script_start).as('seconds')).toFixed(3)}s`);
    } catch (error) {
        console.log(`ERROR: ${error.message}`);
    }
}

async_main();
