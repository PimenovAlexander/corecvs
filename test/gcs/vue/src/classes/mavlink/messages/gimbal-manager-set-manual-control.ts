import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GimbalManagerFlags} from '../enums/gimbal-manager-flags';
/*
High level message to control a gimbal manually. The angles or angular rates are unitless; the actual rates will depend on internal gimbal manager settings/configuration (e.g. set by parameters). This message is to be sent to the gimbal manager (e.g. from a ground station). Angles and rates can be set to NaN according to use case.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// flags High level gimbal manager flags. uint32_t
// gimbal_device_id Component ID of gimbal device to address (or 1-6 for non-MAVLink gimbal), 0 for all gimbal device components. Send command multiple times for more than one gimbal (but not all gimbals). uint8_t
// pitch Pitch angle unitless (-1..1, positive: up, negative: down, NaN to be ignored). float
// yaw Yaw angle unitless (-1..1, positive: to the right, negative: to the left, NaN to be ignored). float
// pitch_rate Pitch angular rate unitless (-1..1, positive: up, negative: down, NaN to be ignored). float
// yaw_rate Yaw angular rate unitless (-1..1, positive: to the right, negative: to the left, NaN to be ignored). float
export class GimbalManagerSetManualControl extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public flags!: GimbalManagerFlags;
	public gimbal_device_id!: number;
	public pitch!: number;
	public yaw!: number;
	public pitch_rate!: number;
	public yaw_rate!: number;
	public _message_id: number = 288;
	public _message_name: string = 'GIMBAL_MANAGER_SET_MANUAL_CONTROL';
	public _crc_extra: number = 20;
	public _message_fields: [string, string, boolean][] = [
		['flags', 'uint32_t', false],
		['pitch', 'float', false],
		['yaw', 'float', false],
		['pitch_rate', 'float', false],
		['yaw_rate', 'float', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['gimbal_device_id', 'uint8_t', false],
	];
}