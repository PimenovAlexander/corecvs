import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavParamExtType} from '../enums/mav-param-ext-type';
import {ParamAck} from '../enums/param-ack';
/*
Response from a PARAM_EXT_SET message.
*/
// param_id Parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string char
// param_value Parameter value (new value if PARAM_ACK_ACCEPTED, current value otherwise) char
// param_type Parameter type. uint8_t
// param_result Result code. uint8_t
export class ParamExtAck extends MAVLinkMessage {
	public param_id!: string;
	public param_value!: string;
	public param_type!: MavParamExtType;
	public param_result!: ParamAck;
	public _message_id: number = 324;
	public _message_name: string = 'PARAM_EXT_ACK';
	public _crc_extra: number = 132;
	public _message_fields: [string, string, boolean][] = [
		['param_id', 'char', false],
		['param_value', 'char', false],
		['param_type', 'uint8_t', false],
		['param_result', 'uint8_t', false],
	];
}