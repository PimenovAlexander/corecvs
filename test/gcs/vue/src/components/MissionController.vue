
<template>
	<div id="missionController">
		<button id="buttonStartMission" class="buttonWaypoint buttonGreen" @click="startMission()">Start Mission</button>
		<button id="buttonStartMission" class="buttonWaypoint buttonRed" @click="pauseMission()">Pause</button>
		<button id="buttonSwitchFlightMode" class="buttonWaypoint buttonGreen" @click="switchManualMode()"></button>
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

import { MissionSetCurrent } from '../classes/mavlink/messages/mission-set-current';

import { send } from '../classes/AjaxRequest';

// A component that allows user to start and pause mission as well as switch between automatic and manual control modes

@Component
export default class MissionController extends Vue {
	@Prop() private SYSTEM_ID!: () => number;
	@Prop() private COMPONENT_ID!: () => number;
	@Prop() private getManual!: () => boolean;
	@Prop() private setManual!: (value: boolean) => void;

	get manualControlMode() {
		return this.getManual();
	}

	set manualControlMode(value: boolean) {
		this.setManual(value);
	}

	mounted() {
		this.switchManualMode();
	}

	startMission() {
		const missionSetCurrent = new MissionSetCurrent(this.SYSTEM_ID(), this.COMPONENT_ID());
		missionSetCurrent.seq = 0;

		send('mavlink/missionSetCurrent', missionSetCurrent);
	}

	pauseMission() {
		const missionSetCurrent = new MissionSetCurrent(this.SYSTEM_ID(), this.COMPONENT_ID());
		missionSetCurrent.seq = 65535;

		send('mavlink/pauseMission', missionSetCurrent);
	}

	setActiveMissionItem(missionItemIndex: number) {
		const missionSetCurrent = new MissionSetCurrent(this.SYSTEM_ID(), this.COMPONENT_ID());
		missionSetCurrent.seq = missionItemIndex;

		send('mavlink/missionSetCurrent', missionSetCurrent);
	}

	switchManualMode() {
		this.manualControlMode = !this.manualControlMode;
		document.getElementById('buttonSwitchFlightMode')!.textContent = `Switch to ${this.manualControlMode ? 'Automatic' : 'Manual'} Mode`;
	}
}
</script>

<style scoped>
#missionController {
	width: 80%;
	margin: auto;
	margin-top: 0.2%;
	border: solid 4px black;
	background-color: #ABC;
	border-radius: 40px;

	display: grid;
	grid-template-columns: repeat(2, 20%) 30%;
}

#buttonStartMission {
	grid-column: 0;
}

button {
	height: 50px;
	margin: 10px;
}
</style>
