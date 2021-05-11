import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The filtered global position (e.g. fused GPS and accelerometers). The position is in GPS-frame (right-handed, Z-up). It
               is designed as scaled integer message since the resolution of float is not sufficient.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// lat Latitude, expressed int32_t
// lon Longitude, expressed int32_t
// alt Altitude (MSL). Note that virtually all GPS modules provide both WGS84 and MSL. int32_t
// relative_alt Altitude above ground int32_t
// vx Ground X Speed (Latitude, positive north) int16_t
// vy Ground Y Speed (Longitude, positive east) int16_t
// vz Ground Z Speed (Altitude, positive down) int16_t
// hdg Vehicle heading (yaw angle), 0.0..359.99 degrees. If unknown, set to: UINT16_MAX uint16_t
export class GlobalPositionInt extends MAVLinkMessage {
	public time_boot_ms!: number;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public relative_alt!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public hdg!: number;
	public _message_id: number = 33;
	public _message_name: string = 'GLOBAL_POSITION_INT';
	public _crc_extra: number = 104;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'int32_t', false],
		['relative_alt', 'int32_t', false],
		['vx', 'int16_t', false],
		['vy', 'int16_t', false],
		['vz', 'int16_t', false],
		['hdg', 'uint16_t', false],
	];
}