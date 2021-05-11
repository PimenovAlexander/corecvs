import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The RAW values of the RC channels received. The standard PPM modulation is as follows: 1000 microseconds: 0%, 2000 microseconds: 100%. A value of UINT16_MAX implies the channel is unused. Individual receivers/transmitters might violate this specification.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// port Servo output port (set of 8 outputs = 1 port). Flight stacks running on Pixhawk should use: 0 = MAIN, 1 = AUX. uint8_t
// chan1_raw RC channel 1 value. uint16_t
// chan2_raw RC channel 2 value. uint16_t
// chan3_raw RC channel 3 value. uint16_t
// chan4_raw RC channel 4 value. uint16_t
// chan5_raw RC channel 5 value. uint16_t
// chan6_raw RC channel 6 value. uint16_t
// chan7_raw RC channel 7 value. uint16_t
// chan8_raw RC channel 8 value. uint16_t
// rssi Receive signal strength indicator in device-dependent units/scale. Values: [0-254], 255: invalid/unknown. uint8_t
export class RcChannelsRaw extends MAVLinkMessage {
	public time_boot_ms!: number;
	public port!: number;
	public chan1_raw!: number;
	public chan2_raw!: number;
	public chan3_raw!: number;
	public chan4_raw!: number;
	public chan5_raw!: number;
	public chan6_raw!: number;
	public chan7_raw!: number;
	public chan8_raw!: number;
	public rssi!: number;
	public _message_id: number = 35;
	public _message_name: string = 'RC_CHANNELS_RAW';
	public _crc_extra: number = 244;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['chan1_raw', 'uint16_t', false],
		['chan2_raw', 'uint16_t', false],
		['chan3_raw', 'uint16_t', false],
		['chan4_raw', 'uint16_t', false],
		['chan5_raw', 'uint16_t', false],
		['chan6_raw', 'uint16_t', false],
		['chan7_raw', 'uint16_t', false],
		['chan8_raw', 'uint16_t', false],
		['port', 'uint8_t', false],
		['rssi', 'uint8_t', false],
	];
}