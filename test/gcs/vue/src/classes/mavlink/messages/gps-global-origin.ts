import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Publishes the GPS co-ordinates of the vehicle local origin (0,0,0) position. Emitted whenever a new GPS-Local position mapping is requested or set - e.g. following SET_GPS_GLOBAL_ORIGIN message.
*/
// latitude Latitude (WGS84) int32_t
// longitude Longitude (WGS84) int32_t
// altitude Altitude (MSL). Positive for up. int32_t
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
export class GpsGlobalOrigin extends MAVLinkMessage {
	public latitude!: number;
	public longitude!: number;
	public altitude!: number;
	public time_usec!: number;
	public _message_id: number = 49;
	public _message_name: string = 'GPS_GLOBAL_ORIGIN';
	public _crc_extra: number = 39;
	public _message_fields: [string, string, boolean][] = [
		['latitude', 'int32_t', false],
		['longitude', 'int32_t', false],
		['altitude', 'int32_t', false],
		['time_usec', 'uint64_t', true],
	];
}