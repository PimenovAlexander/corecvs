import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavMissionType} from '../enums/mav-mission-type';
/*
Request the information of the mission item with the sequence number seq. The response of the system to this message should be a MISSION_ITEM_INT message. https://mavlink.io/en/services/mission.html
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// seq Sequence uint16_t
// mission_type Mission type. uint8_t
export class MissionRequestInt extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public seq!: number;
	public mission_type!: MavMissionType;
	public _message_id: number = 51;
	public _message_name: string = 'MISSION_REQUEST_INT';
	public _crc_extra: number = 196;
	public _message_fields: [string, string, boolean][] = [
		['seq', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['mission_type', 'uint8_t', true],
	];
}