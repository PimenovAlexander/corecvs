import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GimbalManagerFlags} from '../enums/gimbal-manager-flags';
/*
Current status about a high level gimbal manager. This message should be broadcast at a low regular rate (e.g. 5Hz).
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// flags High level gimbal manager flags currently applied. uint32_t
// gimbal_device_id Gimbal device ID that this gimbal manager is responsible for. uint8_t
// primary_control_sysid System ID of MAVLink component with primary control, 0 for none. uint8_t
// primary_control_compid Component ID of MAVLink component with primary control, 0 for none. uint8_t
// secondary_control_sysid System ID of MAVLink component with secondary control, 0 for none. uint8_t
// secondary_control_compid Component ID of MAVLink component with secondary control, 0 for none. uint8_t
export class GimbalManagerStatus extends MAVLinkMessage {
	public time_boot_ms!: number;
	public flags!: GimbalManagerFlags;
	public gimbal_device_id!: number;
	public primary_control_sysid!: number;
	public primary_control_compid!: number;
	public secondary_control_sysid!: number;
	public secondary_control_compid!: number;
	public _message_id: number = 281;
	public _message_name: string = 'GIMBAL_MANAGER_STATUS';
	public _crc_extra: number = 48;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['flags', 'uint32_t', false],
		['gimbal_device_id', 'uint8_t', false],
		['primary_control_sysid', 'uint8_t', false],
		['primary_control_compid', 'uint8_t', false],
		['secondary_control_sysid', 'uint8_t', false],
		['secondary_control_compid', 'uint8_t', false],
	];
}