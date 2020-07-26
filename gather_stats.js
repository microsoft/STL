// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

const fs = require('fs');

const cliProgress = require('cli-progress');
const dotenv = require('dotenv');
const { DateTime, Duration } = require('luxon');
const { Octokit } = require('@octokit/rest');

{
    const result = dotenv.config();

    if (result.error) {
        throw result.error;
    }

    if (process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN === undefined) {
        throw 'Missing SECRET_GITHUB_PERSONAL_ACCESS_TOKEN key in .env file.'
    }
}

const progress_bar = new cliProgress.SingleBar(
    { format: '{bar} {percentage}% | ETA: {eta}s | {value}/{total} {name}' },
    cliProgress.Presets.shades_classic);
let progress_value = 0;

progress_bar.start(1000 /* placeholder total */, 0, { name: 'PRs and issues received' });

const octokit = new Octokit({
    auth: process.env.SECRET_GITHUB_PERSONAL_ACCESS_TOKEN,
});

octokit.paginate(
    octokit.issues.listForRepo,
    {
        owner: 'microsoft',
        repo: 'STL',
        state: 'all',
    },
    response => {
        progress_value += response.data.length;

        // PRs/issues are received in descending order by default.
        // Deleted PRs/issues are skipped, so we recalculate the expected total.
        // If the last PR/issue we received was number N, we can expect to receive N - 1 more: numbers [1, N - 1].
        const remaining = response.data[response.data.length - 1]['number'] - 1;

        progress_bar.setTotal(progress_value + remaining);
        progress_bar.update(progress_value);

        return response.data.map(item => {
            const labels = item['labels'].map(label => label['name']);
            return {
                opened: DateTime.fromISO(item['created_at']),
                closed: DateTime.fromISO(item['closed_at'] ?? '2100-01-01'),
                is_pr: item['pull_request'] !== undefined,
                labeled_cxx20: labels.includes('cxx20'),
                labeled_lwg: labels.includes('LWG') && !labels.includes('vNext') && !labels.includes('blocked'),
                labeled_bug: labels.includes('bug'),
            };
        });
    }
).then(transformed_output => {
    progress_bar.setTotal(progress_value); // Just in case PR/issue number 1 was deleted,
    progress_bar.update(progress_value); // which would prevent the progress bar from reaching 100%.
    progress_bar.stop();

    const begin = DateTime.fromISO('2019-09-05' + 'T23:00:00-07');
    const now = DateTime.local();

    let str = '// Copyright (c) Microsoft Corporation.\n';
    str += '// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception\n';
    str += '\n';
    const generated_file_warning_comment = '// Generated file - DO NOT EDIT manually!\n';
    str += generated_file_warning_comment;
    str += 'const daily_table = [\n';

    progress_bar.start(Math.ceil(now.diff(begin, 'days').as('days')), 0, { name: 'days analyzed' });

    for (let when = begin; when < now; when = when.plus({ days: 1 })) {
        let num_pr = 0;
        let num_cxx20 = 0;
        let num_lwg = 0;
        let num_issue = 0;
        let num_bug = 0;
        let combined_pr_age = Duration.fromObject({ seconds: 0 });

        for (const elem of transformed_output) {
            if (when < elem.opened || elem.closed < when) {
                // This PR/issue wasn't active; do nothing.
            } else if (elem.is_pr) {
                ++num_pr;
                combined_pr_age = combined_pr_age.plus(when.diff(elem.opened, 'seconds'));
            } else if (elem.labeled_cxx20) {
                // Avoid double-counting C++20 Features and GitHub Issues.
                ++num_cxx20;
            } else if (elem.labeled_lwg) {
                // Avoid double-counting LWG Resolutions and GitHub Issues.
                ++num_lwg;
            } else {
                ++num_issue;

                if (elem.labeled_bug) {
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
            '},\n'].join(', ');

        progress_bar.increment();
    }

    str += '];\n';
    str += generated_file_warning_comment;

    progress_bar.stop();

    fs.writeFileSync('./daily_table.js', str);
});
