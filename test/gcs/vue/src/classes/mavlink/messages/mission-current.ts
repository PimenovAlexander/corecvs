import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Message that announces the sequence number of the current active mission item. The MAV will fly towards this mission item.
*/
// seq Sequence uint16_t
export class MissionCurrent extends MAVLinkMessage {
	public seq!: number;
	public _message_id: number = 42;
	public _message_name: string = 'MISSION_CURRENT';
	public _crc_extra: number = 28;
	public _message_fields: [string, string, boolean][] = [
		['seq', 'uint16_t', false],
	];
}