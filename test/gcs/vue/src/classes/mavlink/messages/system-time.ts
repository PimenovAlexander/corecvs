import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The system time is the time of the master clock, typically the computer clock of the main onboard computer.
*/
// time_unix_usec Timestamp (UNIX epoch time). uint64_t
// time_boot_ms Timestamp (time since system boot). uint32_t
export class SystemTime extends MAVLinkMessage {
	public time_unix_usec!: number;
	public time_boot_ms!: number;
	public _message_id: number = 2;
	public _message_name: string = 'SYSTEM_TIME';
	public _crc_extra: number = 137;
	public _message_fields: [string, string, boolean][] = [
		['time_unix_usec', 'uint64_t', false],
		['time_boot_ms', 'uint32_t', false],
	];
}