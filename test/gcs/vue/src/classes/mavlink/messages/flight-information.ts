import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Information about flight since last arming.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// arming_time_utc Timestamp at arming (time since UNIX epoch) in UTC, 0 for unknown uint64_t
// takeoff_time_utc Timestamp at takeoff (time since UNIX epoch) in UTC, 0 for unknown uint64_t
// flight_uuid Universally unique identifier (UUID) of flight, should correspond to name of log files uint64_t
export class FlightInformation extends MAVLinkMessage {
	public time_boot_ms!: number;
	public arming_time_utc!: number;
	public takeoff_time_utc!: number;
	public flight_uuid!: number;
	public _message_id: number = 264;
	public _message_name: string = 'FLIGHT_INFORMATION';
	public _crc_extra: number = 49;
	public _message_fields: [string, string, boolean][] = [
		['arming_time_utc', 'uint64_t', false],
		['takeoff_time_utc', 'uint64_t', false],
		['flight_uuid', 'uint64_t', false],
		['time_boot_ms', 'uint32_t', false],
	];
}