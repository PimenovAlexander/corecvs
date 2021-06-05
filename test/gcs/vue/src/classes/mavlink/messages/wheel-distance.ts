import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Cumulative distance traveled for each reported wheel.
*/
// time_usec Timestamp (synced to UNIX time or since system boot). uint64_t
// count Number of wheels reported. uint8_t
// distance Distance reported by individual wheel encoders. Forward rotations increase values, reverse rotations decrease them. Not all wheels will necessarily have wheel encoders; the mapping of encoders to wheel positions must be agreed/understood by the endpoints. double
export class WheelDistance extends MAVLinkMessage {
	public time_usec!: number;
	public count!: number;
	public distance!: number;
	public _message_id: number = 9000;
	public _message_name: string = 'WHEEL_DISTANCE';
	public _crc_extra: number = 113;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['distance', 'double', false],
		['count', 'uint8_t', false],
	];
}