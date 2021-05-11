
<template>
	<div id="waypoints">
		<div class="waypoint">
			<label>Latitude</label>
			<label>Longitude</label>
			<label>Altitude</label>
			<label>Yaw</label>
			<label>Hold</label>
			<label>Accept radius</label>
			<label>Pass radius</label>
			<button id="buttonAddWaypoint" class="buttonWaypoint buttonGreen" @click="addWaypoint()">Add a waypoint</button>
		</div>
		<WaypointComponent v-for="waypoint in waypoints" :key="`Waypoint${waypoint.id}`"
			:waypoint="waypoint"
			:removeWaypoint="_ => { removeWaypoint(waypoint.id) }"
			:moveWaypointUp="_ => { moveWaypointUp(waypoint.id) }"
			:moveWaypointDown="_ => { moveWaypointDown(waypoint.id) }"
			:marker="getMarker(waypoint.id)"
			:updateWaypoints="updateWaypoints"
			:removeMarker="removeMarker"/>
	</div>
</template>

<script lang="ts">

import { Component, Vue, Prop } from 'vue-property-decorator';

import * as $ from "jquery";

import { mavLink } from '../classes/mavlink'

import { empty, set, get } from '@typed/hashmap'

import L from 'leaflet';

import Waypoint from '../classes/Waypoint'
import Position from '../classes/Position'

import WaypointComponent from './Waypoint.vue'
import { Heartbeat } from '@/classes/mavlink/messages/heartbeat';
import { MissionItem } from '@/classes/mavlink/messages/mission-item';
import { MissionClearAll } from '@/classes/mavlink/messages/mission-clear-all';
import { MavFrame } from '@/classes/mavlink/enums/mav-frame';
import { MavCmd } from '@/classes/mavlink/enums/mav-cmd';
import { MavMissionType } from '@/classes/mavlink/enums/mav-mission-type';
import { send } from '@/classes/AjaxRequest';

@Component({
	components: {
		WaypointComponent
	},
})

// A component that allows the user to add, change and remove waypoints

export default class MissionPlanner extends Vue {
	@Prop() private startingWaypoints!: Waypoint[]
	@Prop() private position!: Position
	@Prop() private addMarker!: (marker: L.Marker) => void
	@Prop() private removeMarker!: (marker: L.Marker) => void
	@Prop() private SYSTEM_ID!: () => number;
	@Prop() private COMPONENT_ID!: () => number;

	markers = empty<number, L.Marker>();

	waypoints = [ ...this.startingWaypoints ]

	get size(): number {
		return this.waypoints.length
	}

	// Test function that returns a position which is close to starting position
	// Used to assign position to added waypoints before user enters exact coordinates needed for added mission item
	getWaypointLocation(): Position {
		return new Position(
			this.position.latitude + (Math.random() % 100 * 0.001),
			this.position.longitude + (Math.random() % 100 * 0.001),
			this.position.altitude
		)
	}

	addWaypoint(): void {
		const waypoint = new Waypoint(this.getWaypointLocation())

		const marker = new L.Marker(L.latLng(
			waypoint.position.latitude,
			waypoint.position.longitude
		))

		this.addMarker(marker)

		this.markers = set(waypoint.id, marker, this.markers)

		this.waypoints.push(waypoint)

		const missionItem = new MissionItem(this.SYSTEM_ID(), this.COMPONENT_ID())
		missionItem.seq = this.waypoints.length - 1
		missionItem.frame = MavFrame.MAV_FRAME_GLOBAL
		missionItem.command = MavCmd.MAV_CMD_NAV_WAYPOINT
		missionItem.current = 0
		missionItem.autocontinue = 1
		missionItem.x = waypoint.position.latitude
		missionItem.y = waypoint.position.longitude
		missionItem.z = waypoint.position.altitude
		missionItem.mission_type = MavMissionType.MAV_MISSION_TYPE_MISSION
		
		send('mavlink/missionItem', missionItem)
	}

	getMarker(id: number) {
		return get(id, this.markers)
	}

	removeWaypoint(id: number): void {
		this.waypoints = this.waypoints.filter(waypoint => {
			return waypoint.id != id
		})
		this.updateWaypoints()
	}

	getWaypointIndex(id: number): number {
		const waypoints = this.waypoints.map((waypoint, ind) => ({
			waypoint, ind
		})).filter(item => item.waypoint.id == id)

		if (waypoints.length == 0) return -1;

		return waypoints[0].ind
	}

