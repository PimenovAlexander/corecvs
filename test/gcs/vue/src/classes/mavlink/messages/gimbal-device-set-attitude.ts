import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GimbalDeviceFlags} from '../enums/gimbal-device-flags';
/*
Low level message to control a gimbal device's attitude. This message is to be sent from the gimbal manager to the gimbal device component. Angles and rates can be set to NaN according to use case.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// flags Low level gimbal flags. uint16_t
// q Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation, the frame is depends on whether the flag GIMBAL_DEVICE_FLAGS_YAW_LOCK is set, set all fields to NaN if only angular velocity should be used) float
// angular_velocity_x X component of angular velocity, positive is rolling to the right, NaN to be ignored. float
// angular_velocity_y Y component of angular velocity, positive is pitching up, NaN to be ignored. float
// angular_velocity_z Z component of angular velocity, positive is yawing to the right, NaN to be ignored. float
export class GimbalDeviceSetAttitude extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public flags!: GimbalDeviceFlags;
	public q!: number;
	public angular_velocity_x!: number;
	public angular_velocity_y!: number;
	public angular_velocity_z!: number;
	public _message_id: number = 284;
	public _message_name: string = 'GIMBAL_DEVICE_SET_ATTITUDE';
	public _crc_extra: number = 99;
	public _message_fields: [string, string, boolean][] = [
		['q', 'float', false],
		['angular_velocity_x', 'float', false],
		['angular_velocity_y', 'float', false],
		['angular_velocity_z', 'float', false],
		['flags', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}