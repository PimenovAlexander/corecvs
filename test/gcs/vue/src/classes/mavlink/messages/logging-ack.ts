import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
An ack for a LOGGING_DATA_ACKED message
*/
// target_system system ID of the target uint8_t
// target_component component ID of the target uint8_t
// sequence sequence number (must match the one in LOGGING_DATA_ACKED) uint16_t
export class LoggingAck extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public sequence!: number;
	public _message_id: number = 268;
	public _message_name: string = 'LOGGING_ACK';
	public _crc_extra: number = 14;
	public _message_fields: [string, string, boolean][] = [
		['sequence', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}