import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavParamType} from '../enums/mav-param-type';
import {ParamAck} from '../enums/param-ack';
/*
Response from a PARAM_SET message when it is used in a transaction.
*/
// target_system Id of system that sent PARAM_SET message. uint8_t
// target_component Id of system that sent PARAM_SET message. uint8_t
// param_id Parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string char
// param_value Parameter value (new value if PARAM_ACCEPTED, current value otherwise) float
// param_type Parameter type. uint8_t
// param_result Result code. uint8_t
export class ParamAckTransaction extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public param_id!: string;
	public param_value!: number;
	public param_type!: MavParamType;
	public param_result!: ParamAck;
	public _message_id: number = 19;
	public _message_name: string = 'PARAM_ACK_TRANSACTION';
	public _crc_extra: number = 137;
	public _message_fields: [string, string, boolean][] = [
		['param_value', 'float', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['param_id', 'char', false],
		['param_type', 'uint8_t', false],
		['param_result', 'uint8_t', false],
	];
}