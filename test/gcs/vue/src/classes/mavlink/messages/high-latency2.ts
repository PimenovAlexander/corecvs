import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavType} from '../enums/mav-type';
import {MavAutopilot} from '../enums/mav-autopilot';
import {HlFailureFlag} from '../enums/hl-failure-flag';
/*
Message appropriate for high latency connections like Iridium (version 2)
*/
// timestamp Timestamp (milliseconds since boot or Unix epoch) uint32_t
// type Type of the MAV (quadrotor, helicopter, etc.) uint8_t
// autopilot Autopilot type / class. Use MAV_AUTOPILOT_INVALID for components that are not flight controllers. uint8_t
// custom_mode A bitfield for use for autopilot-specific flags (2 byte version). uint16_t
// latitude Latitude int32_t
// longitude Longitude int32_t
// altitude Altitude above mean sea level int16_t
// target_altitude Altitude setpoint int16_t
// heading Heading uint8_t
// target_heading Heading setpoint uint8_t
// target_distance Distance to target waypoint or position uint16_t
// throttle Throttle uint8_t
// airspeed Airspeed uint8_t
// airspeed_sp Airspeed setpoint uint8_t
// groundspeed Groundspeed uint8_t
// windspeed Windspeed uint8_t
// wind_heading Wind heading uint8_t
// eph Maximum error horizontal position since last message uint8_t
// epv Maximum error vertical position since last message uint8_t
// temperature_air Air temperature from airspeed sensor int8_t
// climb_rate Maximum climb rate magnitude since last message int8_t
// battery Battery level (-1 if field not provided). int8_t
// wp_num Current waypoint number uint16_t
// failure_flags Bitmap of failure flags. uint16_t
// custom0 Field for custom payload. int8_t
// custom1 Field for custom payload. int8_t
// custom2 Field for custom payload. int8_t
export class HighLatency2 extends MAVLinkMessage {
	public timestamp!: number;
	public type!: MavType;
	public autopilot!: MavAutopilot;
	public custom_mode!: number;
	public latitude!: number;
	public longitude!: number;
	public altitude!: number;
	public target_altitude!: number;
	public heading!: number;
	public target_heading!: number;
	public target_distance!: number;
	public throttle!: number;
	public airspeed!: number;
	public airspeed_sp!: number;
	public groundspeed!: number;
	public windspeed!: number;
	public wind_heading!: number;
	public eph!: number;
	public epv!: number;
	public temperature_air!: number;
	public climb_rate!: number;
	public battery!: number;
	public wp_num!: number;
	public failure_flags!: HlFailureFlag;
	public custom0!: number;
	public custom1!: number;
	public custom2!: number;
	public _message_id: number = 235;
	public _message_name: string = 'HIGH_LATENCY2';
	public _crc_extra: number = 179;
	public _message_fields: [string, string, boolean][] = [
		['timestamp', 'uint32_t', false],
		['latitude', 'int32_t', false],
		['longitude', 'int32_t', false],
		['custom_mode', 'uint16_t', false],
		['altitude', 'int16_t', false],
		['target_altitude', 'int16_t', false],
		['target_distance', 'uint16_t', false],
		['wp_num', 'uint16_t', false],
		['failure_flags', 'uint16_t', false],
		['type', 'uint8_t', false],
		['autopilot', 'uint8_t', false],
		['heading', 'uint8_t', false],
		['target_heading', 'uint8_t', false],
		['throttle', 'uint8_t', false],
		['airspeed', 'uint8_t', false],
		['airspeed_sp', 'uint8_t', false],
		['groundspeed', 'uint8_t', false],
		['windspeed', 'uint8_t', false],
		['wind_heading', 'uint8_t', false],
		['eph', 'uint8_t', false],
		['epv', 'uint8_t', false],
		['temperature_air', 'int8_t', false],
		['climb_rate', 'int8_t', false],
		['battery', 'int8_t', false],
		['custom0', 'int8_t', false],
		['custom1', 'int8_t', false],
		['custom2', 'int8_t', false],
	];
}