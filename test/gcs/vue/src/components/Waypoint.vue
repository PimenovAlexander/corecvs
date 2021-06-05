
<template>
	<div class="waypoint">
		<input type="text" v-model="waypoint.position.latitude" placeholder="latitude"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_latitude`">
		<input type="text" v-model="waypoint.position.longitude" placeholder="longitude"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_longitude`">
		<input type="text" v-model="waypoint.position.altitude" placeholder="altitude"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_altitude`">
		<input type="text" v-model="waypoint.yaw" placeholder="yaw"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_yaw`">
		<input type="text" v-model="waypoint.hold" placeholder="hold"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_hold`">
		<input type="text" v-model="waypoint.acceptRadius" placeholder="accept radius"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_acceptRadius`">
		<input type="text" v-model="waypoint.passRadius" placeholder="pass radius"
			@keypress="checkCharacter($event)" @keyup="checkWaypoint()" :id="`Waypoint${waypoint.id}_passRadius`">
		<div class="arrow up" @click="moveWaypointUp" v-if="hasUpperNeighbor()"></div>
		<div class="arrow down" @click="moveWaypointDown" v-if="hasLowerNeighbor()"></div>
		<button class="buttonWaypoint buttonRed buttonRemoveWaypoint" @click="removeComponent">Remove</button>
	</div>
</template>

<script lang="ts">

import Waypoint from '../classes/Waypoint'
import { Component, Prop, Vue } from 'vue-property-decorator';
import MissionPlanner from './MissionPlanner.vue';

import L from 'leaflet';

// A component that represents a mission item and allows a user to change its properties

@Component
export default class WaypointComponent extends Vue {
	@Prop() private waypoint!: Waypoint
	@Prop() private removeWaypoint!: () => void
	@Prop() private moveWaypointUp!: () => void
	@Prop() private moveWaypointDown!: () => void
	@Prop() private marker!: L.Marker
	@Prop() private removeMarker!: (marker: L.Marker) => void
	@Prop() private updateWaypoints!: () => void

	missionPlanner = this.$parent as MissionPlanner

	private removeComponent() {
		this.removeWaypoint()
		this.removeMarker(this.marker)
	}

	private hasUpperNeighbor() {
		return this.missionPlanner.getWaypointIndex(this.waypoint.id) != 0;
	}

	private hasLowerNeighbor() {
		return this.missionPlanner.getWaypointIndex(this.waypoint.id) != this.missionPlanner.size - 1;
	}

	// This function highlights fields that have either type or constraint errors in them
	private checkWaypoint() {
		const fields = this.waypoint.errorValues()

		if (fields.length == 0) {
			this.updateMarker();
			this.updateWaypoints();
		}

		const waypointComponents = [
			document.getElementById(`Waypoint${this.waypoint.id}_latitude`),
			document.getElementById(`Waypoint${this.waypoint.id}_longitude`),
			document.getElementById(`Waypoint${this.waypoint.id}_altitude`),
			document.getElementById(`Waypoint${this.waypoint.id}_yaw`),
			document.getElementById(`Waypoint${this.waypoint.id}_hold`),
			document.getElementById(`Waypoint${this.waypoint.id}_acceptRadius`),
			document.getElementById(`Waypoint${this.waypoint.id}_passRadius`)
		]

		if (waypointComponents.filter(item => item == null).length != 0) {
			throw Error("Waypoint not found")
		}

		const position = [
			fields.includes(Waypoint.OPTIONS.latitude),
			fields.includes(Waypoint.OPTIONS.longitude),
			fields.includes(Waypoint.OPTIONS.altitude),
			fields.includes(Waypoint.OPTIONS.yaw),
			fields.includes(Waypoint.OPTIONS.hold),
			fields.includes(Waypoint.OPTIONS.acceptRadius),
			fields.includes(Waypoint.OPTIONS.passRadius)
			]

		position.forEach((value, index) => {
			if (value) {
				waypointComponents[index]?.classList.add("inputTextNaN")
			} else {
				waypointComponents[index]?.classList.remove("inputTextNaN")
			}
		})
	}

	// This function prevents any inputs other than [ '0', '1', ... , '9', '.', '-' ]
    private checkCharacter(event: Event | undefined) {
		// Modified code from:
		//	https://stackoverflow.com/questions/39782176/filter-input-text-only-accept-number-and-dot-vue-js

		event = (event) ? event : window.event;
		if (!event) return

		const evt = event as KeyboardEvent
		var charCode = (evt.which) ? evt.which : evt.keyCode;
		if ((charCode != 45) && (charCode > 31 && (charCode < 48 || charCode > 57)) && charCode !== 46) {
			evt.preventDefault();
		}
    }

	private updateMarker() {
		this.marker.setLatLng(
			new L.LatLng(this.waypoint.position.latitude, this.waypoint.position.longitude, this.waypoint.position.altitude)
		)
	}
}

</script>

<style scoped>

* {
	height: 50px;
}

.arrow.up {
	grid-column: 8;
}

.arrow.down {
	grid-column: 9;
}

.buttonRemoveWaypoint {
	grid-column: 10;
}

input {
	font-size: 18px;
	height: 70%;
	margin: 4px;
}

div.arrow {
	/*
	width: 6vmin;
	height: 6vmin;
	*/
	width: 32px;
	height: 32px;

	box-sizing: border-box;
	left: 50%;
	top: 50%;
	transform: rotate(-45deg);
}
div.arrow::before {
	content: "";
	width: 100%;
	height: 100%;
	border-width: 0.8vmin 0.8vmin 0 0;
	border-style: solid;
	border-color: #fafafa;
	transition: 0.2s ease;
	display: block;
	transform-origin: 100% 0;
}
div.arrow:after {
	content: "";
	float: left;
	position: relative;
	top: -100%;
	width: 100%;
	height: 100%;
	border-width: 0 0.8vmin 0 0;
	border-style: solid;
	border-color: #fafafa;
	transform-origin: 100% 0;
	transition: 0.2s ease;
}
div.arrow:hover::after {
	transform: rotate(45deg);
	border-color: orange;
	height: 120%;
}
div.arrow:hover::before {
	border-color: orange;
	transform: scale(0.8);
}

div.up {
	margin-top: 18px;
	margin-left: 12px;
}

div.down {
	transform: rotate(135deg);
	-webkit-transform: rotate(135deg);
}

#wrap {
  width: 100%;
  max-width: 900px;
  margin: 0 auto 60px;
  box-shadow: 0 3px 6px rgba(0, 0, 0, 0.25);
}

.inputTextNaN {
	background-color: red;
}
</style>
