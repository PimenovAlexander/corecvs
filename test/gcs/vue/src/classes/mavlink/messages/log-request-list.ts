import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Request a list of available logs. On some systems calling this may stop on-board logging until LOG_REQUEST_END is called. If there are no log files available this request shall be answered with one LOG_ENTRY message with id = 0 and num_logs = 0.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// start First log id (0 for first available) uint16_t
// end Last log id (0xffff for last available) uint16_t
export class LogRequestList extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public start!: number;
	public end!: number;
	public _message_id: number = 117;
	public _message_name: string = 'LOG_REQUEST_LIST';
	public _crc_extra: number = 128;
	public _message_fields: [string, string, boolean][] = [
		['start', 'uint16_t', false],
		['end', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}