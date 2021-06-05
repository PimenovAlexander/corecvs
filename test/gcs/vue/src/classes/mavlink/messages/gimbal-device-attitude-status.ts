import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GimbalDeviceFlags} from '../enums/gimbal-device-flags';
import {GimbalDeviceErrorFlags} from '../enums/gimbal-device-error-flags';
/*
Message reporting the status of a gimbal device. This message should be broadcasted by a gimbal device component. The angles encoded in the quaternion are in the global frame (roll: positive is rolling to the right, pitch: positive is pitching up, yaw is turn to the right). This message should be broadcast at a low regular rate (e.g. 10Hz).
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// time_boot_ms Timestamp (time since system boot). uint32_t
// flags Current gimbal flags set. uint16_t
// q Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation, the frame is depends on whether the flag GIMBAL_DEVICE_FLAGS_YAW_LOCK is set) float
// angular_velocity_x X component of angular velocity (NaN if unknown) float
// angular_velocity_y Y component of angular velocity (NaN if unknown) float
// angular_velocity_z Z component of angular velocity (NaN if unknown) float
// failure_flags Failure flags (0 for no failure) uint32_t
export class GimbalDeviceAttitudeStatus extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public time_boot_ms!: number;
	public flags!: GimbalDeviceFlags;
	public q!: number;
	public angular_velocity_x!: number;
	public angular_velocity_y!: number;
	public angular_velocity_z!: number;
	public failure_flags!: GimbalDeviceErrorFlags;
	public _message_id: number = 285;
	public _message_name: string = 'GIMBAL_DEVICE_ATTITUDE_STATUS';
	public _crc_extra: number = 137;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['q', 'float', false],
		['angular_velocity_x', 'float', false],
		['angular_velocity_y', 'float', false],
		['angular_velocity_z', 'float', false],
		['failure_flags', 'uint32_t', false],
		['flags', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}