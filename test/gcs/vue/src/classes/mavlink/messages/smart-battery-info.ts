import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavBatteryFunction} from '../enums/mav-battery-function';
import {MavBatteryType} from '../enums/mav-battery-type';
/*
Smart Battery information (static/infrequent update). Use for updates from: smart battery to flight stack, flight stack to GCS. Use BATTERY_STATUS for smart battery frequent updates.
*/
// id Battery ID uint8_t
// battery_function Function of the battery uint8_t
// type Type (chemistry) of the battery uint8_t
// capacity_full_specification Capacity when full according to manufacturer, -1: field not provided. int32_t
// capacity_full Capacity when full (accounting for battery degradation), -1: field not provided. int32_t
// cycle_count Charge/discharge cycle count. UINT16_MAX: field not provided. uint16_t
// serial_number Serial number in ASCII characters, 0 terminated. All 0: field not provided. char
// device_name Static device name. Encode as manufacturer and product names separated using an underscore. char
// weight Battery weight. 0: field not provided. uint16_t
// discharge_minimum_voltage Minimum per-cell voltage when discharging. If not supplied set to UINT16_MAX value. uint16_t
// charging_minimum_voltage Minimum per-cell voltage when charging. If not supplied set to UINT16_MAX value. uint16_t
// resting_minimum_voltage Minimum per-cell voltage when resting. If not supplied set to UINT16_MAX value. uint16_t
export class SmartBatteryInfo extends MAVLinkMessage {
	public id!: number;
	public battery_function!: MavBatteryFunction;
	public type!: MavBatteryType;
	public capacity_full_specification!: number;
	public capacity_full!: number;
	public cycle_count!: number;
	public serial_number!: string;
	public device_name!: string;
	public weight!: number;
	public discharge_minimum_voltage!: number;
	public charging_minimum_voltage!: number;
	public resting_minimum_voltage!: number;
	public _message_id: number = 370;
	public _message_name: string = 'SMART_BATTERY_INFO';
	public _crc_extra: number = 75;
	public _message_fields: [string, string, boolean][] = [
		['capacity_full_specification', 'int32_t', false],
		['capacity_full', 'int32_t', false],
		['cycle_count', 'uint16_t', false],
		['weight', 'uint16_t', false],
		['discharge_minimum_voltage', 'uint16_t', false],
		['charging_minimum_voltage', 'uint16_t', false],
		['resting_minimum_voltage', 'uint16_t', false],
		['id', 'uint8_t', false],
		['battery_function', 'uint8_t', false],
		['type', 'uint8_t', false],
		['serial_number', 'char', false],
		['device_name', 'char', false],
	];
}