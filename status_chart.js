// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

'use strict';

function get_values(table, key) {
    return table.filter(row => row[key] !== undefined).map(row => ({ x: row.date, y: row[key] }));
}

const url_search_params = new URLSearchParams(window.location.search);
const hide_string = 'n';
const show_string = 'y';

function get_hidden(key, default_hidden = false) {
    let hidden = null;

    const value = url_search_params.get(key);

    if (value === hide_string) {
        hidden = true;
    } else if (value === show_string) {
        hidden = false;
    } else {
        hidden = default_hidden;
    }

    return {
        hidden: hidden,
        stl_default_hidden: default_hidden,
        stl_key: key,
    };
}

function update_url() {
    let url = window.location.pathname;

    const params_string = `${url_search_params}`;

    if (params_string.length > 0) {
        url += `?${params_string}`;
    }

    window.history.replaceState({}, '', url);
}

const status_data = {
    datasets: [
        {
            data: get_values(weekly_table, 'cxx17'),
            label: 'C++17 Features',
            borderColor: '#9966FF',
            backgroundColor: '#9966FF',
            borderDash: [10, 5],
            yAxisID: 'smallAxis',
            ...get_hidden('cxx17'),
        },
        {
            data: get_values(weekly_table, 'cxx20').concat(get_values(daily_table, 'cxx20')),
            label: 'C++20 Features',
            borderColor: '#7030A0',
            backgroundColor: '#7030A0',
            yAxisID: 'smallAxis',
            ...get_hidden('cxx20'),
        },
        {
            data: get_values(daily_table, 'cxx23'),
            label: 'C++23 Features',
            borderColor: '#9966FF',
            backgroundColor: '#9966FF',
            yAxisID: 'smallAxis',
            ...get_hidden('cxx23'),
        },
        {
            data: get_values(weekly_table, 'lwg').concat(get_values(daily_table, 'lwg')),
            label: 'LWG Resolutions',
            borderColor: '#0070C0',
            backgroundColor: '#0070C0',
            yAxisID: 'smallAxis',
            ...get_hidden('lwg'),
        },
        {
            data: get_values(daily_table, 'pr'),
            label: 'Pull Requests',
            borderColor: '#00B050',
            backgroundColor: '#00B050',
            yAxisID: 'smallAxis',
            ...get_hidden('pr'),
        },
        {
            data: get_values(weekly_table, 'vso'),
            label: 'Old Bugs',
            borderColor: '#900000',
            backgroundColor: '#900000',
            yAxisID: 'largeAxis',
            ...get_hidden('vso'),
        },
        {
            data: get_values(daily_table, 'bug'),
            label: 'GitHub Bugs',
            borderColor: '#FF0000',
            backgroundColor: '#FF0000',
            yAxisID: 'largeAxis',
            ...get_hidden('bug'),
        },
        {
            data: get_values(daily_table, 'issue'),
            label: 'GitHub Issues',
            borderColor: '#909090',
            backgroundColor: '#909090',
            yAxisID: 'largeAxis',
            ...get_hidden('issue'),
        },
        {
            data: get_values(weekly_table, 'libcxx'),
            label: 'Skipped Libcxx Tests',
            borderColor: '#FFC000',
            backgroundColor: '#FFC000',
            yAxisID: 'largeAxis',
            ...get_hidden('libcxx'),
        },
    ],
};

const age_data = {
    datasets: [
        {
            data: get_values(daily_table, 'avg_age'),
            label: 'Average Age',
            borderColor: '#909090',
            backgroundColor: '#909090',
            yAxisID: 'leftAxis',
            ...get_hidden('avg_age', true),
        },
        {
            data: get_values(daily_table, 'avg_wait'),
            label: 'Average Wait',
            borderColor: '#FF9090',
            backgroundColor: '#FF9090',
            yAxisID: 'leftAxis',
            ...get_hidden('avg_wait', true),
        },
        {
            data: get_values(daily_table, 'sum_age'),
            label: 'Combined Age',
            borderColor: '#000000',
            backgroundColor: '#000000',
            yAxisID: 'rightAxis',
            ...get_hidden('sum_age'),
        },
        {
            data: get_values(daily_table, 'sum_wait'),
            label: 'Combined Wait',
            borderColor: '#FF0000',
            backgroundColor: '#FF0000',
            yAxisID: 'rightAxis',
            ...get_hidden('sum_wait'),
        },
    ],
};

const merge_data = {
    datasets: [
        {
            data: get_values(daily_table, 'merged'),
            label: 'Line: Sliding Window',
            borderColor: '#00B050',
            backgroundColor: '#00B050',
            yAxisID: 'mergeAxis',
            ...get_hidden('merged'),
        },
        {
            type: 'bar',
            data: get_values(monthly_table, 'merge_bar'),
            label: 'Bars: Calendar Months',
            borderColor: '#CCCCCC',
            borderWidth: 1,
            yAxisID: 'mergeAxis',
            ...get_hidden('merge_bar'),
        },
    ],
};

const timeframe_all = {
    min: '2017-06-09',
    time: { unit: 'quarter' },
};
const timeframe_github = {
    min: '2019-09-20', // first Friday after 2019-09-16
    time: { unit: 'quarter' },
};
const timeframe_2021 = {
    min: '2021-01-01',
    time: { unit: 'month' },
};
const timeframes = [timeframe_all, timeframe_github, timeframe_2021];
const timeframe_github_idx = 1;
let timeframe_idx = timeframe_github_idx;

const common_options = {
    animation: {
        duration: 0,
    },
    elements: {
        line: {
            borderCapStyle: 'round',
            borderJoinStyle: 'round',
            fill: false,
            spanGaps: false,
        },
        point: {
            radius: 0,
        },
    },
    hover: {
        mode: 'nearest',
    },
};

