import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
A message containing logged data (see also MAV_CMD_LOGGING_START)
*/
// target_system system ID of the target uint8_t
// target_component component ID of the target uint8_t
// sequence sequence number (can wrap) uint16_t
// length data length uint8_t
// first_message_offset offset into data where first message starts. This can be used for recovery, when a previous message got lost (set to 255 if no start exists). uint8_t
// data logged data uint8_t
export class LoggingData extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public sequence!: number;
	public length!: number;
	public first_message_offset!: number;
	public data!: number;
	public _message_id: number = 266;
	public _message_name: string = 'LOGGING_DATA';
	public _crc_extra: number = 193;
	public _message_fields: [string, string, boolean][] = [
		['sequence', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['length', 'uint8_t', false],
		['first_message_offset', 'uint8_t', false],
		['data', 'uint8_t', false],
	];
}