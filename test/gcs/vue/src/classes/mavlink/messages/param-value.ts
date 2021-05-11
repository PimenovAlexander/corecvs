import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavParamType} from '../enums/mav-param-type';
/*
Emit the value of a onboard parameter. The inclusion of param_count and param_index in the message allows the recipient to keep track of received parameters and allows him to re-request missing parameters after a loss or timeout. The parameter microservice is documented at https://mavlink.io/en/services/parameter.html
*/
// param_id Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string char
// param_value Onboard parameter value float
// param_type Onboard parameter type. uint8_t
// param_count Total number of onboard parameters uint16_t
// param_index Index of this onboard parameter uint16_t
export class ParamValue extends MAVLinkMessage {
	public param_id!: string;
	public param_value!: number;
	public param_type!: MavParamType;
	public param_count!: number;
	public param_index!: number;
	public _message_id: number = 22;
	public _message_name: string = 'PARAM_VALUE';
	public _crc_extra: number = 220;
	public _message_fields: [string, string, boolean][] = [
		['param_value', 'float', false],
		['param_count', 'uint16_t', false],
		['param_index', 'uint16_t', false],
		['param_id', 'char', false],
		['param_type', 'uint8_t', false],
	];
}