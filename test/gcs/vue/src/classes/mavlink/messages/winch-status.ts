import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavWinchStatusFlag} from '../enums/mav-winch-status-flag';
/*
Winch status.
*/
// time_usec Timestamp (synced to UNIX time or since system boot). uint64_t
// line_length Length of line released. NaN if unknown float
// speed Speed line is being released or retracted. Positive values if being released, negative values if being retracted, NaN if unknown float
// tension Tension on the line. NaN if unknown float
// voltage Voltage of the battery supplying the winch. NaN if unknown float
// current Current draw from the winch. NaN if unknown float
// temperature Temperature of the motor. INT16_MAX if unknown int16_t
// status Status flags uint32_t
export class WinchStatus extends MAVLinkMessage {
	public time_usec!: number;
	public line_length!: number;
	public speed!: number;
	public tension!: number;
	public voltage!: number;
	public current!: number;
	public temperature!: number;
	public status!: MavWinchStatusFlag;
	public _message_id: number = 9005;
	public _message_name: string = 'WINCH_STATUS';
	public _crc_extra: number = 117;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['line_length', 'float', false],
		['speed', 'float', false],
		['tension', 'float', false],
		['voltage', 'float', false],
		['current', 'float', false],
		['status', 'uint32_t', false],
		['temperature', 'int16_t', false],
	];
}