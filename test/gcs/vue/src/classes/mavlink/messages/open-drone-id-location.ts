import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavOdidStatus} from '../enums/mav-odid-status';
import {MavOdidHeightRef} from '../enums/mav-odid-height-ref';
import {MavOdidHorAcc} from '../enums/mav-odid-hor-acc';
import {MavOdidVerAcc} from '../enums/mav-odid-ver-acc';
import {MavOdidSpeedAcc} from '../enums/mav-odid-speed-acc';
import {MavOdidTimeAcc} from '../enums/mav-odid-time-acc';
/*
Data for filling the OpenDroneID Location message. The float data types are 32-bit IEEE 754. The Location message provides the location, altitude, direction and speed of the aircraft.
*/
// target_system System ID (0 for broadcast). uint8_t
// target_component Component ID (0 for broadcast). uint8_t
// id_or_mac Only used for drone ID data received from other UAs. See detailed description at https://mavlink.io/en/services/opendroneid.html. uint8_t
// status Indicates whether the unmanned aircraft is on the ground or in the air. uint8_t
// direction Direction over ground (not heading, but direction of movement) measured clockwise from true North: 0 - 35999 centi-degrees. If unknown: 36100 centi-degrees. uint16_t
// speed_horizontal Ground speed. Positive only. If unknown: 25500 cm/s. If speed is larger than 25425 cm/s, use 25425 cm/s. uint16_t
// speed_vertical The vertical speed. Up is positive. If unknown: 6300 cm/s. If speed is larger than 6200 cm/s, use 6200 cm/s. If lower than -6200 cm/s, use -6200 cm/s. int16_t
// latitude Current latitude of the unmanned aircraft. If unknown: 0 (both Lat/Lon). int32_t
// longitude Current longitude of the unmanned aircraft. If unknown: 0 (both Lat/Lon). int32_t
// altitude_barometric The altitude calculated from the barometric pressue. Reference is against 29.92inHg or 1013.2mb. If unknown: -1000 m. float
// altitude_geodetic The geodetic altitude as defined by WGS84. If unknown: -1000 m. float
// height_reference Indicates the reference point for the height field. uint8_t
// height The current height of the unmanned aircraft above the take-off location or the ground as indicated by height_reference. If unknown: -1000 m. float
// horizontal_accuracy The accuracy of the horizontal position. uint8_t
// vertical_accuracy The accuracy of the vertical position. uint8_t
// barometer_accuracy The accuracy of the barometric altitude. uint8_t
// speed_accuracy The accuracy of the horizontal and vertical speed. uint8_t
// timestamp Seconds after the full hour with reference to UTC time. Typically the GPS outputs a time-of-week value in milliseconds. First convert that to UTC and then convert for this field using ((float) (time_week_ms % (60*60*1000))) / 1000. float
// timestamp_accuracy The accuracy of the timestamps. uint8_t
export class OpenDroneIdLocation extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public id_or_mac!: number;
	public status!: MavOdidStatus;
	public direction!: number;
	public speed_horizontal!: number;
	public speed_vertical!: number;
	public latitude!: number;
	public longitude!: number;
	public altitude_barometric!: number;
	public altitude_geodetic!: number;
	public height_reference!: MavOdidHeightRef;
	public height!: number;
	public horizontal_accuracy!: MavOdidHorAcc;
	public vertical_accuracy!: MavOdidVerAcc;
	public barometer_accuracy!: MavOdidVerAcc;
	public speed_accuracy!: MavOdidSpeedAcc;
	public timestamp!: number;
	public timestamp_accuracy!: MavOdidTimeAcc;
	public _message_id: number = 12901;
	public _message_name: string = 'OPEN_DRONE_ID_LOCATION';
	public _crc_extra: number = 254;
	public _message_fields: [string, string, boolean][] = [
		['latitude', 'int32_t', false],
		['longitude', 'int32_t', false],
		['altitude_barometric', 'float', false],
		['altitude_geodetic', 'float', false],
		['height', 'float', false],
		['timestamp', 'float', false],
		['direction', 'uint16_t', false],
		['speed_horizontal', 'uint16_t', false],
		['speed_vertical', 'int16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['id_or_mac', 'uint8_t', false],
		['status', 'uint8_t', false],
		['height_reference', 'uint8_t', false],
		['horizontal_accuracy', 'uint8_t', false],
		['vertical_accuracy', 'uint8_t', false],
		['barometer_accuracy', 'uint8_t', false],
		['speed_accuracy', 'uint8_t', false],
		['timestamp_accuracy', 'uint8_t', false],
	];
}