import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The smoothed, monotonic system state used to feed the control loops of the system.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// x_acc X acceleration in body frame float
// y_acc Y acceleration in body frame float
// z_acc Z acceleration in body frame float
// x_vel X velocity in body frame float
// y_vel Y velocity in body frame float
// z_vel Z velocity in body frame float
// x_pos X position in local frame float
// y_pos Y position in local frame float
// z_pos Z position in local frame float
// airspeed Airspeed, set to -1 if unknown float
// vel_variance Variance of body velocity estimate float
// pos_variance Variance in local position float
// q The attitude, represented as Quaternion float
// roll_rate Angular rate in roll axis float
// pitch_rate Angular rate in pitch axis float
// yaw_rate Angular rate in yaw axis float
export class ControlSystemState extends MAVLinkMessage {
	public time_usec!: number;
	public x_acc!: number;
	public y_acc!: number;
	public z_acc!: number;
	public x_vel!: number;
	public y_vel!: number;
	public z_vel!: number;
	public x_pos!: number;
	public y_pos!: number;
	public z_pos!: number;
	public airspeed!: number;
	public vel_variance!: number;
	public pos_variance!: number;
	public q!: number;
	public roll_rate!: number;
	public pitch_rate!: number;
	public yaw_rate!: number;
	public _message_id: number = 146;
	public _message_name: string = 'CONTROL_SYSTEM_STATE';
	public _crc_extra: number = 103;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['x_acc', 'float', false],
		['y_acc', 'float', false],
		['z_acc', 'float', false],
		['x_vel', 'float', false],
		['y_vel', 'float', false],
		['z_vel', 'float', false],
		['x_pos', 'float', false],
		['y_pos', 'float', false],
		['z_pos', 'float', false],
		['airspeed', 'float', false],
		['vel_variance', 'float', false],
		['pos_variance', 'float', false],
		['q', 'float', false],
		['roll_rate', 'float', false],
		['pitch_rate', 'float', false],
		['yaw_rate', 'float', false],
	];
}