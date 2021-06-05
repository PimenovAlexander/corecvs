import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GimbalManagerFlags} from '../enums/gimbal-manager-flags';
/*
High level message to control a gimbal's attitude. This message is to be sent to the gimbal manager (e.g. from a ground station). Angles and rates can be set to NaN according to use case.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// flags High level gimbal manager flags to use. uint32_t
// gimbal_device_id Component ID of gimbal device to address (or 1-6 for non-MAVLink gimbal), 0 for all gimbal device components. Send command multiple times for more than one gimbal (but not all gimbals). uint8_t
// q Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation, the frame is depends on whether the flag GIMBAL_MANAGER_FLAGS_YAW_LOCK is set) float
// angular_velocity_x X component of angular velocity, positive is rolling to the right, NaN to be ignored. float
// angular_velocity_y Y component of angular velocity, positive is pitching up, NaN to be ignored. float
// angular_velocity_z Z component of angular velocity, positive is yawing to the right, NaN to be ignored. float
export class GimbalManagerSetAttitude extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public flags!: GimbalManagerFlags;
	public gimbal_device_id!: number;
	public q!: number;
	public angular_velocity_x!: number;
	public angular_velocity_y!: number;
	public angular_velocity_z!: number;
	public _message_id: number = 282;
	public _message_name: string = 'GIMBAL_MANAGER_SET_ATTITUDE';
	public _crc_extra: number = 123;
	public _message_fields: [string, string, boolean][] = [
		['flags', 'uint32_t', false],
		['q', 'float', false],
		['angular_velocity_x', 'float', false],
		['angular_velocity_y', 'float', false],
		['angular_velocity_z', 'float', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['gimbal_device_id', 'uint8_t', false],
	];
}