import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The offset in X, Y, Z and yaw between the LOCAL_POSITION_NED messages of MAV X and the global coordinate frame in NED coordinates. Coordinate frame is right-handed, Z-axis down (aeronautical frame, NED / north-east-down convention)
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// x X Position float
// y Y Position float
// z Z Position float
// roll Roll float
// pitch Pitch float
// yaw Yaw float
export class LocalPositionNedSystemGlobalOffset extends MAVLinkMessage {
	public time_boot_ms!: number;
	public x!: number;
	public y!: number;
	public z!: number;
	public roll!: number;
	public pitch!: number;
	public yaw!: number;
	public _message_id: number = 89;
	public _message_name: string = 'LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET';
	public _crc_extra: number = 231;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['x', 'float', false],
		['y', 'float', false],
		['z', 'float', false],
		['roll', 'float', false],
		['pitch', 'float', false],
		['yaw', 'float', false],
	];
}