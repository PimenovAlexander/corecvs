import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Wind covariance estimate from vehicle.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// wind_x Wind in X (NED) direction float
// wind_y Wind in Y (NED) direction float
// wind_z Wind in Z (NED) direction float
// var_horiz Variability of the wind in XY. RMS of a 1 Hz lowpassed wind estimate. float
// var_vert Variability of the wind in Z. RMS of a 1 Hz lowpassed wind estimate. float
// wind_alt Altitude (MSL) that this measurement was taken at float
// horiz_accuracy Horizontal speed 1-STD accuracy float
// vert_accuracy Vertical speed 1-STD accuracy float
export class WindCov extends MAVLinkMessage {
	public time_usec!: number;
	public wind_x!: number;
	public wind_y!: number;
	public wind_z!: number;
	public var_horiz!: number;
	public var_vert!: number;
	public wind_alt!: number;
	public horiz_accuracy!: number;
	public vert_accuracy!: number;
	public _message_id: number = 231;
	public _message_name: string = 'WIND_COV';
	public _crc_extra: number = 105;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['wind_x', 'float', false],
		['wind_y', 'float', false],
		['wind_z', 'float', false],
		['var_horiz', 'float', false],
		['var_vert', 'float', false],
		['wind_alt', 'float', false],
		['horiz_accuracy', 'float', false],
		['vert_accuracy', 'float', false],
	];
}