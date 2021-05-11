import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavCollisionSrc} from '../enums/mav-collision-src';
import {MavCollisionAction} from '../enums/mav-collision-action';
import {MavCollisionThreatLevel} from '../enums/mav-collision-threat-level';
/*
Information about a potential collision
*/
// src Collision data source uint8_t
// id Unique identifier, domain based on src field uint32_t
// action Action that is being taken to avoid this collision uint8_t
// threat_level How concerned the aircraft is about this collision uint8_t
// time_to_minimum_delta Estimated time until collision occurs float
// altitude_minimum_delta Closest vertical distance between vehicle and object float
// horizontal_minimum_delta Closest horizontal distance between vehicle and object float
export class Collision extends MAVLinkMessage {
	public src!: MavCollisionSrc;
	public id!: number;
	public action!: MavCollisionAction;
	public threat_level!: MavCollisionThreatLevel;
	public time_to_minimum_delta!: number;
	public altitude_minimum_delta!: number;
	public horizontal_minimum_delta!: number;
	public _message_id: number = 247;
	public _message_name: string = 'COLLISION';
	public _crc_extra: number = 81;
	public _message_fields: [string, string, boolean][] = [
		['id', 'uint32_t', false],
		['time_to_minimum_delta', 'float', false],
		['altitude_minimum_delta', 'float', false],
		['horizontal_minimum_delta', 'float', false],
		['src', 'uint8_t', false],
		['action', 'uint8_t', false],
		['threat_level', 'uint8_t', false],
	];
}