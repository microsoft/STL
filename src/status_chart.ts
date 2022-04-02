// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import { Chart, ChartEvent, ChartType, LegendElement, LegendItem, TimeUnit, registerables } from 'chart.js';
Chart.register(...registerables);
import 'chartjs-adapter-luxon';

import { DailyRow, daily_table } from './daily_table';
import { WeeklyRow, weekly_table } from './weekly_table';
import { MonthlyRow, monthly_table } from './monthly_table';

type StlDataPoint = {
    x: string;
    y: number | null;
};

function get_values<TimelyRow extends DailyRow | WeeklyRow | MonthlyRow>(
    table: TimelyRow[],
    get_y: (row: TimelyRow) => number | null | undefined
) {
    const datapoints: StlDataPoint[] = [];
    for (const row of table) {
        const y = get_y(row);
        if (y !== undefined) {
            datapoints.push({ x: row.date, y: y });
        }
    }
    return datapoints;
}

function get_daily_values(key: Exclude<keyof DailyRow, 'date'>) {
    return get_values(daily_table, row => row[key]);
}

function get_weekly_values(key: Exclude<keyof WeeklyRow, 'date'>) {
    return get_values(weekly_table, row => row[key]);
}

function get_monthly_values(key: Exclude<keyof MonthlyRow, 'date'>) {
    return get_values(monthly_table, row => row[key]);
}

class HiddenInfo {
    url_key: string;
    chart_label: string;
    default_hidden: boolean;

    constructor(url_key: string, chart_label: string, default_hidden: boolean = false) {
        this.url_key = url_key;
        this.chart_label = chart_label;
        this.default_hidden = default_hidden;
    }
}

class HiddenInfoMaps {
    static url_key = new Map<string, HiddenInfo>();
    static chart_label = new Map<string, HiddenInfo>();

    static {
        const arr = [
            new HiddenInfo('cxx17', 'C++17 Features'),
            new HiddenInfo('cxx20', 'C++20 Features'),
            new HiddenInfo('cxx23', 'C++23 Features'),
            new HiddenInfo('lwg', 'LWG Resolutions'),
            new HiddenInfo('pr', 'Pull Requests'),
            new HiddenInfo('vso', 'Old Bugs'),
            new HiddenInfo('bug', 'GitHub Bugs'),
            new HiddenInfo('issue', 'GitHub Issues'),
            new HiddenInfo('libcxx', 'Skipped Libcxx Tests'),

            new HiddenInfo('avg_age', 'Average Age', true),
            new HiddenInfo('avg_wait', 'Average Wait', true),
            new HiddenInfo('sum_age', 'Combined Age'),
            new HiddenInfo('sum_wait', 'Combined Wait'),

            new HiddenInfo('merged', 'Line: Sliding Window'),
            new HiddenInfo('merge_bar', 'Bars: Calendar Months'),
        ];

        for (const elem of arr) {
            this.url_key.set(elem.url_key, elem);
            this.chart_label.set(elem.chart_label, elem);
        }
    }

    static lookup(field: 'url_key' | 'chart_label', value: string) {
        const ret = this[field].get(value);

        if (ret === undefined) {
            throw new Error('HiddenInfoMaps.lookup() should always find the value.');
        }

        return ret;
    }
}

const url_search_params = new URLSearchParams(window.location.search);
const hide_string = 'n';
const show_string = 'y';

