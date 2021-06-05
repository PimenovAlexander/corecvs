import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavComponent} from '../enums/mav-component';
import {MavMissionType} from '../enums/mav-mission-type';
/*
A broadcast message to notify any ground station or SDK if a mission, geofence or safe points have changed on the vehicle.
*/
// start_index Start index for partial mission change (-1 for all items). int16_t
// end_index End index of a partial mission change. -1 is a synonym for the last mission item (i.e. selects all items from start_index). Ignore field if start_index=-1. int16_t
// origin_sysid System ID of the author of the new mission. uint8_t
// origin_compid Compnent ID of the author of the new mission. uint8_t
// mission_type Mission type. uint8_t
export class MissionChanged extends MAVLinkMessage {
	public start_index!: number;
	public end_index!: number;
	public origin_sysid!: number;
	public origin_compid!: MavComponent;
	public mission_type!: MavMissionType;
	public _message_id: number = 52;
	public _message_name: string = 'MISSION_CHANGED';
	public _crc_extra: number = 132;
	public _message_fields: [string, string, boolean][] = [
		['start_index', 'int16_t', false],
		['end_index', 'int16_t', false],
		['origin_sysid', 'uint8_t', false],
		['origin_compid', 'uint8_t', false],
		['mission_type', 'uint8_t', false],
	];
}