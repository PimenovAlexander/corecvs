import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavEstimatorType} from '../enums/mav-estimator-type';
/*
The filtered global position (e.g. fused GPS and accelerometers). The position is in GPS-frame (right-handed, Z-up). It  is designed as scaled integer message since the resolution of float is not sufficient. NOTE: This message is intended for onboard networks / companion computers and higher-bandwidth links and optimized for accuracy and completeness. Please use the GLOBAL_POSITION_INT message for a minimal subset.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// estimator_type Class id of the estimator this estimate originated from. uint8_t
// lat Latitude int32_t
// lon Longitude int32_t
// alt Altitude in meters above MSL int32_t
// relative_alt Altitude above ground int32_t
// vx Ground X Speed (Latitude) float
// vy Ground Y Speed (Longitude) float
// vz Ground Z Speed (Altitude) float
// covariance Row-major representation of a 6x6 position and velocity 6x6 cross-covariance matrix (states: lat, lon, alt, vx, vy, vz; first six entries are the first ROW, next six entries are the second row, etc.). If unknown, assign NaN value to first element in the array. float
export class GlobalPositionIntCov extends MAVLinkMessage {
	public time_usec!: number;
	public estimator_type!: MavEstimatorType;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public relative_alt!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public covariance!: number;
	public _message_id: number = 63;
	public _message_name: string = 'GLOBAL_POSITION_INT_COV';
	public _crc_extra: number = 119;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'int32_t', false],
		['relative_alt', 'int32_t', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
		['covariance', 'float', false],
		['estimator_type', 'uint8_t', false],
	];
}