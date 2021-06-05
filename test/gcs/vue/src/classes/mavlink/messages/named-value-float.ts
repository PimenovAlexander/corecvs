import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Send a key-value pair as float. The use of this message is discouraged for normal packets, but a quite efficient way for testing new messages and getting experimental debug output.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// name Name of the debug variable char
// value Floating point value float
export class NamedValueFloat extends MAVLinkMessage {
	public time_boot_ms!: number;
	public name!: string;
	public value!: number;
	public _message_id: number = 251;
	public _message_name: string = 'NAMED_VALUE_FLOAT';
	public _crc_extra: number = 170;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['value', 'float', false],
		['name', 'char', false],
	];
}