import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {GpsFixType} from '../enums/gps-fix-type';
/*
Second GPS data.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// fix_type GPS fix type. uint8_t
// lat Latitude (WGS84) int32_t
// lon Longitude (WGS84) int32_t
// alt Altitude (MSL). Positive for up. int32_t
// eph GPS HDOP horizontal dilution of position (unitless). If unknown, set to: UINT16_MAX uint16_t
// epv GPS VDOP vertical dilution of position (unitless). If unknown, set to: UINT16_MAX uint16_t
// vel GPS ground speed. If unknown, set to: UINT16_MAX uint16_t
// cog Course over ground (NOT heading, but direction of movement): 0.0..359.99 degrees. If unknown, set to: UINT16_MAX uint16_t
// satellites_visible Number of satellites visible. If unknown, set to 255 uint8_t
// dgps_numch Number of DGPS satellites uint8_t
// dgps_age Age of DGPS info uint32_t
// yaw Yaw in earth frame from north. Use 0 if this GPS does not provide yaw. Use 65535 if this GPS is configured to provide yaw and is currently unable to provide it. Use 36000 for north. uint16_t
export class Gps2Raw extends MAVLinkMessage {
	public time_usec!: number;
	public fix_type!: GpsFixType;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public eph!: number;
	public epv!: number;
	public vel!: number;
	public cog!: number;
	public satellites_visible!: number;
	public dgps_numch!: number;
	public dgps_age!: number;
	public yaw!: number;
	public _message_id: number = 124;
	public _message_name: string = 'GPS2_RAW';
	public _crc_extra: number = 87;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'int32_t', false],
		['dgps_age', 'uint32_t', false],
		['eph', 'uint16_t', false],
		['epv', 'uint16_t', false],
		['vel', 'uint16_t', false],
		['cog', 'uint16_t', false],
		['fix_type', 'uint8_t', false],
		['satellites_visible', 'uint8_t', false],
		['dgps_numch', 'uint8_t', false],
		['yaw', 'uint16_t', true],
	];
}