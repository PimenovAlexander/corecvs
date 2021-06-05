<template>
	<div id="app">
		<div id="main">
			<ChartComponent v-show="this.showingChart"/>
			<Menu v-show="!this.showingChart" :windows="this.windows" :changeActiveWindow="this.changeActiveWindow"/>
			<Map v-show="this.currentWindow('map') && !this.showingChart" v-bind:position="position" :addMap="addMap"/>
			<Camera :receivedMessagesRecently="this.receivedMessagesRecently" v-show="this.currentWindow('camera') && !this.showingChart"/>
		</div>

		<MissionController
			:SYSTEM_ID='this.SYSTEM_ID' :COMPONENT_ID='this.COMPONENT_ID'
			:getManual="this.getManualMode" :setManual="this.setManualMode"/>
		<MissionPlanner
			v-show="!this.getManualMode()"
			:SYSTEM_ID='this.SYSTEM_ID' :COMPONENT_ID='this.COMPONENT_ID'
			:startingWaypoints="waypoints" :position="position" :addMarker="addMarker" :removeMarker="removeMarker"/>
		<Properties
			:SYSTEM_ID='this.SYSTEM_ID' :COMPONENT_ID='this.COMPONENT_ID'
			:showChart='this.showChart' :hideChart='this.hideChart'
			:receivedMessagesRecently="this.receivedMessagesRecently"
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
import ChartComponent from './components/Chart.vue';

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
		Properties,
		ChartComponent
	},
})

export default class App extends Vue {
	// Placeholder for starting position
	static position = new Position(59.87994329833602, 29.82886671034883, 100)

	// Number of messages sent after previous UAV's response
	unansweredMessages = 0;

	get position() {
		return App.position
	}

	/*
		"The sender always fills in the System ID and Component ID fields so that the receiver knows where the packet came from. The System ID is a unique ID for each vehicle or ground station. Ground stations normally use a high system id like “255” and vehicles default to use “1” (this can be changed by setting the SYSID_THISMAV parameter). The Component ID for the ground station or flight controller is normally “1”." - Ardupilot Documentation
	*/

	// Placeholder for GCS's system id
	private SYSTEM_ID(): number {
		return 255;
	}

	// Placeholder for component id
	private COMPONENT_ID(): number {
		return 1;
	}

	// List of menu items available for displaying
	windows = [
		"map",
		"camera"
	]

	showingChart = false;

	manualControlMode = false;

	private getManualMode() {
		return this.manualControlMode;
	}

	private setManualMode(value: boolean) {
		this.manualControlMode = value;
	}

	private currentWindow(window: string): boolean {
		return this.activeWindow == this.windows.indexOf(window)
	}

	waypoints: Waypoint[] = [
	]

	maps: L.Map[] = [
	]

	private addMap(map: L.Map) {
		this.maps.push(map)
	}

	private addMarker(marker: L.Marker) {
		marker.addTo(this.maps[0])
	}

	private removeMarker(marker: L.Marker) {
		marker.removeFrom(this.maps[0])
	}

	private resetMessagesCounter() {
		this.unansweredMessages = 0;
	}

	private increaseMessagesCounter() {
		this.unansweredMessages++;
	}

	private receivedMessagesRecently() {
		return this.unansweredMessages < 10;
	}

	activeWindow = 0;

	private changeActiveWindow(windowIndex: number) {
		this.activeWindow = windowIndex;
	}

	private showChart() { this.showingChart = true; }
	private hideChart() { this.showingChart = false; }

	mounted() {
		// Sending messages to notify UAV of GCS's state
		setInterval(_ => {
			const heartbeatMessage = new Heartbeat(this.SYSTEM_ID(), this.COMPONENT_ID())
			heartbeatMessage.type = MavType.MAV_TYPE_GCS;
			heartbeatMessage.autopilot = MavAutopilot.MAV_AUTOPILOT_INVALID;
			heartbeatMessage.system_status = MavState.MAV_STATE_ACTIVE;

			this.increaseMessagesCounter();

			send('mavlink/heartbeatMessage', heartbeatMessage, this.resetMessagesCounter);
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
