import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {FenceBreach} from '../enums/fence-breach';
import {FenceMitigate} from '../enums/fence-mitigate';
/*
Status of geo-fencing. Sent in extended status stream when fencing enabled.
*/
// breach_status Breach status (0 if currently inside fence, 1 if outside). uint8_t
// breach_count Number of fence breaches. uint16_t
// breach_type Last breach type. uint8_t
// breach_time Time (since boot) of last breach. uint32_t
// breach_mitigation Active action to prevent fence breach uint8_t
export class FenceStatus extends MAVLinkMessage {
	public breach_status!: number;
	public breach_count!: number;
	public breach_type!: FenceBreach;
	public breach_time!: number;
	public breach_mitigation!: FenceMitigate;
	public _message_id: number = 162;
	public _message_name: string = 'FENCE_STATUS';
	public _crc_extra: number = 189;
	public _message_fields: [string, string, boolean][] = [
		['breach_time', 'uint32_t', false],
		['breach_count', 'uint16_t', false],
		['breach_status', 'uint8_t', false],
		['breach_type', 'uint8_t', false],
		['breach_mitigation', 'uint8_t', true],
	];
}