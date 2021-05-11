import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Time synchronization message.
*/
// tc1 Time sync timestamp 1 int64_t
// ts1 Time sync timestamp 2 int64_t
export class Timesync extends MAVLinkMessage {
	public tc1!: number;
	public ts1!: number;
	public _message_id: number = 111;
	public _message_name: string = 'TIMESYNC';
	public _crc_extra: number = 34;
	public _message_fields: [string, string, boolean][] = [
		['tc1', 'int64_t', false],
		['ts1', 'int64_t', false],
	];
}