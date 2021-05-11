import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Vibration levels and accelerometer clipping
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// vibration_x Vibration levels on X-axis float
// vibration_y Vibration levels on Y-axis float
// vibration_z Vibration levels on Z-axis float
// clipping_0 first accelerometer clipping count uint32_t
// clipping_1 second accelerometer clipping count uint32_t
// clipping_2 third accelerometer clipping count uint32_t
export class Vibration extends MAVLinkMessage {
	public time_usec!: number;
	public vibration_x!: number;
	public vibration_y!: number;
	public vibration_z!: number;
	public clipping_0!: number;
	public clipping_1!: number;
	public clipping_2!: number;
	public _message_id: number = 241;
	public _message_name: string = 'VIBRATION';
	public _crc_extra: number = 90;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['vibration_x', 'float', false],
		['vibration_y', 'float', false],
		['vibration_z', 'float', false],
		['clipping_0', 'uint32_t', false],
		['clipping_1', 'uint32_t', false],
		['clipping_2', 'uint32_t', false],
	];
}