function get_label_and_hidden(url_key: string) {
    const { chart_label, default_hidden } = HiddenInfoMaps.lookup('url_key', url_key);

    let hidden: boolean;

    const value = url_search_params.get(url_key);

    if (value === hide_string) {
        hidden = true;
    } else if (value === show_string) {
        hidden = false;
    } else {
        hidden = default_hidden;
    }

    return {
        label: chart_label,
        hidden: hidden,
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
            data: get_weekly_values('cxx17'),
            borderColor: '#9966FF',
            backgroundColor: '#9966FF',
            borderDash: [10, 5],
            yAxisID: 'smallAxis',
            ...get_label_and_hidden('cxx17'),
        },
        {
            data: get_weekly_values('cxx20').concat(get_daily_values('cxx20')),
            borderColor: '#7030A0',
            backgroundColor: '#7030A0',
            yAxisID: 'smallAxis',
            ...get_label_and_hidden('cxx20'),
        },
        {
            data: get_daily_values('cxx23'),
            borderColor: '#9966FF',
            backgroundColor: '#9966FF',
            yAxisID: 'smallAxis',
            ...get_label_and_hidden('cxx23'),
        },
        {
            data: get_weekly_values('lwg').concat(get_daily_values('lwg')),
            borderColor: '#0070C0',
            backgroundColor: '#0070C0',
            yAxisID: 'smallAxis',
            ...get_label_and_hidden('lwg'),
        },
        {
            data: get_daily_values('pr'),
            borderColor: '#00B050',
            backgroundColor: '#00B050',
            yAxisID: 'smallAxis',
            ...get_label_and_hidden('pr'),
        },
        {
            data: get_weekly_values('vso'),
            borderColor: '#900000',
            backgroundColor: '#900000',
            yAxisID: 'largeAxis',
            ...get_label_and_hidden('vso'),
        },
        {
            data: get_daily_values('bug'),
            borderColor: '#FF0000',
            backgroundColor: '#FF0000',
            yAxisID: 'largeAxis',
            ...get_label_and_hidden('bug'),
        },
        {
            data: get_daily_values('issue'),
            borderColor: '#909090',
            backgroundColor: '#909090',
            yAxisID: 'largeAxis',
            ...get_label_and_hidden('issue'),
        },
        {
            data: get_weekly_values('libcxx'),
            borderColor: '#FFC000',
            backgroundColor: '#FFC000',
            yAxisID: 'largeAxis',
            ...get_label_and_hidden('libcxx'),
        },
    ],
};

const age_data = {
    datasets: [
        {
            data: get_daily_values('avg_age'),
            borderColor: '#909090',
            backgroundColor: '#909090',
            yAxisID: 'leftAxis',
            ...get_label_and_hidden('avg_age'),
        },
        {
            data: get_daily_values('avg_wait'),
            borderColor: '#FF9090',
            backgroundColor: '#FF9090',
            yAxisID: 'leftAxis',
            ...get_label_and_hidden('avg_wait'),
        },
        {
            data: get_daily_values('sum_age'),
            borderColor: '#000000',
            backgroundColor: '#000000',
            yAxisID: 'rightAxis',
            ...get_label_and_hidden('sum_age'),
        },
        {
            data: get_daily_values('sum_wait'),
            borderColor: '#FF0000',
            backgroundColor: '#FF0000',
            yAxisID: 'rightAxis',
            ...get_label_and_hidden('sum_wait'),
        },
    ],
};

const merge_data = {
    datasets: [
        {
            data: get_daily_values('merged'),
            borderColor: '#00B050',
            backgroundColor: '#00B050',
            yAxisID: 'mergeAxis',
            ...get_label_and_hidden('merged'),
        },
        {
            type: 'bar' as const,
            data: get_monthly_values('merge_bar'),
            borderColor: '#CCCCCC',
            borderWidth: 1,
            yAxisID: 'mergeAxis',
            ...get_label_and_hidden('merge_bar'),
        },
    ],
};

type Timeframe = {
    min: string;
    time: { unit: TimeUnit };
};

const timeframe_all: Timeframe = {
    min: '2017-06-09',
    time: { unit: 'year' },
};
const timeframe_github: Timeframe = {
    min: '2019-09-20', // first Friday after 2019-09-16
    time: { unit: 'quarter' },
};
const timeframe_2021: Timeframe = {
    min: '2021-01-01',
    time: { unit: 'quarter' },
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
            borderCapStyle: 'round' as const,
            borderJoinStyle: 'round' as const,
            fill: false,
            spanGaps: false,
        },
        point: {
            radius: 0,
        },
    },
    hover: {
        mode: 'nearest' as const,
    },
};

