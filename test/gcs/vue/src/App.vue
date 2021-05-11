<template>
	<div id="app">
		<div id="main">
			<Menu :windows="this.windows" :changeActiveWindow="this.changeActiveWindow"/>
			<Map v-show="this.currentWindow('map')" v-bind:position="position" :addMap="addMap"/>
			<Camera v-show="this.currentWindow('camera')"/>
		</div>

		<MissionController
			:SYSTEM_ID='this.SYSTEM_ID' :COMPONENT_ID='this.COMPONENT_ID'
			:getManual="this.getManualMode" :setManual="this.setManualMode"/>
		<MissionPlanner
			v-show="!this.getManualMode()"
			:SYSTEM_ID='this.SYSTEM_ID' :COMPONENT_ID='this.COMPONENT_ID'
			:startingWaypoints="waypoints" :position="position" :addMarker="addMarker" :removeMarker="removeMarker"/>
		<Properties
			v-show="this.getManualMode()"/>
	</div>
</template>

<script lang="ts">
import { Component, Vue } from 'vue-property-decorator';
import Map from './components/Map.vue';
import Position from './classes/Position';
import Waypoint from './classes/Waypoint';
import WaypointComponent from './components/Waypoint.vue';
import Menu from './components/Menu.vue';
import MissionPlanner from './components/MissionPlanner.vue';
import Camera from './components/Camera.vue';
import MissionController from './components/MissionController.vue';
import Properties from './components/Properties.vue';

import { Heartbeat } from './classes/mavlink/messages/heartbeat';

import L from 'leaflet';
import { MavType } from './classes/mavlink/enums/mav-type';
import { MavAutopilot } from './classes/mavlink/enums/mav-autopilot';
import { MavState } from './classes/mavlink/enums/mav-state';

import { send } from './classes/AjaxRequest';

@Component({
	components: {
		Map,
		WaypointComponent,
		Menu,
		MissionPlanner,
		Camera,
		MissionController,
		Properties
	},
})

export default class App extends Vue {
	static position = new Position(59.87994329833602, 29.82886671034883, 100)

	get position() {
		return App.position
	}

	// Placeholder for GCS's system id
	SYSTEM_ID(): number {
		return 1;
	}

	// Placeholder for component id
	COMPONENT_ID(): number {
		return 2;
	}

	// List of menu items available for displaying
	windows = [
		"map",
		"camera"
	]

	manualControlMode = false;

	getManualMode() {
		return this.manualControlMode;
	}

	setManualMode(value: boolean) {
		this.manualControlMode = value;
	}

	currentWindow(window: string): boolean {
		return this.activeWindow == this.windows.indexOf(window)
	}

	waypoints: Waypoint[] = [
	]

	maps: L.Map[] = [
	]

	addMap(map: L.Map) {
		this.maps.push(map)
	}

	addMarker(marker: L.Marker) {
		marker.addTo(this.maps[0])
	}

	removeMarker(marker: L.Marker) {
		marker.removeFrom(this.maps[0])
	}

	activeWindow = 0;

	changeActiveWindow(windowIndex: number) {
		this.activeWindow = windowIndex;
	}

	mounted() {
		// Sending messages to notify UAV of GCS's state
		setInterval(_ => {
			const heartbeatMessage = new Heartbeat(this.SYSTEM_ID(), this.COMPONENT_ID())
			heartbeatMessage.type = MavType.MAV_TYPE_GCS;
			heartbeatMessage.autopilot = MavAutopilot.MAV_AUTOPILOT_INVALID;
			heartbeatMessage.system_status = MavState.MAV_STATE_ACTIVE;
			
			send('mavlink/heartbeatMessage', heartbeatMessage);
		}, 1000);
	}
}
</script>

<style>
body {
	background-color: #789;
}

#app {
	font-family: Avenir, Helvetica, Arial, sans-serif;
	-webkit-font-smoothing: antialiased;
	-moz-osx-font-smoothing: grayscale;
	text-align: center;
	color: #2c3e50;
	margin-top: 60px;
}

#main {
	display: grid;
	grid-template-columns: 250px auto;
}

Menu {
	grid-column: 1;
}

Map {
	grid-column: 2;
}
</style>
