
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
	@Prop() private receivedMessagesRecently!: () => boolean;

	mounted() {
		// Update image src to get new image
		setInterval(_ => {
			if (!this.receivedMessagesRecently()) return;
			
			document.getElementById('cameraImage')?.setAttribute(
				'src', `${window.location.href}I/frame.jpg?name=example1&date=${new Date().getTime()}`
			)
		}, 200);
	}
}
</script>

<style scoped>
#camera {
	background-color: green;
	width: 80%;
	height: 400px;
}

#cameraImage {
	width: 400px;
	height: 400px;
}
</style>
