import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {RtkBaselineCoordinateSystem} from '../enums/rtk-baseline-coordinate-system';
/*
RTK GPS data. Gives information on the relative baseline calculation the GPS is reporting
*/
// time_last_baseline_ms Time since boot of last baseline message received. uint32_t
// rtk_receiver_id Identification of connected RTK receiver. uint8_t
// wn GPS Week Number of last baseline uint16_t
// tow GPS Time of Week of last baseline uint32_t
// rtk_health GPS-specific health report for RTK data. uint8_t
// rtk_rate Rate of baseline messages being received by GPS uint8_t
// nsats Current number of sats used for RTK calculation. uint8_t
// baseline_coords_type Coordinate system of baseline uint8_t
// baseline_a_mm Current baseline in ECEF x or NED north component. int32_t
// baseline_b_mm Current baseline in ECEF y or NED east component. int32_t
// baseline_c_mm Current baseline in ECEF z or NED down component. int32_t
// accuracy Current estimate of baseline accuracy. uint32_t
// iar_num_hypotheses Current number of integer ambiguity hypotheses. int32_t
export class Gps2Rtk extends MAVLinkMessage {
	public time_last_baseline_ms!: number;
	public rtk_receiver_id!: number;
	public wn!: number;
	public tow!: number;
	public rtk_health!: number;
	public rtk_rate!: number;
	public nsats!: number;
	public baseline_coords_type!: RtkBaselineCoordinateSystem;
	public baseline_a_mm!: number;
	public baseline_b_mm!: number;
	public baseline_c_mm!: number;
	public accuracy!: number;
	public iar_num_hypotheses!: number;
	public _message_id: number = 128;
	public _message_name: string = 'GPS2_RTK';
	public _crc_extra: number = 226;
	public _message_fields: [string, string, boolean][] = [
		['time_last_baseline_ms', 'uint32_t', false],
		['tow', 'uint32_t', false],
		['baseline_a_mm', 'int32_t', false],
		['baseline_b_mm', 'int32_t', false],
		['baseline_c_mm', 'int32_t', false],
		['accuracy', 'uint32_t', false],
		['iar_num_hypotheses', 'int32_t', false],
		['wn', 'uint16_t', false],
		['rtk_receiver_id', 'uint8_t', false],
		['rtk_health', 'uint8_t', false],
		['rtk_rate', 'uint8_t', false],
		['nsats', 'uint8_t', false],
		['baseline_coords_type', 'uint8_t', false],
	];
}