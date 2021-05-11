import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavBatteryFunction} from '../enums/mav-battery-function';
import {MavBatteryType} from '../enums/mav-battery-type';
import {MavBatteryChargeState} from '../enums/mav-battery-charge-state';
import {MavBatteryMode} from '../enums/mav-battery-mode';
import {MavBatteryFault} from '../enums/mav-battery-fault';
/*
Battery information. Updates GCS with flight controller battery status. Smart batteries also use this message, but may additionally send SMART_BATTERY_INFO.
*/
// id Battery ID uint8_t
// battery_function Function of the battery uint8_t
// type Type (chemistry) of the battery uint8_t
// temperature Temperature of the battery. INT16_MAX for unknown temperature. int16_t
// voltages Battery voltage of cells 1 to 10 (see voltages_ext for cells 11-14). Cells in this field above the valid cell count for this battery should have the UINT16_MAX value. If individual cell voltages are unknown or not measured for this battery, then the overall battery voltage should be filled in cell 0, with all others set to UINT16_MAX. If the voltage of the battery is greater than (UINT16_MAX - 1), then cell 0 should be set to (UINT16_MAX - 1), and cell 1 to the remaining voltage. This can be extended to multiple cells if the total voltage is greater than 2 * (UINT16_MAX - 1). uint16_t
// current_battery Battery current, -1: autopilot does not measure the current int16_t
// current_consumed Consumed charge, -1: autopilot does not provide consumption estimate int32_t
// energy_consumed Consumed energy, -1: autopilot does not provide energy consumption estimate int32_t
// battery_remaining Remaining battery energy. Values: [0-100], -1: autopilot does not estimate the remaining battery. int8_t
// time_remaining Remaining battery time, 0: autopilot does not provide remaining battery time estimate int32_t
// charge_state State for extent of discharge, provided by autopilot for warning or external reactions uint8_t
// voltages_ext Battery voltages for cells 11 to 14. Cells above the valid cell count for this battery should have a value of 0, where zero indicates not supported (note, this is different than for the voltages field and allows empty byte truncation). If the measured value is 0 then 1 should be sent instead. uint16_t
// mode Battery mode. Default (0) is that battery mode reporting is not supported or battery is in normal-use mode. uint8_t
// fault_bitmask Fault/health indications. These should be set when charge_state is MAV_BATTERY_CHARGE_STATE_FAILED or MAV_BATTERY_CHARGE_STATE_UNHEALTHY (if not, fault reporting is not supported). uint32_t
export class BatteryStatus extends MAVLinkMessage {
	public id!: number;
	public battery_function!: MavBatteryFunction;
	public type!: MavBatteryType;
	public temperature!: number;
	public voltages!: number;
	public current_battery!: number;
	public current_consumed!: number;
	public energy_consumed!: number;
	public battery_remaining!: number;
	public time_remaining!: number;
	public charge_state!: MavBatteryChargeState;
	public voltages_ext!: number;
	public mode!: MavBatteryMode;
	public fault_bitmask!: MavBatteryFault;
	public _message_id: number = 147;
	public _message_name: string = 'BATTERY_STATUS';
	public _crc_extra: number = 154;
	public _message_fields: [string, string, boolean][] = [
		['current_consumed', 'int32_t', false],
		['energy_consumed', 'int32_t', false],
		['temperature', 'int16_t', false],
		['voltages', 'uint16_t', false],
		['current_battery', 'int16_t', false],
		['id', 'uint8_t', false],
		['battery_function', 'uint8_t', false],
		['type', 'uint8_t', false],
		['battery_remaining', 'int8_t', false],
		['time_remaining', 'int32_t', true],
		['charge_state', 'uint8_t', true],
		['voltages_ext', 'uint16_t', true],
		['mode', 'uint8_t', true],
		['fault_bitmask', 'uint32_t', true],
	];
}