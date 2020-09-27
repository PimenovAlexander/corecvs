
const chartColors = {
    red: 'rgb(255, 99, 132)',
    orange: 'rgb(255, 159, 64)',
    yellow: 'rgb(255, 205, 86)',
    green: 'rgb(75, 192, 192)',
    blue: 'rgb(54, 162, 235)',
    purple: 'rgb(153, 102, 255)',
    grey: 'rgb(201, 203, 207)'
};

const COLORS = [
    '#4dc9f6',
    '#f67019',
    '#f53794',
    '#537bc4',
    '#acc236',
    '#166a8f',
    '#00a950',
    '#58595b',
    '#8549ba'
];

const MONTHS = [
    'January',
    'February',
    'March',
    'April',
    'May',
    'June',
    'July',
    'August',
    'September',
    'October',
    'November',
    'December'
];

Samples.utils = {
    // Adapted from http://indiegamr.com/generate-repeatable-random-numbers-in-js/
    srand: function(seed) {
        this._seed = seed;
    },

    rand: function(min = 0, max = 1) {
        this._seed = (this._seed * 9301 + 49297) % 233280;
        return min + (this._seed / 233280) * (max - min);
    },

    numbers: function(config) {
        const cfg = config || {};
        const min = cfg.min || 0;
        const max = cfg.max || 1;
        const from = cfg.from || [];
        const count = cfg.count || 8;
        const decimals = cfg.decimals || 8;
        const continuity = cfg.continuity || 1;
        const dfactor = Math.pow(10, decimals) || 0;
        const data = [];

        let i, value;

        for (i = 0; i < count; ++i) {
            value = (from[i] || 0) + this.rand(min, max);
            if (this.rand() <= continuity) {
                data.push(Math.round(dfactor * value) / dfactor);
            } else {
                data.push(null);
            }
        }

        return data;
    },

    labels: function(config) {
        const cfg = config || {};
        const min = cfg.min || 0;
        const max = cfg.max || 100;
        const count = cfg.count || 8;
        const step = (max - min) / count;
        const decimals = cfg.decimals || 8;
        const dfactor = Math.pow(10, decimals) || 0;
        const prefix = cfg.prefix || '';
        const values = [];
        let i;

        for (i = min; i < max; i += step) {
            values.push(prefix + Math.round(dfactor * i) / dfactor);
        }

        return values;
    },

    months: function(config) {
        var cfg = config || {};
        var count = cfg.count || 12;
        var section = cfg.section;
        var values = [];
        var i, value;

        for (i = 0; i < count; ++i) {
            value = MONTHS[Math.ceil(i) % 12];
            values.push(value.substring(0, section));
        }

        return values;
    },

    color: function(index) {
        return COLORS[index % COLORS.length];
    },

    transparentize: function(color, opacity) {
        var alpha = opacity === undefined ? 0.5 : 1 - opacity;
        return Color(color).alpha(alpha).rgbString();
    }
};
