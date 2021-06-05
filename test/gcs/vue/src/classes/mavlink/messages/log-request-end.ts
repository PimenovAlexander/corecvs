import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Stop log transfer and resume normal logging
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
export class LogRequestEnd extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public _message_id: number = 122;
	public _message_name: string = 'LOG_REQUEST_END';
	public _crc_extra: number = 203;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}