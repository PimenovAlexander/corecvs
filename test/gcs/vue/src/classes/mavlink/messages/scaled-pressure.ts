import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The pressure readings for the typical setup of one absolute and differential pressure sensor. The units are as specified in each field.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// press_abs Absolute pressure float
// press_diff Differential pressure 1 float
// temperature Absolute pressure temperature int16_t
// temperature_press_diff Differential pressure temperature (0, if not available). Report values of 0 (or 1) as 1 cdegC. int16_t
export class ScaledPressure extends MAVLinkMessage {
	public time_boot_ms!: number;
	public press_abs!: number;
	public press_diff!: number;
	public temperature!: number;
	public temperature_press_diff!: number;
	public _message_id: number = 29;
	public _message_name: string = 'SCALED_PRESSURE';
	public _crc_extra: number = 115;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['press_abs', 'float', false],
		['press_diff', 'float', false],
		['temperature', 'int16_t', false],
		['temperature_press_diff', 'int16_t', true],
	];
}