	moveWaypointUp(id: number): void {
		if (this.waypoints.length < 2) return;

		const ind = this.getWaypointIndex(id)

		if (ind < 1) return;

		// This is a solution introduced in ES6
		// It's both elegant and efficient in terms of the memory usage as well as in terms of CPU performance
		// It also doesn't work because JS doesn't allow Vue to observe changes related to the order of elements in the array
		/*
		[this.waypoints[ind - 1], this.waypoints[ind]] = [this.waypoints[ind], this.waypoints[ind - 1]]
		*/

		const waypoints = [ this.waypoints[ind], this.waypoints[ind - 1] ]
		this.waypoints.splice(ind - 1, 2, waypoints[0], waypoints[1])

		this.updateWaypoints()
	}

	moveWaypointDown(id: number): void {
		if (this.waypoints.length < 2) return;

		const ind = this.getWaypointIndex(id)

		if (ind < 0) return;
		if (ind >= this.waypoints.length - 1) return;

		const waypoints = [ this.waypoints[ind + 1], this.waypoints[ind] ]
		this.waypoints.splice(ind, 2, waypoints[0], waypoints[1])

		this.updateWaypoints()
	}

	updateWaypoints(): void {
		const missionClearAll = new MissionClearAll(this.SYSTEM_ID(), this.COMPONENT_ID())
		missionClearAll.mission_type = MavMissionType.MAV_MISSION_TYPE_MISSION

		const missionItems = []

		for(let i = 0; i < this.waypoints.length; i++) {
			missionItems.push(new MissionItem(this.SYSTEM_ID(), this.COMPONENT_ID()))
			const missionItem = missionItems[i];
			missionItem.seq = i
			missionItem.frame = MavFrame.MAV_FRAME_GLOBAL
			missionItem.command = MavCmd.MAV_CMD_NAV_WAYPOINT
			missionItem.current = 0
			missionItem.autocontinue = 1
			missionItem.x = this.waypoints[i].position.latitude
			missionItem.y = this.waypoints[i].position.longitude
			missionItem.z = this.waypoints[i].position.altitude
			missionItem.mission_type = MavMissionType.MAV_MISSION_TYPE_MISSION
		}

		send('mavlink/clearAll', missionClearAll)

		missionItems.forEach(item => {
			send('mavlink/missionItem', item)
		})
	}
}
</script>

<style>
.waypoint {
	display: grid;
	grid-template-columns: 20% 20% 9% 5% 5% 5% 5% 5.5% 5.5% 250px;
	padding: 2px;
	background-color: #abc;
	border: solid 2px black;
	left: 0;
	right: 0;
	box-sizing: border-box;
	height: 60px;
	width: 80%;
	min-width: 1450px;
	margin-left: 200px;
}

#waypoints {
	display: grid;
	width: 80%;
	margin-top: 1%;
	box-sizing: border-box;
}

#buttonAddWaypoint {
	margin: 4px;
	grid-column: 10;
}

.buttonRed {
	background: #a00;
}

label {
	font-size: 22px;
	/*
	margin: auto;
	*/
}

.buttonWaypoint {
	font-family: Hack, monospace;
	color: #ffffff;
	cursor: pointer;
	font-size: 2em;
	padding: 0.5rem;
	border: 0;
	transition: all 0.5s;
	border-radius: 10px;
	position: relative;
	box-sizing: border-box;
}

.buttonWaypoint::after {
	content: "〉";
	font-family: "Font Awesome 5 Pro";
	font-weight: 400;
	position: absolute;
	left: 85%;
	top: 5%;
	right: 5%;
	bottom: 0;
	opacity: 0;
}

.buttonRemoveWaypoint:hover {
	background-color: red;
	box-shadow: 0px 6px 15px #a00;
}

.buttonWaypoint:hover {
	transition: all 0.5s;
	border-radius: 10px;
	padding: 0.5rem 3.5rem 0.5rem 0.5rem;
}

.buttonWaypoint:hover::after {
	opacity: 1;
	transition: all 0.5s;
}

.buttonGreen {
	font-size: 20px;
	background-color: green;
}

.buttonGreen:hover {
	background-color: green;
	box-shadow: 0px 6px 15px #0a0;
}
</style>
