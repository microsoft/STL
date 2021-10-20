// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import * as fs from 'fs';

import * as cliProgress from 'cli-progress';
import * as dotenv from 'dotenv';
import { DateTime, Duration, Settings } from 'luxon';
import { graphql } from '@octokit/graphql';
import * as yargs from 'yargs';

Settings.defaultZone = 'America/Los_Angeles';

if (process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN === undefined) {
    // GitHub Actions will provide the PAT as an environment variable. Otherwise, we need to load the .env file.

    const result = dotenv.config();

    if (result.error) {
        throw result.error;
    }

    if (process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN === undefined) {
        throw new Error('Missing SECRET_GITHUB_PERSONAL_ACCESS_TOKEN key in .env file.');
    }
}

const argv = yargs(process.argv.slice(2))
    .option('save-file', {
        alias: 's',
        description: 'Save the GraphQL result to a file.',
        requiresArg: true,
        type: 'string',
    })
    .option('load-file', {
        alias: 'l',
        description: 'Load the GraphQL result from a file.',
        requiresArg: true,
        type: 'string',
    })
    .conflicts('save-file', 'load-file')
    .check((argv, _options) => {
        if (argv._.length > 0) {
            throw new Error(`Unknown arguments: ${argv._.join(' ')}`);
        } else {
            return true;
        }
    })
    .strict()
    .parseSync();

type RawLabelNode = {
    name: string;
};

type RawLabels = {
    totalCount: number;
    nodes: RawLabelNode[];
};

type RawReviewNode = {
    author: {
        login: string;
    };
    submittedAt: string;
};

type RawReviews = {
    totalCount: number;
    nodes: RawReviewNode[];
};

type RawPRNode = {
    id: number;
    createdAt: string;
    closedAt: string;
    mergedAt: string;
    labels: RawLabels;
    reviews: RawReviews;
};

type RawIssueNode = {
    id: number;
    createdAt: string;
    closedAt: string;
    labels: RawLabels;
};

