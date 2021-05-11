
<template>
	<div class="camera" id="camera">
		<img id="cameraImage"/>
	</div>
</template>

<script lang="ts">
import { Component, Prop, Vue } from 'vue-property-decorator';

import Position from '../classes/Position';

// A component that displays an image stream from UAV

@Component
export default class Camera extends Vue {
	mounted() {
		// Update image src to get new image
		setInterval(_ => {
			document.getElementById('cameraImage')?.setAttribute(
				'src', `${window.location.href}I/frame.jpg?name=example1&date=${new Date().getTime()}`
			)
		}, 50);
	}
}
</script>

<style scoped>
#camera {
	background-color: green;
	width: 90%;
	height: 400px;
}

#cameraImage {
	width: 400px;
	height: 400px;
}
</style>
