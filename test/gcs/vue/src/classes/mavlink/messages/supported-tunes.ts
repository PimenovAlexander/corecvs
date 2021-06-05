import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {TuneFormat} from '../enums/tune-format';
/*
Tune formats supported by vehicle. This should be emitted as response to MAV_CMD_REQUEST_MESSAGE.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// format Bitfield of supported tune formats. uint32_t
export class SupportedTunes extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public format!: TuneFormat;
	public _message_id: number = 401;
	public _message_name: string = 'SUPPORTED_TUNES';
	public _crc_extra: number = 183;
	public _message_fields: [string, string, boolean][] = [
		['format', 'uint32_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}