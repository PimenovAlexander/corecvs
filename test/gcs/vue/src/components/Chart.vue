
<template>
	<div id = "graphDiv"><canvas id="graphCanvas"></canvas></div>
</template>

<script lang="ts">
import { Component, Vue, Prop } from 'vue-property-decorator';

import { send } from '../classes/AjaxRequest';

@Component({
	components: {
	},
})

export default class ChartComponent extends Vue {
}
</script>

<style scoped>
#graphDiv {
	width: 1700px;
	height: 410px;
	top: 0px;
	left: 0px;
}

#graphCanvas {
	width: 100%;
}
</style>

