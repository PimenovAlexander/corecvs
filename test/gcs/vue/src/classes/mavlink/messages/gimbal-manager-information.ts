import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GimbalManagerCapFlags} from '../enums/gimbal-manager-cap-flags';
/*
Information about a high level gimbal manager. This message should be requested by a ground station using MAV_CMD_REQUEST_MESSAGE.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// cap_flags Bitmap of gimbal capability flags. uint32_t
// gimbal_device_id Gimbal device ID that this gimbal manager is responsible for. uint8_t
// roll_min Minimum hardware roll angle (positive: rolling to the right, negative: rolling to the left) float
// roll_max Maximum hardware roll angle (positive: rolling to the right, negative: rolling to the left) float
// pitch_min Minimum pitch angle (positive: up, negative: down) float
// pitch_max Maximum pitch angle (positive: up, negative: down) float
// yaw_min Minimum yaw angle (positive: to the right, negative: to the left) float
// yaw_max Maximum yaw angle (positive: to the right, negative: to the left) float
export class GimbalManagerInformation extends MAVLinkMessage {
	public time_boot_ms!: number;
	public cap_flags!: GimbalManagerCapFlags;
	public gimbal_device_id!: number;
	public roll_min!: number;
	public roll_max!: number;
	public pitch_min!: number;
	public pitch_max!: number;
	public yaw_min!: number;
	public yaw_max!: number;
	public _message_id: number = 280;
	public _message_name: string = 'GIMBAL_MANAGER_INFORMATION';
	public _crc_extra: number = 70;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['cap_flags', 'uint32_t', false],
		['roll_min', 'float', false],
		['roll_max', 'float', false],
		['pitch_min', 'float', false],
		['pitch_max', 'float', false],
		['yaw_min', 'float', false],
		['yaw_max', 'float', false],
		['gimbal_device_id', 'uint8_t', false],
	];
}