import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Request all parameters of this component. All parameters should be emitted in response as PARAM_EXT_VALUE.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
export class ParamExtRequestList extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public _message_id: number = 321;
	public _message_name: string = 'PARAM_EXT_REQUEST_LIST';
	public _crc_extra: number = 88;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}