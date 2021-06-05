import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavMissionType} from '../enums/mav-mission-type';
/*
This message is emitted as response to MISSION_REQUEST_LIST by the MAV and to initiate a write transaction. The GCS can then request the individual mission item based on the knowledge of the total number of waypoints.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// count Number of mission items in the sequence uint16_t
// mission_type Mission type. uint8_t
export class MissionCount extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public count!: number;
	public mission_type!: MavMissionType;
	public _message_id: number = 44;
	public _message_name: string = 'MISSION_COUNT';
	public _crc_extra: number = 221;
	public _message_fields: [string, string, boolean][] = [
		['count', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['mission_type', 'uint8_t', true],
	];
}