
<template>
	<div id="tables">
	<table class="table-fill" id="tableProperties">
		<thead>
		<tr>
			<th>Property</th>
			<th>Value</th>
		</tr>
		</thead>
		<tbody class="table-hover">
			<tr v-for="property in this.properties.filter(item => !item.userControlled)" :key="`property${property.name}`">
				<td>{{property.name}}</td>
				<td>{{property.value}}</td>
			</tr>
		</tbody>
	</table>
	<table class="table-fill" id="properties">
		<thead>
		<tr>
			<th>Property</th>
			<th>Value</th>
		</tr>
		</thead>

		<tbody class="table-hover" id="tableProperties">
		<tr v-for="property in this.properties.filter(item => item.userControlled)" :key="`property${property.name}`">
			<td>{{property.name}}</td>
			<td>
				<input class="property_confirmed" type="number" :value="property.value" @keyup="setParam(property.name, property.value)">
			</td>
		</tr>
		</tbody>
	</table>
	</div>
</template>

<script lang="ts">

import { ParamSet } from '../classes/mavlink/messages/param-set';

import { Component, Vue, Prop } from 'vue-property-decorator';
import { MavParamType } from '@/classes/mavlink/enums/mav-param-type';

import { send } from '../classes/AjaxRequest';

// A component that displays text data coming from UAV and allows changing manually controlled fields

class Property {
	name: string
	userControlled: boolean
	value: number

	constructor(name: string, userControlled: boolean, value: number) {
		this.name = name
		this.userControlled = userControlled
		this.value = value
	}
}

@Component
export default class Properties extends Vue {
	@Prop() private SYSTEM_ID!: () => number;
	@Prop() private COMPONENT_ID!: () => number;

	properties: Property[] = []

	mounted() {
		// This is a mock as a list of actual properties has to come from UAV based on it's current set of attached equipment
		const viewProperties = [
			'ViewProperty1', 'ViewProperty2', 'ViewProperty3'
		]
		viewProperties.forEach(property => {
			this.properties.push({
				name: property,
				userControlled: false,
				value: 0
			})
		})
		const properties = [
			'Row', 'Pitch', 'Yaw', 'Thrust'
		]
		properties.forEach(property => {
			this.properties.push({
				name: property,
				userControlled: true,
				value: 0
			})
		})
	}

	private setParam(): void {
		const setParamMessage = new ParamSet(this.SYSTEM_ID(), this.COMPONENT_ID())
		setParamMessage.param_id = "1";
		setParamMessage.param_value = 2;
		setParamMessage.param_type = MavParamType.MAV_PARAM_TYPE_INT32;

		console.log('Sending param')

		send('mavlink/setParam', setParamMessage)
	}
}

</script>

<style scoped>

#properties input {
	border: 2px solid #008000;
	width: 230px;
	/* border: 2px solid #456879; */
	border-radius:10px;
}
#properties input:focus {
	outline: none;
}
.property_confirmed {
	background-color : #99FFCC;
}
.property_loading {
	background-color : #FFFF50;
}
.property_failed {
	background-color : red;
}
.table-fill {
	background: white;
	border-radius:3px;
	border-collapse: collapse;
	width: 100%;
	padding:5px;
	box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);
	animation: float 5s infinite;
}
th {
	color:#D5DDE5;;
	background:#1b1e24;
	border-bottom:4px solid #9ea7af;
	border-right: 1px solid #343a45;
	font-size:23px;
	font-weight: 100;
	padding:24px;
	text-align:left;
	text-shadow: 0 1px 1px rgba(0, 0, 0, 0.1);
	vertical-align:middle;
}
tr {
	border-top: 1px solid #C1C3D1;
	border-bottom: 1px solid #C1C3D1;
	color:#666B85;
	font-size:16px;
	font-weight:normal;
	text-shadow: 0 1px 1px rgba(256, 256, 256, 0.1);
}
tr:hover td {
	background:#4E5066;
	color:#FFFFFF;
	border-top: 1px solid #22262e;
}
tr:nth-child(odd) td {
	background:#EBEBEB;
}
tr:nth-child(odd):hover td {
	background:#4E5066;
}
td {
	background:#FFFFFF;
	padding:20px;
	text-align:left;
	vertical-align:middle;
	font-weight:300;
	font-size:18px;
	text-shadow: -1px -1px 1px rgba(0, 0, 0, 0.1);
	border-right: 1px solid #C1C3D1;
	height: 32px;
}
td input {
	font-size: 18px;
}
#tables {
	display: grid;
	grid-template-columns: 10% repeat(2, 40%) 10%;
	margin-top: 0.2%;
}
#tableProperties {
	grid-column: 2;
}
#properties {
	grid-column: 3;
}

</style>