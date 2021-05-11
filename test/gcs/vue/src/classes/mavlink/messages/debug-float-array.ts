import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Large debug/prototyping array. The message uses the maximum available payload for data. The array_id and name fields are used to discriminate between messages in code and in user interfaces (respectively). Do not use in production code.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// name Name, for human-friendly display in a Ground Control Station char
// array_id Unique ID used to discriminate between arrays uint16_t
// data data float
export class DebugFloatArray extends MAVLinkMessage {
	public time_usec!: number;
	public name!: string;
	public array_id!: number;
	public data!: number;
	public _message_id: number = 350;
	public _message_name: string = 'DEBUG_FLOAT_ARRAY';
	public _crc_extra: number = 232;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['array_id', 'uint16_t', false],
		['name', 'char', false],
		['data', 'float', true],
	];
}