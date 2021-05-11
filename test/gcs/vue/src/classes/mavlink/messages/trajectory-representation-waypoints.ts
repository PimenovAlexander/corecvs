import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavCmd} from '../enums/mav-cmd';
/*
Describe a trajectory using an array of up-to 5 waypoints in the local frame (MAV_FRAME_LOCAL_NED).
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// valid_points Number of valid points (up-to 5 waypoints are possible) uint8_t
// pos_x X-coordinate of waypoint, set to NaN if not being used float
// pos_y Y-coordinate of waypoint, set to NaN if not being used float
// pos_z Z-coordinate of waypoint, set to NaN if not being used float
// vel_x X-velocity of waypoint, set to NaN if not being used float
// vel_y Y-velocity of waypoint, set to NaN if not being used float
// vel_z Z-velocity of waypoint, set to NaN if not being used float
// acc_x X-acceleration of waypoint, set to NaN if not being used float
// acc_y Y-acceleration of waypoint, set to NaN if not being used float
// acc_z Z-acceleration of waypoint, set to NaN if not being used float
// pos_yaw Yaw angle, set to NaN if not being used float
// vel_yaw Yaw rate, set to NaN if not being used float
// command Scheduled action for each waypoint, UINT16_MAX if not being used. uint16_t
export class TrajectoryRepresentationWaypoints extends MAVLinkMessage {
	public time_usec!: number;
	public valid_points!: number;
	public pos_x!: number;
	public pos_y!: number;
	public pos_z!: number;
	public vel_x!: number;
	public vel_y!: number;
	public vel_z!: number;
	public acc_x!: number;
	public acc_y!: number;
	public acc_z!: number;
	public pos_yaw!: number;
	public vel_yaw!: number;
	public command!: MavCmd;
	public _message_id: number = 332;
	public _message_name: string = 'TRAJECTORY_REPRESENTATION_WAYPOINTS';
	public _crc_extra: number = 236;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['pos_x', 'float', false],
		['pos_y', 'float', false],
		['pos_z', 'float', false],
		['vel_x', 'float', false],
		['vel_y', 'float', false],
		['vel_z', 'float', false],
		['acc_x', 'float', false],
		['acc_y', 'float', false],
		['acc_z', 'float', false],
		['pos_yaw', 'float', false],
		['vel_yaw', 'float', false],
		['command', 'uint16_t', false],
		['valid_points', 'uint8_t', false],
	];
}