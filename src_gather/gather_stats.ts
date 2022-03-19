// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import fs from 'fs';

import cliProgress from 'cli-progress';
import dotenv from 'dotenv';
import { DateTime, Duration, Settings } from 'luxon';
import { graphql } from '@octokit/graphql';
import yargs from 'yargs';

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

type RawAuthor = {
    login: string;
};

type RawReviewNode = {
    author: RawAuthor | null;
    submittedAt: string;
};

type RawReviews = {
    totalCount: number;
    nodes: RawReviewNode[];
};

type RawPRNode = {
    id: number;
    createdAt: string;
    closedAt: string | null;
    mergedAt: string | null;
    labels: RawLabels;
    reviews: RawReviews;
};

type RawIssueNode = {
    id: number;
    createdAt: string;
    closedAt: string | null;
    labels: RawLabels;
};

type RateLimit = {
    spent: number;
    remaining: number;
    limit: number;
};

type RawNodes = {
    pr_nodes: RawPRNode[];
    issue_nodes: RawIssueNode[];
    rate_limit: RateLimit;
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

        let spent: number;
        let limit: number;

        {
            const query_total_counts = fs.readFileSync('./query_total_counts.graphql', 'utf8');

            const {
                rateLimit,
                repository: {
                    pullRequests: { totalCount: total_prs },
                    issues: { totalCount: total_issues },
                },
            } = await authorized_graphql(query_total_counts);

            spent = rateLimit.cost;
            limit = rateLimit.limit;

            progress.pr_bar = progress.multi_bar.create(total_prs, 0, { name: 'PRs received' });
            progress.issue_bar = progress.multi_bar.create(total_issues, 0, { name: 'issues received' });
        }

        let pr_nodes: RawPRNode[] = [];
        let issue_nodes: RawIssueNode[] = [];
        let remaining: number;

        const variables = {
            query: fs.readFileSync('./query_prs_and_issues.graphql', 'utf8'),
            want_prs: true,
            pr_cursor: null as null | string,
            want_issues: true,
            issue_cursor: null as null | string,
        };

        do {
            const {
                rateLimit,
                repository: { pullRequests, issues },
            } = await authorized_graphql(variables);

            spent += rateLimit.cost;
            remaining = rateLimit.remaining;

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
        } while (variables.want_prs || variables.want_issues);

        const ret: RawNodes = {
            pr_nodes: pr_nodes,
            issue_nodes: issue_nodes,
            rate_limit: { spent: spent, remaining: remaining, limit: limit },
        };
        return ret;
    } finally {
        progress.multi_bar.stop();
    }
}

async function retrieve_nodes() {
    if (argv['load-file']) {
        return JSON.parse(fs.readFileSync(argv['load-file'], 'utf8')) as RawNodes;
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
    const uniques = new Set<string>();
    const duplicates = new Set<string>();

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
    closed: DateTime | null;
    merged: DateTime | null;
    reviews: DateTime[];
};

function transform_pr_nodes(pr_nodes: RawPRNode[]) {
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

            const ret: CookedPRNode = {
                id: pr_node.id,
                opened: DateTime.fromISO(pr_node.createdAt),
                closed: pr_node.closedAt ? DateTime.fromISO(pr_node.closedAt) : null,
                merged: pr_node.mergedAt ? DateTime.fromISO(pr_node.mergedAt) : null,
                reviews: maintainer_reviews,
            };
            return ret;
        });
}

type CookedIssueNode = {
    id: number;
    opened: DateTime;
    closed: DateTime | null;
    labeled_cxx20: boolean;
    labeled_cxx23: boolean;
    labeled_lwg: boolean;
    labeled_bug: boolean;
};

function transform_issue_nodes(issue_nodes: RawIssueNode[]) {
    warn_if_pagination_needed(
        issue_nodes,
        (node: RawIssueNode) => node.labels,
        (retrieved: number, total: number, id: number) =>
            `WARNING: Retrieved ${retrieved}/${total} labels for issue #${id}.`
    );
    return issue_nodes.map(node => {
        const labels = node.labels.nodes.map(label => label.name);
        const ret: CookedIssueNode = {
            id: node.id,
            opened: DateTime.fromISO(node.createdAt),
            closed: node.closedAt ? DateTime.fromISO(node.closedAt) : null,
            labeled_cxx20: labels.includes('cxx20'),
            labeled_cxx23: labels.includes('cxx23'),
            labeled_lwg: labels.includes('LWG') && !labels.includes('vNext') && !labels.includes('blocked'),
            labeled_bug: labels.includes('bug'),
        };
        return ret;
    });
}

