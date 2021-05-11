import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The RAW pressure readings for the typical setup of one absolute pressure and one differential pressure sensor. The sensor values should be the raw, UNSCALED ADC values.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// press_abs Absolute pressure (raw) int16_t
// press_diff1 Differential pressure 1 (raw, 0 if nonexistent) int16_t
// press_diff2 Differential pressure 2 (raw, 0 if nonexistent) int16_t
// temperature Raw Temperature measurement (raw) int16_t
export class RawPressure extends MAVLinkMessage {
	public time_usec!: number;
	public press_abs!: number;
	public press_diff1!: number;
	public press_diff2!: number;
	public temperature!: number;
	public _message_id: number = 28;
	public _message_name: string = 'RAW_PRESSURE';
	public _crc_extra: number = 67;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['press_abs', 'int16_t', false],
		['press_diff1', 'int16_t', false],
		['press_diff2', 'int16_t', false],
		['temperature', 'int16_t', false],
	];
}