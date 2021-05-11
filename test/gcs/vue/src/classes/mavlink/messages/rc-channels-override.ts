import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The RAW values of the RC channels sent to the MAV to override info received from the RC radio. The standard PPM modulation is as follows: 1000 microseconds: 0%, 2000 microseconds: 100%. Individual receivers/transmitters might violate this specification.  Note carefully the semantic differences between the first 8 channels and the subsequent channels
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// chan1_raw RC channel 1 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan2_raw RC channel 2 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan3_raw RC channel 3 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan4_raw RC channel 4 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan5_raw RC channel 5 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan6_raw RC channel 6 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan7_raw RC channel 7 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan8_raw RC channel 8 value. A value of UINT16_MAX means to ignore this field. A value of 0 means to release this channel back to the RC radio. uint16_t
// chan9_raw RC channel 9 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan10_raw RC channel 10 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan11_raw RC channel 11 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan12_raw RC channel 12 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan13_raw RC channel 13 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan14_raw RC channel 14 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan15_raw RC channel 15 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan16_raw RC channel 16 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan17_raw RC channel 17 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
// chan18_raw RC channel 18 value. A value of 0 or UINT16_MAX means to ignore this field. A value of UINT16_MAX-1 means to release this channel back to the RC radio. uint16_t
export class RcChannelsOverride extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
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
	public chan13_raw!: number;
	public chan14_raw!: number;
	public chan15_raw!: number;
	public chan16_raw!: number;
	public chan17_raw!: number;
	public chan18_raw!: number;
	public _message_id: number = 70;
	public _message_name: string = 'RC_CHANNELS_OVERRIDE';
	public _crc_extra: number = 124;
	public _message_fields: [string, string, boolean][] = [
		['chan1_raw', 'uint16_t', false],
		['chan2_raw', 'uint16_t', false],
		['chan3_raw', 'uint16_t', false],
		['chan4_raw', 'uint16_t', false],
		['chan5_raw', 'uint16_t', false],
		['chan6_raw', 'uint16_t', false],
		['chan7_raw', 'uint16_t', false],
		['chan8_raw', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['chan9_raw', 'uint16_t', true],
		['chan10_raw', 'uint16_t', true],
		['chan11_raw', 'uint16_t', true],
		['chan12_raw', 'uint16_t', true],
		['chan13_raw', 'uint16_t', true],
		['chan14_raw', 'uint16_t', true],
		['chan15_raw', 'uint16_t', true],
		['chan16_raw', 'uint16_t', true],
		['chan17_raw', 'uint16_t', true],
		['chan18_raw', 'uint16_t', true],
	];
}