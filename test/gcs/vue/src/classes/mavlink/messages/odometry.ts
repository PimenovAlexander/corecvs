import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavFrame} from '../enums/mav-frame';
import {MavEstimatorType} from '../enums/mav-estimator-type';
/*
Odometry message to communicate odometry information with an external interface. Fits ROS REP 147 standard for aerial vehicles (http://www.ros.org/reps/rep-0147.html).
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// frame_id Coordinate frame of reference for the pose data. uint8_t
// child_frame_id Coordinate frame of reference for the velocity in free space (twist) data. uint8_t
// x X Position float
// y Y Position float
// z Z Position float
// q Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation) float
// vx X linear speed float
// vy Y linear speed float
// vz Z linear speed float
// rollspeed Roll angular speed float
// pitchspeed Pitch angular speed float
// yawspeed Yaw angular speed float
// pose_covariance Row-major representation of a 6x6 pose cross-covariance matrix upper right triangle (states: x, y, z, roll, pitch, yaw; first six entries are the first ROW, next five entries are the second ROW, etc.). If unknown, assign NaN value to first element in the array. float
// velocity_covariance Row-major representation of a 6x6 velocity cross-covariance matrix upper right triangle (states: vx, vy, vz, rollspeed, pitchspeed, yawspeed; first six entries are the first ROW, next five entries are the second ROW, etc.). If unknown, assign NaN value to first element in the array. float
// reset_counter Estimate reset counter. This should be incremented when the estimate resets in any of the dimensions (position, velocity, attitude, angular speed). This is designed to be used when e.g an external SLAM system detects a loop-closure and the estimate jumps. uint8_t
// estimator_type Type of estimator that is providing the odometry. uint8_t
export class Odometry extends MAVLinkMessage {
	public time_usec!: number;
	public frame_id!: MavFrame;
	public child_frame_id!: MavFrame;
	public x!: number;
	public y!: number;
	public z!: number;
	public q!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public rollspeed!: number;
	public pitchspeed!: number;
	public yawspeed!: number;
	public pose_covariance!: number;
	public velocity_covariance!: number;
	public reset_counter!: number;
	public estimator_type!: MavEstimatorType;
	public _message_id: number = 331;
	public _message_name: string = 'ODOMETRY';
	public _crc_extra: number = 91;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['x', 'float', false],
		['y', 'float', false],
		['z', 'float', false],
		['q', 'float', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
		['rollspeed', 'float', false],
		['pitchspeed', 'float', false],
		['yawspeed', 'float', false],
		['pose_covariance', 'float', false],
		['velocity_covariance', 'float', false],
		['frame_id', 'uint8_t', false],
		['child_frame_id', 'uint8_t', false],
		['reset_counter', 'uint8_t', true],
		['estimator_type', 'uint8_t', true],
	];
}