import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Set the mission item with sequence number seq as current item. This means that the MAV will continue to this mission item on the shortest path (not following the mission items in-between).
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// seq Sequence uint16_t
export class MissionSetCurrent extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public seq!: number;
	public _message_id: number = 41;
	public _message_name: string = 'MISSION_SET_CURRENT';
	public _crc_extra: number = 28;
	public _message_fields: [string, string, boolean][] = [
		['seq', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}