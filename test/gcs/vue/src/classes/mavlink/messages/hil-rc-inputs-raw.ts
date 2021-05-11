import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Sent from simulation to autopilot. The RAW values of the RC channels received. The standard PPM modulation is as follows: 1000 microseconds: 0%, 2000 microseconds: 100%. Individual receivers/transmitters might violate this specification.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// chan1_raw RC channel 1 value uint16_t
// chan2_raw RC channel 2 value uint16_t
// chan3_raw RC channel 3 value uint16_t
// chan4_raw RC channel 4 value uint16_t
// chan5_raw RC channel 5 value uint16_t
// chan6_raw RC channel 6 value uint16_t
// chan7_raw RC channel 7 value uint16_t
// chan8_raw RC channel 8 value uint16_t
// chan9_raw RC channel 9 value uint16_t
// chan10_raw RC channel 10 value uint16_t
// chan11_raw RC channel 11 value uint16_t
// chan12_raw RC channel 12 value uint16_t
// rssi Receive signal strength indicator in device-dependent units/scale. Values: [0-254], 255: invalid/unknown. uint8_t
export class HilRcInputsRaw extends MAVLinkMessage {
	public time_usec!: number;
	public chan1_raw!: number;
	public chan2_raw!: number;
	public chan3_raw!: number;
	public chan4_raw!: number;
	public chan5_raw!: number;
	public chan6_raw!: number;
	public chan7_raw!: number;
	public chan8_raw!: number;
	public chan9_raw!: number;
	public chan10_raw!: number;
	public chan11_raw!: number;
	public chan12_raw!: number;
	public rssi!: number;
	public _message_id: number = 92;
	public _message_name: string = 'HIL_RC_INPUTS_RAW';
	public _crc_extra: number = 54;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['chan1_raw', 'uint16_t', false],
		['chan2_raw', 'uint16_t', false],
		['chan3_raw', 'uint16_t', false],
		['chan4_raw', 'uint16_t', false],
		['chan5_raw', 'uint16_t', false],
		['chan6_raw', 'uint16_t', false],
		['chan7_raw', 'uint16_t', false],
		['chan8_raw', 'uint16_t', false],
		['chan9_raw', 'uint16_t', false],
		['chan10_raw', 'uint16_t', false],
		['chan11_raw', 'uint16_t', false],
		['chan12_raw', 'uint16_t', false],
		['rssi', 'uint8_t', false],
	];
}