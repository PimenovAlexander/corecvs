import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavMissionType} from '../enums/mav-mission-type';
/*
Request the overall list of mission items from the system/component.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// mission_type Mission type. uint8_t
export class MissionRequestList extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public mission_type!: MavMissionType;
	public _message_id: number = 43;
	public _message_name: string = 'MISSION_REQUEST_LIST';
	public _crc_extra: number = 132;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['mission_type', 'uint8_t', true],
	];
}