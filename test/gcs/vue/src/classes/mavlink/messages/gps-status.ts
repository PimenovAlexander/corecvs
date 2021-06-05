import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The positioning status, as reported by GPS. This message is intended to display status information about each satellite visible to the receiver. See message GLOBAL_POSITION for the global position estimate. This message can contain information for up to 20 satellites.
*/
// satellites_visible Number of satellites visible uint8_t
// satellite_prn Global satellite ID uint8_t
// satellite_used 0: Satellite not used, 1: used for localization uint8_t
// satellite_elevation Elevation (0: right on top of receiver, 90: on the horizon) of satellite uint8_t
// satellite_azimuth Direction of satellite, 0: 0 deg, 255: 360 deg. uint8_t
// satellite_snr Signal to noise ratio of satellite uint8_t
export class GpsStatus extends MAVLinkMessage {
	public satellites_visible!: number;
	public satellite_prn!: number;
	public satellite_used!: number;
	public satellite_elevation!: number;
	public satellite_azimuth!: number;
	public satellite_snr!: number;
	public _message_id: number = 25;
	public _message_name: string = 'GPS_STATUS';
	public _crc_extra: number = 23;
	public _message_fields: [string, string, boolean][] = [
		['satellites_visible', 'uint8_t', false],
		['satellite_prn', 'uint8_t', false],
		['satellite_used', 'uint8_t', false],
		['satellite_elevation', 'uint8_t', false],
		['satellite_azimuth', 'uint8_t', false],
		['satellite_snr', 'uint8_t', false],
	];
}