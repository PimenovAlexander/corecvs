import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavFrame} from '../enums/mav-frame';
/*
Vehicle status report that is sent out while orbit execution is in progress (see MAV_CMD_DO_ORBIT).
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// radius Radius of the orbit circle. Positive values orbit clockwise, negative values orbit counter-clockwise. float
// frame The coordinate system of the fields: x, y, z. uint8_t
// x X coordinate of center point. Coordinate system depends on frame field: local = x position in meters * 1e4, global = latitude in degrees * 1e7. int32_t
// y Y coordinate of center point.  Coordinate system depends on frame field: local = x position in meters * 1e4, global = latitude in degrees * 1e7. int32_t
// z Altitude of center point. Coordinate system depends on frame field. float
export class OrbitExecutionStatus extends MAVLinkMessage {
	public time_usec!: number;
	public radius!: number;
	public frame!: MavFrame;
	public x!: number;
	public y!: number;
	public z!: number;
	public _message_id: number = 360;
	public _message_name: string = 'ORBIT_EXECUTION_STATUS';
	public _crc_extra: number = 11;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['radius', 'float', false],
		['x', 'int32_t', false],
		['y', 'int32_t', false],
		['z', 'float', false],
		['frame', 'uint8_t', false],
	];
}