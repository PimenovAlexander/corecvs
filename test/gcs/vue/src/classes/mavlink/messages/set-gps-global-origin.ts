import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Sets the GPS co-ordinates of the vehicle local origin (0,0,0) position. Vehicle should emit GPS_GLOBAL_ORIGIN irrespective of whether the origin is changed. This enables transform between the local coordinate frame and the global (GPS) coordinate frame, which may be necessary when (for example) indoor and outdoor settings are connected and the MAV should move from in- to outdoor.
*/
// target_system System ID uint8_t
// latitude Latitude (WGS84) int32_t
// longitude Longitude (WGS84) int32_t
// altitude Altitude (MSL). Positive for up. int32_t
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
export class SetGpsGlobalOrigin extends MAVLinkMessage {
	public target_system!: number;
	public latitude!: number;
	public longitude!: number;
	public altitude!: number;
	public time_usec!: number;
	public _message_id: number = 48;
	public _message_name: string = 'SET_GPS_GLOBAL_ORIGIN';
	public _crc_extra: number = 41;
	public _message_fields: [string, string, boolean][] = [
		['latitude', 'int32_t', false],
		['longitude', 'int32_t', false],
		['altitude', 'int32_t', false],
		['target_system', 'uint8_t', false],
		['time_usec', 'uint64_t', true],
	];
}