function legend_click_handler(_event: ChartEvent, legend_item: LegendItem, legend: LegendElement<ChartType>) {
    const ch = legend.chart;
    const index = legend_item.datasetIndex;

    const becoming_hidden = ch.isDatasetVisible(index);

    if (becoming_hidden) {
        ch.hide(index);
    } else {
        ch.show(index);
    }

    legend_item.hidden = becoming_hidden;

    const { url_key, default_hidden } = HiddenInfoMaps.lookup('chart_label', legend_item.text);

    if (becoming_hidden === default_hidden) {
        url_search_params.delete(url_key);
    } else {
        url_search_params.set(url_key, becoming_hidden ? hide_string : show_string);
        url_search_params.sort();
    }

    update_url();
}

const common_plugins = {
    legend: {
        onClick: legend_click_handler,
    },
    tooltip: {
        mode: 'nearest' as const,
        intersect: false,
    },
};

const common_title = {
    display: true,
    font: {
        size: 24,
    },
};

function make_xAxis(timeframe: Timeframe) {
    return {
        type: 'time' as const,
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
            type: 'linear' as const,
            display: 'auto' as const,
            position: 'left' as const,
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
            type: 'linear' as const,
            display: 'auto' as const,
            position: 'right' as const,
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
            type: 'linear' as const,
            display: 'auto' as const,
            position: 'left' as const,
            title: {
                display: true,
                text: 'Average Age, Average Wait (days)',
            },
            min: 0,
            max: 600,
            ticks: {
                stepSize: 100,
            },
        },
        rightAxis: {
            type: 'linear' as const,
            display: 'auto' as const,
            position: 'right' as const,
            title: {
                display: true,
                text: 'Combined Age, Combined Wait (PR-months)',
            },
            min: 0,
            max: 600,
            ticks: {
                stepSize: 100,
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
            type: 'linear' as const,
            display: 'auto' as const,
            position: 'right' as const,
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

function getElementByIdAs<Type extends HTMLElement>(id: string, type: new () => Type) {
    const element = document.getElementById(id);

    if (element === null) {
        throw new Error(`document.getElementById('${id}') returned null.`);
    }

    if (element instanceof type) {
        return element as Type;
    }

    throw new Error(`document.getElementById('${id}') returned an unexpected type.`);
}

function load_charts() {
    const status_chart = new Chart('statusChart', {
        type: 'line',
        data: status_data,
        options: status_options,
    });

    const age_chart = new Chart('ageChart', {
        type: 'line',
        data: age_data,
        options: age_options,
    });

    const merge_chart = new Chart('mergeChart', {
        type: 'line',
        data: merge_data,
        options: merge_options,
    });

    function update_chart_timeframe(chart: typeof status_chart, idx: number) {
        if (!('scales' in chart.options)) {
            throw new Error('update_chart_timeframe() expected chart.options.scales to exist.');
        }

        chart.options.scales.x = make_xAxis(timeframes[idx]);

        chart.update();
    }

    const moreHistoryButton = getElementByIdAs('moreHistory', HTMLButtonElement);
    const lessHistoryButton = getElementByIdAs('lessHistory', HTMLButtonElement);

    function update_all_timeframes() {
        moreHistoryButton.disabled = timeframe_idx === 0;
        lessHistoryButton.disabled = timeframe_idx === timeframes.length - 1;

        const clamped_idx = Math.max(timeframe_idx, timeframe_github_idx);

        update_chart_timeframe(status_chart, timeframe_idx);
        update_chart_timeframe(age_chart, clamped_idx);
        update_chart_timeframe(merge_chart, clamped_idx);
    }

    moreHistoryButton.addEventListener('click', function () {
        if (timeframe_idx > 0) {
            --timeframe_idx;
        }

        update_all_timeframes();
    });

    lessHistoryButton.addEventListener('click', function () {
        if (timeframe_idx < timeframes.length - 1) {
            ++timeframe_idx;
        }

        update_all_timeframes();
    });

    const daily_keys = [
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
    ] as const;
    for (const field of daily_keys) {
        const value = daily_table[daily_table.length - 1][field] ?? 0;
        const span = getElementByIdAs(`currentValue-${field}`, HTMLSpanElement);
        span.textContent = value.toString();
    }

    const weekly_keys = ['vso', 'libcxx'] as const;
    for (const field of weekly_keys) {
        const value = weekly_table[weekly_table.length - 1][field];
        const span = getElementByIdAs(`currentValue-${field}`, HTMLSpanElement);
        span.textContent = value.toString();
    }
}

load_charts();