function legend_click_handler(_event, legend_item, legend) {
    const ch = legend.chart;
    const index = legend_item.datasetIndex;

    const { stl_default_hidden, stl_key } = ch.data.datasets[index];

    const becoming_hidden = ch.isDatasetVisible(index);

    if (becoming_hidden) {
        ch.hide(index);
    } else {
        ch.show(index);
    }

    legend_item.hidden = becoming_hidden;

    if (becoming_hidden === stl_default_hidden) {
        url_search_params.delete(stl_key);
    } else {
        url_search_params.set(stl_key, becoming_hidden ? hide_string : show_string);
        url_search_params.sort();
    }

    update_url();
}

const common_plugins = {
    legend: {
        onClick: legend_click_handler,
    },
    tooltip: {
        mode: 'nearest',
        intersect: false,
    },
};

const common_title = {
    display: true,
    font: {
        size: 24,
    },
};

function make_xAxis(timeframe) {
    return {
        type: 'time',
        min: timeframe.min,
        max: daily_table[daily_table.length - 1].date,
        grid: {
            offset: false,
        },
        offset: false,
        time: {
            parser: 'yyyy-MM-dd',
            tooltipFormat: 'MMM d, yyyy',
            unit: timeframe.time.unit,
            displayFormats: {
                quarter: 'MMM yyyy',
            },
        },
    };
}

const status_options = {
    ...common_options,
    plugins: {
        ...common_plugins,
        title: {
            ...common_title,
            text: 'STL Status Chart',
        },
    },
    scales: {
        x: make_xAxis(timeframes[timeframe_idx]),
        largeAxis: {
            type: 'linear',
            display: 'auto',
            position: 'left',
            title: {
                display: true,
                text: 'Bugs, Issues, Skipped Libcxx Tests',
            },
            min: 0,
            max: 900,
            ticks: {
                stepSize: 100,
            },
        },
        smallAxis: {
            type: 'linear',
            display: 'auto',
            position: 'right',
            title: {
                display: true,
                text: 'Features, LWG Resolutions, Pull Requests',
            },
            min: 0,
            max: 90,
            ticks: {
                stepSize: 10,
            },
        },
    },
};

const age_options = {
    ...common_options,
    plugins: {
        ...common_plugins,
        title: {
            ...common_title,
            text: 'Pull Request Age',
        },
    },
    scales: {
        x: make_xAxis(timeframe_github),
        leftAxis: {
            type: 'linear',
            display: 'auto',
            position: 'left',
            title: {
                display: true,
                text: 'Average Age, Average Wait (days)',
            },
            min: 0,
            max: 450,
            ticks: {
                stepSize: 50,
            },
        },
        rightAxis: {
            type: 'linear',
            display: 'auto',
            position: 'right',
            title: {
                display: true,
                text: 'Combined Age, Combined Wait (PR-months)',
            },
            min: 0,
            max: 450,
            ticks: {
                stepSize: 50,
            },
        },
    },
};

const merge_options = {
    ...common_options,
    plugins: {
        ...common_plugins,
        title: {
            ...common_title,
            text: 'Monthly Merged PRs',
        },
    },
    scales: {
        x: make_xAxis(timeframe_github),
        mergeAxis: {
            type: 'linear',
            display: 'auto',
            position: 'right',
            title: {
                display: true,
                text: 'PRs / month',
            },
            min: 0,
            max: 80,
            ticks: {
                stepSize: 10,
            },
        },
    },
};

window.onload = function () {
    window.status_chart = new Chart('statusChart', {
        type: 'line',
        data: status_data,
        options: status_options,
    });

    window.age_chart = new Chart('ageChart', {
        type: 'line',
        data: age_data,
        options: age_options,
    });

    window.merge_chart = new Chart('mergeChart', {
        type: 'line',
        data: merge_data,
        options: merge_options,
    });

    function update_chart_timeframe(chart, idx) {
        const xAxis = chart.options.scales.x;
        const new_timeframe = timeframes[idx];

        xAxis.min = new_timeframe.min;
        xAxis.time.unit = new_timeframe.time.unit;

        chart.update();
    }

    function update_all_timeframes() {
        document.getElementById('moreHistory').disabled = timeframe_idx === 0;
        document.getElementById('lessHistory').disabled = timeframe_idx === timeframes.length - 1;

        const clamped_idx = Math.max(timeframe_idx, timeframe_github_idx);

        update_chart_timeframe(window.status_chart, timeframe_idx);
        update_chart_timeframe(window.age_chart, clamped_idx);
        update_chart_timeframe(window.merge_chart, clamped_idx);
    }

    document.getElementById('moreHistory').addEventListener('click', function () {
        if (timeframe_idx > 0) {
            --timeframe_idx;
        }

        update_all_timeframes();
    });

    document.getElementById('lessHistory').addEventListener('click', function () {
        if (timeframe_idx < timeframes.length - 1) {
            ++timeframe_idx;
        }

        update_all_timeframes();
    });

    for (const field of [
        'cxx20',
        'cxx23',
        'lwg',
        'pr',
        'bug',
        'issue',
        'avg_age',
        'avg_wait',
        'sum_age',
        'sum_wait',
        'merged',
    ]) {
        const value = daily_table[daily_table.length - 1][field] ?? 0;
        document.getElementById(`currentValue-${field}`).textContent = value.toString();
    }

    for (const field of ['vso', 'libcxx']) {
        const value = weekly_table[weekly_table.length - 1][field];
        document.getElementById(`currentValue-${field}`).textContent = value.toString();
    }
};
