import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Barometer readings for 3rd barometer
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// press_abs Absolute pressure float
// press_diff Differential pressure float
// temperature Absolute pressure temperature int16_t
// temperature_press_diff Differential pressure temperature (0, if not available). Report values of 0 (or 1) as 1 cdegC. int16_t
export class ScaledPressure3 extends MAVLinkMessage {
	public time_boot_ms!: number;
	public press_abs!: number;
	public press_diff!: number;
	public temperature!: number;
	public temperature_press_diff!: number;
	public _message_id: number = 143;
	public _message_name: string = 'SCALED_PRESSURE3';
	public _crc_extra: number = 131;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['press_abs', 'float', false],
		['press_diff', 'float', false],
		['temperature', 'int16_t', false],
		['temperature_press_diff', 'int16_t', true],
	];
}