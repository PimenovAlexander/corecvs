
function onLoad() {
    /*
    const graphDiv = document.getElementById("graph")
    const R = Raphael(graphDiv, graphDiv.width, graphDiv.height);
    R.circle(20, 30, 50);
     */

    const graphCanvas = document.getElementById("graphCanvas");

    graphCanvas.style.maxWidth = '100%';
    graphCanvas.style.height = '100%';
    graphCanvas.width = 800;
    graphCanvas.height = 600;

    const ctx = graphCanvas.getContext('2d');

    Chart.defaults.global.defaultFontSize = 32;
    Chart.defaults.global.defaultFontColor = 'rgba(250, 250, 250, 1)';

    const defaultColors = [
        'rgba(222, 0, 0, 1)',
        'rgba(0, 222, 0, 1)',
        'rgba(0, 222, 222, 1)'
    ]

    const graph = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: []
        },
        options: {
            maintainAspectRatio: false,

            responsive: true,
            animation: {
                duration: 0
            },
            hover: {
                animationDuration: 0
            },
            responsiveAnimationDuration: 0
        }
    });

    recursive_Ajax("/G/graph.json", val => {
        const jsonObject = JSON.parse(val);
        graph.data.datasets = jsonObject.map((val, ind) => {
            val.borderColor = defaultColors[ind];
            return val;
        })
        graph.data.labels = jsonObject[0].data.map((val, ind) => ind);
        graph.update();
    })
}

(function(fn){
    if (document.readyState !== 'loading') {
        fn();
    } else {
        document.addEventListener('DOMContentLoaded', fn);
    }
})(onLoad)
