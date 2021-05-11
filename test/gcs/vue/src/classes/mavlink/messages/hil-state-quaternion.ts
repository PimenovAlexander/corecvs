import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Sent from simulation to autopilot, avoids in contrast to HIL_STATE singularities. This packet is useful for high throughput applications such as hardware in the loop simulations.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// attitude_quaternion Vehicle attitude expressed as normalized quaternion in w, x, y, z order (with 1 0 0 0 being the null-rotation) float
// rollspeed Body frame roll / phi angular speed float
// pitchspeed Body frame pitch / theta angular speed float
// yawspeed Body frame yaw / psi angular speed float
// lat Latitude int32_t
// lon Longitude int32_t
// alt Altitude int32_t
// vx Ground X Speed (Latitude) int16_t
// vy Ground Y Speed (Longitude) int16_t
// vz Ground Z Speed (Altitude) int16_t
// ind_airspeed Indicated airspeed uint16_t
// true_airspeed True airspeed uint16_t
// xacc X acceleration int16_t
// yacc Y acceleration int16_t
// zacc Z acceleration int16_t
export class HilStateQuaternion extends MAVLinkMessage {
	public time_usec!: number;
	public attitude_quaternion!: number;
	public rollspeed!: number;
	public pitchspeed!: number;
	public yawspeed!: number;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public ind_airspeed!: number;
	public true_airspeed!: number;
	public xacc!: number;
	public yacc!: number;
	public zacc!: number;
	public _message_id: number = 115;
	public _message_name: string = 'HIL_STATE_QUATERNION';
	public _crc_extra: number = 4;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['attitude_quaternion', 'float', false],
		['rollspeed', 'float', false],
		['pitchspeed', 'float', false],
		['yawspeed', 'float', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'int32_t', false],
		['vx', 'int16_t', false],
		['vy', 'int16_t', false],
		['vz', 'int16_t', false],
		['ind_airspeed', 'uint16_t', false],
		['true_airspeed', 'uint16_t', false],
		['xacc', 'int16_t', false],
		['yacc', 'int16_t', false],
		['zacc', 'int16_t', false],
	];
}