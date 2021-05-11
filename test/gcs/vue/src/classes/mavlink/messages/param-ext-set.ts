import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavParamExtType} from '../enums/mav-param-ext-type';
/*
Set a parameter value. In order to deal with message loss (and retransmission of PARAM_EXT_SET), when setting a parameter value and the new value is the same as the current value, you will immediately get a PARAM_ACK_ACCEPTED response. If the current state is PARAM_ACK_IN_PROGRESS, you will accordingly receive a PARAM_ACK_IN_PROGRESS in response.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// param_id Parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string char
// param_value Parameter value char
// param_type Parameter type. uint8_t
export class ParamExtSet extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public param_id!: string;
	public param_value!: string;
	public param_type!: MavParamExtType;
	public _message_id: number = 323;
	public _message_name: string = 'PARAM_EXT_SET';
	public _crc_extra: number = 78;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['param_id', 'char', false],
		['param_value', 'char', false],
		['param_type', 'uint8_t', false],
	];
}