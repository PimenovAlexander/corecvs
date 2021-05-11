import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
A certain mission item has been reached. The system will either hold this position (or circle on the orbit) or (if the autocontinue on the WP was set) continue to the next waypoint.
*/
// seq Sequence uint16_t
export class MissionItemReached extends MAVLinkMessage {
	public seq!: number;
	public _message_id: number = 46;
	public _message_name: string = 'MISSION_ITEM_REACHED';
	public _crc_extra: number = 11;
	public _message_fields: [string, string, boolean][] = [
		['seq', 'uint16_t', false],
	];
}