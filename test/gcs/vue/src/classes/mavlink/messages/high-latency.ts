import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavModeFlag} from '../enums/mav-mode-flag';
import {MavLandedState} from '../enums/mav-landed-state';
import {GpsFixType} from '../enums/gps-fix-type';
/*
Message appropriate for high latency connections like Iridium
*/
// base_mode Bitmap of enabled system modes. uint8_t
// custom_mode A bitfield for use for autopilot-specific flags. uint32_t
// landed_state The landed state. Is set to MAV_LANDED_STATE_UNDEFINED if landed state is unknown. uint8_t
// roll roll int16_t
// pitch pitch int16_t
// heading heading uint16_t
// throttle throttle (percentage) int8_t
// heading_sp heading setpoint int16_t
// latitude Latitude int32_t
// longitude Longitude int32_t
// altitude_amsl Altitude above mean sea level int16_t
// altitude_sp Altitude setpoint relative to the home position int16_t
// airspeed airspeed uint8_t
// airspeed_sp airspeed setpoint uint8_t
// groundspeed groundspeed uint8_t
// climb_rate climb rate int8_t
// gps_nsat Number of satellites visible. If unknown, set to 255 uint8_t
// gps_fix_type GPS Fix type. uint8_t
// battery_remaining Remaining battery (percentage) uint8_t
// temperature Autopilot temperature (degrees C) int8_t
// temperature_air Air temperature (degrees C) from airspeed sensor int8_t
// failsafe failsafe (each bit represents a failsafe where 0=ok, 1=failsafe active (bit0:RC, bit1:batt, bit2:GPS, bit3:GCS, bit4:fence) uint8_t
// wp_num current waypoint number uint8_t
// wp_distance distance to target uint16_t
export class HighLatency extends MAVLinkMessage {
	public base_mode!: MavModeFlag;
	public custom_mode!: number;
	public landed_state!: MavLandedState;
	public roll!: number;
	public pitch!: number;
	public heading!: number;
	public throttle!: number;
	public heading_sp!: number;
	public latitude!: number;
	public longitude!: number;
	public altitude_amsl!: number;
	public altitude_sp!: number;
	public airspeed!: number;
	public airspeed_sp!: number;
	public groundspeed!: number;
	public climb_rate!: number;
	public gps_nsat!: number;
	public gps_fix_type!: GpsFixType;
	public battery_remaining!: number;
	public temperature!: number;
	public temperature_air!: number;
	public failsafe!: number;
	public wp_num!: number;
	public wp_distance!: number;
	public _message_id: number = 234;
	public _message_name: string = 'HIGH_LATENCY';
	public _crc_extra: number = 150;
	public _message_fields: [string, string, boolean][] = [
		['custom_mode', 'uint32_t', false],
		['latitude', 'int32_t', false],
		['longitude', 'int32_t', false],
		['roll', 'int16_t', false],
		['pitch', 'int16_t', false],
		['heading', 'uint16_t', false],
		['heading_sp', 'int16_t', false],
		['altitude_amsl', 'int16_t', false],
		['altitude_sp', 'int16_t', false],
		['wp_distance', 'uint16_t', false],
		['base_mode', 'uint8_t', false],
		['landed_state', 'uint8_t', false],
		['throttle', 'int8_t', false],
		['airspeed', 'uint8_t', false],
		['airspeed_sp', 'uint8_t', false],
		['groundspeed', 'uint8_t', false],
		['climb_rate', 'int8_t', false],
		['gps_nsat', 'uint8_t', false],
		['gps_fix_type', 'uint8_t', false],
		['battery_remaining', 'uint8_t', false],
		['temperature', 'int8_t', false],
		['temperature_air', 'int8_t', false],
		['failsafe', 'uint8_t', false],
		['wp_num', 'uint8_t', false],
	];
}