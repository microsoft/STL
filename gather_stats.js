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
    let progress = {
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

        let rate_limit = {
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

        let variables = {
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

function transform_pr_nodes(pr_nodes) {
    return pr_nodes.map(node => ({
        number: node.number,
        opened: DateTime.fromISO(node.createdAt),
        closed: DateTime.fromISO(node.closedAt ?? '2100-01-01'),
    }));
}

function transform_issue_nodes(issue_nodes) {
    return issue_nodes.map(node => {
        const labels = node.labels.nodes.map(label => label.name);
        const total_count = node.labels.totalCount;
        if (labels.length < total_count) {
            console.log(`WARNING: Retrieved ${labels.length}/${total_count} labels for issue #${node.number}.`);
        }
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
            let num_pr = 0;
            let num_cxx20 = 0;
            let num_lwg = 0;
            let num_issue = 0;
            let num_bug = 0;
            let combined_pr_age = Duration.fromObject({});

            for (const pr of all_prs) {
                if (when < pr.opened || pr.closed < when) {
                    // This PR wasn't active; do nothing.
                } else {
                    ++num_pr;
                    combined_pr_age = combined_pr_age.plus(when.diff(pr.opened));
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

            str += '    { ';
            str += [
                `date: '${when.toISODate()}'`,
                `pr: ${num_pr}`,
                `cxx20: ${num_cxx20}`,
                `lwg: ${num_lwg}`,
                `issue: ${num_issue}`,
                `bug: ${num_bug}`,
                `months: ${Number.parseFloat(combined_pr_age.as('months')).toFixed(2)}`,
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
