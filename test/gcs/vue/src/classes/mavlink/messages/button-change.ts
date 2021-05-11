import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Report button state change.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// last_change_ms Time of last change of button state. uint32_t
// state Bitmap for state of buttons. uint8_t
export class ButtonChange extends MAVLinkMessage {
	public time_boot_ms!: number;
	public last_change_ms!: number;
	public state!: number;
	public _message_id: number = 257;
	public _message_name: string = 'BUTTON_CHANGE';
	public _crc_extra: number = 131;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['last_change_ms', 'uint32_t', false],
		['state', 'uint8_t', false],
	];
}