import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavMode} from '../enums/mav-mode';
/*
Set the system mode, as defined by enum MAV_MODE. There is no target component id as the mode is by definition for the overall aircraft, not only for one component.
*/
// target_system The system setting the mode uint8_t
// base_mode The new base mode. uint8_t
// custom_mode The new autopilot-specific mode. This field can be ignored by an autopilot. uint32_t
export class SetMode extends MAVLinkMessage {
	public target_system!: number;
	public base_mode!: MavMode;
	public custom_mode!: number;
	public _message_id: number = 11;
	public _message_name: string = 'SET_MODE';
	public _crc_extra: number = 89;
	public _message_fields: [string, string, boolean][] = [
		['custom_mode', 'uint32_t', false],
		['target_system', 'uint8_t', false],
		['base_mode', 'uint8_t', false],
	];
}