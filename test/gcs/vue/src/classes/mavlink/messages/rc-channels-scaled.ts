import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The scaled values of the RC channels received: (-100%) -10000, (0%) 0, (100%) 10000. Channels that are inactive should be set to UINT16_MAX.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// port Servo output port (set of 8 outputs = 1 port). Flight stacks running on Pixhawk should use: 0 = MAIN, 1 = AUX. uint8_t
// chan1_scaled RC channel 1 value scaled. int16_t
// chan2_scaled RC channel 2 value scaled. int16_t
// chan3_scaled RC channel 3 value scaled. int16_t
// chan4_scaled RC channel 4 value scaled. int16_t
// chan5_scaled RC channel 5 value scaled. int16_t
// chan6_scaled RC channel 6 value scaled. int16_t
// chan7_scaled RC channel 7 value scaled. int16_t
// chan8_scaled RC channel 8 value scaled. int16_t
// rssi Receive signal strength indicator in device-dependent units/scale. Values: [0-254], 255: invalid/unknown. uint8_t
export class RcChannelsScaled extends MAVLinkMessage {
	public time_boot_ms!: number;
	public port!: number;
	public chan1_scaled!: number;
	public chan2_scaled!: number;
	public chan3_scaled!: number;
	public chan4_scaled!: number;
	public chan5_scaled!: number;
	public chan6_scaled!: number;
	public chan7_scaled!: number;
	public chan8_scaled!: number;
	public rssi!: number;
	public _message_id: number = 34;
	public _message_name: string = 'RC_CHANNELS_SCALED';
	public _crc_extra: number = 237;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['chan1_scaled', 'int16_t', false],
		['chan2_scaled', 'int16_t', false],
		['chan3_scaled', 'int16_t', false],
		['chan4_scaled', 'int16_t', false],
		['chan5_scaled', 'int16_t', false],
		['chan6_scaled', 'int16_t', false],
		['chan7_scaled', 'int16_t', false],
		['chan8_scaled', 'int16_t', false],
		['port', 'uint8_t', false],
		['rssi', 'uint8_t', false],
	];
}