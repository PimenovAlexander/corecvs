import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The filtered local position (e.g. fused computer vision and accelerometers). Coordinate frame is right-handed, Z-axis down (aeronautical frame, NED / north-east-down convention)
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// x X Position float
// y Y Position float
// z Z Position float
// vx X Speed float
// vy Y Speed float
// vz Z Speed float
export class LocalPositionNed extends MAVLinkMessage {
	public time_boot_ms!: number;
	public x!: number;
	public y!: number;
	public z!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public _message_id: number = 32;
	public _message_name: string = 'LOCAL_POSITION_NED';
	public _crc_extra: number = 185;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['x', 'float', false],
		['y', 'float', false],
		['z', 'float', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
	];
}