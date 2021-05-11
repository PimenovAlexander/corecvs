import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavMissionType} from '../enums/mav-mission-type';
/*
Request a partial list of mission items from the system/component. https://mavlink.io/en/services/mission.html. If start and end index are the same, just send one waypoint.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// start_index Start index int16_t
// end_index End index, -1 by default (-1: send list to end). Else a valid index of the list int16_t
// mission_type Mission type. uint8_t
export class MissionRequestPartialList extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public start_index!: number;
	public end_index!: number;
	public mission_type!: MavMissionType;
	public _message_id: number = 37;
	public _message_name: string = 'MISSION_REQUEST_PARTIAL_LIST';
	public _crc_extra: number = 212;
	public _message_fields: [string, string, boolean][] = [
		['start_index', 'int16_t', false],
		['end_index', 'int16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['mission_type', 'uint8_t', true],
	];
}