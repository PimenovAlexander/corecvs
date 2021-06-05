import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Send raw controller memory. The use of this message is discouraged for normal packets, but a quite efficient way for testing new messages and getting experimental debug output.
*/
// address Starting address of the debug variables uint16_t
// ver Version code of the type variable. 0=unknown, type ignored and assumed int16_t. 1=as below uint8_t
// type Type code of the memory variables. for ver = 1: 0=16 x int16_t, 1=16 x uint16_t, 2=16 x Q15, 3=16 x 1Q14 uint8_t
// value Memory contents at specified address int8_t
export class MemoryVect extends MAVLinkMessage {
	public address!: number;
	public ver!: number;
	public type!: number;
	public value!: number;
	public _message_id: number = 249;
	public _message_name: string = 'MEMORY_VECT';
	public _crc_extra: number = 204;
	public _message_fields: [string, string, boolean][] = [
		['address', 'uint16_t', false],
		['ver', 'uint8_t', false],
		['type', 'uint8_t', false],
		['value', 'int8_t', false],
	];
}