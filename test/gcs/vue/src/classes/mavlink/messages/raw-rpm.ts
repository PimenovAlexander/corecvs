import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
RPM sensor data message.
*/
// index Index of this RPM sensor (0-indexed) uint8_t
// frequency Indicated rate float
export class RawRpm extends MAVLinkMessage {
	public index!: number;
	public frequency!: number;
	public _message_id: number = 339;
	public _message_name: string = 'RAW_RPM';
	public _crc_extra: number = 199;
	public _message_fields: [string, string, boolean][] = [
		['frequency', 'float', false],
		['index', 'uint8_t', false],
	];
}