async function retrieve_nodes_from_network() {
    const progress = {
        multi_bar: new cliProgress.MultiBar(
            {
                format: '{bar} {percentage}% | ETA: {eta}s | {value}/{total} {name}',
                autopadding: true,
                hideCursor: true,
                noTTYOutput: process.stderr.isTTY !== true,
            },
            cliProgress.Presets.shades_classic
        ),
        pr_bar: null as null | cliProgress.SingleBar,
        issue_bar: null as null | cliProgress.SingleBar,
    };

    try {
        const authorized_graphql = graphql.defaults({
            headers: { authorization: `token ${process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN}` },
        });

        const rate_limit = {
            spent: null as null | number,
            remaining: null as null | number,
            limit: null as null | number,
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

        let pr_nodes: RawPRNode[] = [];
        let issue_nodes: RawIssueNode[] = [];

        const variables = {
            query: fs.readFileSync('./query_prs_and_issues.graphql', 'utf8'),
            want_prs: true,
            pr_cursor: null as null | string,
            want_issues: true,
            issue_cursor: null as null | string,
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

async function retrieve_nodes() {
    if (argv['load-file']) {
        return JSON.parse(fs.readFileSync(argv['load-file'], 'utf8'));
    }

    const graphql_result = await retrieve_nodes_from_network();

    if (argv['save-file']) {
        fs.writeFileSync(argv['save-file'], JSON.stringify(graphql_result));
    }

    return graphql_result;
}

type PaginationWarningMessage = (retrieved: number, total: number, id: number) => string;

function warn_if_pagination_needed<RawNode extends RawPRNode | RawIssueNode>(
    outer_nodes: RawNode[],
    get_field: (node: RawNode) => RawLabels | RawReviews,
    message: PaginationWarningMessage
) {
    if (outer_nodes.length === 0) {
        return;
    }

    let max = outer_nodes[0];

    for (const node of outer_nodes) {
        if (get_field(node).totalCount > get_field(max).totalCount) {
            max = node;
        }
    }

    const retrieved = get_field(max).nodes.length;
    const total_count = get_field(max).totalCount;

    if (retrieved < total_count) {
        console.log(message(retrieved, total_count, max.id));
    }
}

function read_trimmed_lines(filename: string) {
    // Excludes empty lines and comments beginning with '#'.
    return fs
        .readFileSync(filename, 'utf8')
        .split('\n')
        .map(line => line.trim())
        .filter(line => line.length > 0 && line[0] !== '#');
}

type DuplicateWarningMessage = (username: string) => string;

function warn_about_duplicates(array: string[], message: DuplicateWarningMessage) {
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

    warn_about_duplicates(maintainer_list, (username: string) => `WARNING: Duplicate maintainer "${username}".`);
    warn_about_duplicates(contributor_list, (username: string) => `WARNING: Duplicate contributor "${username}".`);

    const maintainer_set = new Set(maintainer_list);
    const contributor_set = new Set(contributor_list);

    warn_about_duplicates(
        [...maintainer_set, ...contributor_set],
        (username: string) => `WARNING: Maintainer "${username}" is also listed as a contributor.`
    );

    return {
        maintainers: maintainer_set,
        contributors: contributor_set,
    };
}

type CookedPRNode = {
    id: number;
    opened: DateTime;
    closed: DateTime;
    merged: DateTime;
    reviews: DateTime[];
};

function transform_pr_nodes(pr_nodes: RawPRNode[]): CookedPRNode[] {
    warn_if_pagination_needed(
        pr_nodes,
        (node: RawPRNode) => node.labels,
        (retrieved: number, total: number, id: number) =>
            `WARNING: Retrieved ${retrieved}/${total} labels for PR #${id}.`
    );
    warn_if_pagination_needed(
        pr_nodes,
        (node: RawPRNode) => node.reviews,
        (retrieved: number, total: number, id: number) =>
            `WARNING: Retrieved ${retrieved}/${total} reviews for PR #${id}.`
    );

    const { maintainers, contributors } = read_usernames();

    return pr_nodes
        .filter(pr_node => {
            const labels = pr_node.labels.nodes.map(label => label.name);
            return !labels.includes('uncharted');
        })
        .map(pr_node => {
            const maintainer_reviews = pr_node.reviews.nodes
                .filter(review_node => {
                    if (review_node.author === null) {
                        return false; // Assume that deleted users were contributors.
                    }

                    const username = review_node.author.login;

                    const is_maintainer = maintainers.has(username);

                    if (!is_maintainer && !contributors.has(username)) {
                        contributors.add(username); // Assume that unknown users are contributors.
                        console.log(`WARNING: Unknown user "${username}" reviewed PR #${pr_node.id}.`);
                    }

                    return is_maintainer;
                })
                .map(review_node => DateTime.fromISO(review_node.submittedAt));

            return {
                id: pr_node.id,
                opened: DateTime.fromISO(pr_node.createdAt),
                closed: DateTime.fromISO(pr_node.closedAt ?? '2100-01-01'),
                merged: DateTime.fromISO(pr_node.mergedAt ?? '2100-01-01'),
                reviews: maintainer_reviews,
            };
        });
}

function calculate_wait(when: DateTime, opened: DateTime, reviews: DateTime[]) {
    let latest_feedback = opened;

    for (const review of reviews) {
        if (latest_feedback < review && review < when) {
            latest_feedback = review;
        }
    }

    return when.diff(latest_feedback);
}

type CookedIssueNode = {
    id: number;
    opened: DateTime;
    closed: DateTime;
    labeled_cxx20: boolean;
    labeled_cxx23: boolean;
    labeled_lwg: boolean;
    labeled_bug: boolean;
};

function transform_issue_nodes(issue_nodes: RawIssueNode[]): CookedIssueNode[] {
    warn_if_pagination_needed(
        issue_nodes,
        (node: RawIssueNode) => node.labels,
        (retrieved: number, total: number, id: number) =>
            `WARNING: Retrieved ${retrieved}/${total} labels for issue #${id}.`
    );
    return issue_nodes.map(node => {
        const labels = node.labels.nodes.map(label => label.name);
        return {
            id: node.id,
            opened: DateTime.fromISO(node.createdAt),
            closed: DateTime.fromISO(node.closedAt ?? '2100-01-01'),
            labeled_cxx20: labels.includes('cxx20'),
            labeled_cxx23: labels.includes('cxx23'),
            labeled_lwg: labels.includes('LWG') && !labels.includes('vNext') && !labels.includes('blocked'),
            labeled_bug: labels.includes('bug'),
        };
    });
}

function calculate_sliding_window(when: DateTime, merged: DateTime) {
    // A sliding window of 30 days would be simple, but would result in a noisy line as PRs abruptly leave the window.
    // To reduce such noise, this function applies smoothing between 20 and 40 days.
    // (A range of 25 to 35 days would also work; the important thing is for the integral of this function to be 30,
    // so we can still describe this metric as 'Monthly Merged PRs'.

    const days_ago = when.diff(merged).as('days');

    if (days_ago < 0) {
        return 0; // PR was merged in the future
    } else if (days_ago < 20) {
        return 1; // PR was merged between 0 and 20 days ago
    } else if (days_ago < 40) {
        return (40 - days_ago) / 20; // PR was merged between 20 and 40 days ago; decrease weight from 1 to 0
    } else {
        return 0; // PR was merged in the ancient past
    }
}

function write_generated_file(filename: string, table_str: string) {
    const generated_file_warning_comment = '// Generated file - DO NOT EDIT manually!\n';

    let str = '// Copyright (c) Microsoft Corporation.\n';
    str += '// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception\n\n';
    str += generated_file_warning_comment;
    str += "'use strict';\n";
    str += table_str;
    str += generated_file_warning_comment;

    fs.writeFileSync(filename, str);
}

type Row = {
    date: DateTime;
    merged: number;
    pr: number;
    cxx20: number;
    cxx23: number;
    lwg: number;
    issue: number;
    bug: number;
    avg_age: number;
    avg_wait: number;
    sum_age: number;
    sum_wait: number;
};

function should_emit_data_point(rows: Row[], i: number, key: keyof Row) {
    return rows[i - 1]?.[key] > 0 || rows[i][key] > 0 || rows[i + 1]?.[key] > 0;
}

function write_daily_table(script_start: DateTime, all_prs: CookedPRNode[], all_issues: CookedIssueNode[]) {
    const rows: Row[] = [];

    const progress_bar = new cliProgress.SingleBar(
        {
            format: '{bar} {percentage}% | ETA: {eta}s | {value}/{total} days analyzed',
            autopadding: true,
            hideCursor: true,
            noTTYOutput: process.stderr.isTTY !== true,
        },
        cliProgress.Presets.shades_classic
    );

    try {
        const begin = DateTime.fromISO('2019-09-05' + 'T23:00:00');

        progress_bar.start(Math.ceil(script_start.diff(begin).as('days')), 0);

        for (let when = begin; when < script_start; when = when.plus({ days: 1 })) {
            let num_merged = 0;
            let num_pr = 0;
            let num_cxx20 = 0;
            let num_cxx23 = 0;
            let num_lwg = 0;
            let num_issue = 0;
            let num_bug = 0;
            let combined_pr_age = Duration.fromObject({});
            let combined_pr_wait = Duration.fromObject({});

            for (const pr of all_prs) {
                num_merged += calculate_sliding_window(when, pr.merged);

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
                } else if (issue.labeled_cxx23) {
                    // Avoid double-counting C++23 Features and GitHub Issues.
                    ++num_cxx23;
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

            rows.push({
                date: when,
                merged: num_merged,
                pr: num_pr,
                cxx20: num_cxx20,
                cxx23: num_cxx23,
                lwg: num_lwg,
                issue: num_issue,
                bug: num_bug,
                avg_age: num_pr === 0 ? 0 : combined_pr_age.as('days') / num_pr,
                avg_wait: num_pr === 0 ? 0 : combined_pr_wait.as('days') / num_pr,
                sum_age: combined_pr_age.as('months'),
                sum_wait: combined_pr_wait.as('months'),
            });

            progress_bar.increment();
        }
    } finally {
        progress_bar.stop();
    }

    let str = 'const daily_table = [\n';

    for (let i = 0; i < rows.length; ++i) {
        const row = rows[i];
        str += '    { ';
        str += `date: '${row.date.toISODate()}', `;
        str += `merged: ${row.merged.toFixed(2)}, `;

        const keys: (keyof Row)[] = ['pr', 'cxx20', 'cxx23', 'lwg', 'issue', 'bug'];
        for (const key of keys) {
            if (should_emit_data_point(rows, i, key)) {
                str += `${key}: ${row[key]}, `;
            } else {
                str += `${key}: null, `;
            }
        }

        str += `avg_age: ${row.avg_age.toFixed(2)}, `;
        str += `avg_wait: ${row.avg_wait.toFixed(2)}, `;
        str += `sum_age: ${row.sum_age.toFixed(2)}, `;
        str += `sum_wait: ${row.sum_wait.toFixed(2)}, `;
        str += '},\n';
    }

    str += '];\n';

    write_generated_file('./daily_table.js', str);
}

function write_monthly_table(script_start: DateTime, all_prs: CookedPRNode[]) {
    const monthly_merges = new Map();

    for (const pr of all_prs) {
        const year_month = pr.merged.toFormat('yyyy-MM');
        const old_value = monthly_merges.get(year_month) ?? 0;
        monthly_merges.set(year_month, old_value + 1);
    }

    let str = 'const monthly_table = [\n';

    // Analyze complete months.
    const begin = DateTime.fromISO('2019-10-01');
    for (let when = begin; when < script_start.startOf('month'); when = when.plus({ months: 1 })) {
        const year_month = when.toFormat('yyyy-MM');
        const value = monthly_merges.get(year_month) ?? 0;

        str += '    { ';
        str += [
            `date: '${year_month}-16'`, // position each bar in the middle of each month
            `merge_bar: ${value}`,
            '},\n',
        ].join(', ');
    }

    str += '];\n';

    write_generated_file('./monthly_table.js', str);
}

async function async_main() {
    try {
        const script_start = DateTime.local();

        const { pr_nodes, issue_nodes, rate_limit } = await retrieve_nodes();

        const all_prs = transform_pr_nodes(pr_nodes);
        const all_issues = transform_issue_nodes(issue_nodes);

        write_daily_table(script_start, all_prs, all_issues);
        write_monthly_table(script_start, all_prs);

        const script_finish = DateTime.local();

        if (!argv['load-file']) {
            console.log(`Spent: ${rate_limit.spent} points`);
            console.log(`Remaining: ${rate_limit.remaining}/${rate_limit.limit} points`);
        }

        console.log(`Time: ${script_finish.diff(script_start).as('seconds').toFixed(3)}s`);
    } catch (error) {
        if (error instanceof Error) {
            console.log(`ERROR: ${error.message}`);
        } else {
            console.log(`UNKNOWN ERROR: ${error}`);
        }
    }
}

async_main();
