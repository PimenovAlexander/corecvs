import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavParamExtType} from '../enums/mav-param-ext-type';
/*
Emit the value of a parameter. The inclusion of param_count and param_index in the message allows the recipient to keep track of received parameters and allows them to re-request missing parameters after a loss or timeout.
*/
// param_id Parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string char
// param_value Parameter value char
// param_type Parameter type. uint8_t
// param_count Total number of parameters uint16_t
// param_index Index of this parameter uint16_t
export class ParamExtValue extends MAVLinkMessage {
	public param_id!: string;
	public param_value!: string;
	public param_type!: MavParamExtType;
	public param_count!: number;
	public param_index!: number;
	public _message_id: number = 322;
	public _message_name: string = 'PARAM_EXT_VALUE';
	public _crc_extra: number = 243;
	public _message_fields: [string, string, boolean][] = [
		['param_count', 'uint16_t', false],
		['param_index', 'uint16_t', false],
		['param_id', 'char', false],
		['param_value', 'char', false],
		['param_type', 'uint8_t', false],
	];
}