const sliding_window = Duration.fromObject({ days: 40 });

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
    const str = `
// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Generated file - DO NOT EDIT manually!
${table_str.trim()}
// Generated file - DO NOT EDIT manually!
`;

    fs.writeFileSync(filename, str.trimStart());
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
    // The original algorithm for calculating stats was simple but slow. For each day in the chart,
    // we iterated over all PRs/issues and counted how many were open/etc. on that day.
    // This involved a lot of repeated work, because it calculated each day independently.

    // There's a faster way to calculate most lines. Consider a simple line, like the number of open bugs.
    // Each bug affects the line only twice, producing a +1 delta on the day that it was opened, and
    // a -1 delta on the day that it was closed. For each bug, we can record those "events", then sort them by date.
    // Then, as we iterate over each day in the chart, we can consume the events that have "happened so far",
    // which will update the number of open bugs without having to recalculate it from scratch.

    // There are limited exceptions to this approach. The PR Age and Monthly Merged PRs stats are more complicated.
    // We partially accelerate them by using events to update maps, allowing us
    // to iterate over all PRs that were open (or recently merged) on that day.
    // In the future, we may need to optimize this further, but this is fast enough for now.

    type Event = {
        date: DateTime;
        action: () => void;
    };

    const events: Event[] = [];

    const opened_prs = new Map<number, DateTime>(); // Tracks when currently open PRs were opened
    const reviewed_prs = new Map<number, DateTime>(); // Tracks when currently open PRs were last reviewed (or opened)
    const merged_prs = new Map<number, DateTime>(); // Tracks when recently merged PRs were merged

    for (const pr of all_prs) {
        events.push({
            date: pr.opened,
            action: () => {
                opened_prs.set(pr.id, pr.opened);
                reviewed_prs.set(pr.id, pr.opened);
            },
        });

        for (const review of pr.reviews) {
            events.push({ date: review, action: () => reviewed_prs.set(pr.id, review) });
        }

        if (pr.closed !== null) {
            events.push({
                date: pr.closed,
                action: () => {
                    opened_prs.delete(pr.id);
                    reviewed_prs.delete(pr.id);
                },
            });
        }

        if (pr.merged !== null) {
            const merge_date = pr.merged;
            const window_end = merge_date.plus(sliding_window);
            events.push({ date: merge_date, action: () => merged_prs.set(pr.id, merge_date) });
            events.push({ date: window_end, action: () => merged_prs.delete(pr.id) });
        }
    }

    let num_cxx20 = 0;
    let num_cxx23 = 0;
    let num_lwg = 0;
    let num_issue = 0;
    let num_bug = 0;

    for (const issue of all_issues) {
        for (const { t, change } of [
            { t: issue.opened, change: 1 },
            { t: issue.closed, change: -1 },
        ]) {
            if (t !== null) {
                events.push({
                    date: t,
                    action: () => {
                        // Avoid double-counting C++20 Features etc. and GitHub Issues.
                        if (issue.labeled_cxx20) {
                            num_cxx20 += change;
                        } else if (issue.labeled_cxx23) {
                            num_cxx23 += change;
                        } else if (issue.labeled_lwg) {
                            num_lwg += change;
                        } else {
                            num_issue += change;

                            if (issue.labeled_bug) {
                                num_bug += change;
                            }
                        }
                    },
                });
            }
        }
    }

    events.sort((a, b) => a.date.toMillis() - b.date.toMillis());

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
            while (events.length > 0 && events[0].date < when) {
                events[0].action();
                events.shift();
            }

            const num_merged = Array.from(merged_prs.values())
                .map(t => calculate_sliding_window(when, t))
                .reduce((x, y) => x + y, 0);

            const num_pr = opened_prs.size;

            const zero_duration = Duration.fromObject({});
            const combined_pr_age = Array.from(opened_prs.values())
                .map(t => when.diff(t))
                .reduce((x, y) => x.plus(y), zero_duration);
            const combined_pr_wait = Array.from(reviewed_prs.values())
                .map(t => when.diff(t))
                .reduce((x, y) => x.plus(y), zero_duration);

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

    let str = `
export type DailyRow = {
    date: string;
    merged: number;
    pr: number | null;
    cxx20: number | null;
    cxx23: number | null;
    lwg: number | null;
    issue: number | null;
    bug: number | null;
    avg_age: number;
    avg_wait: number;
    sum_age: number;
    sum_wait: number;
};
export const daily_table: DailyRow[] = [
`;

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

    write_generated_file('./src/daily_table.ts', str);
}

function write_monthly_table(script_start: DateTime, all_prs: CookedPRNode[]) {
    const monthly_merges = new Map<string, number>();

    for (const pr of all_prs) {
        if (pr.merged !== null) {
            const year_month = pr.merged.toFormat('yyyy-MM');
            const old_value = monthly_merges.get(year_month) ?? 0;
            monthly_merges.set(year_month, old_value + 1);
        }
    }

    let str = `
export type MonthlyRow = {
    date: string;
    merge_bar: number;
};
export const monthly_table: MonthlyRow[] = [
`;

    // Analyze complete months.
    const begin = DateTime.fromISO('2019-10-01');
    for (let when = begin; when < script_start.startOf('month'); when = when.plus({ months: 1 })) {
        const year_month = when.toFormat('yyyy-MM');
        const value = monthly_merges.get(year_month) ?? 0;

        str += '    { ';
        str += `date: '${year_month}-16', `; // position each bar in the middle of each month
        str += `merge_bar: ${value}, `;
        str += '},\n';
    }

    str += '];\n';

    write_generated_file('./src/monthly_table.ts', str);
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
