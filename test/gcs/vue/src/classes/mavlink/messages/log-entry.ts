import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Reply to LOG_REQUEST_LIST
*/
// id Log id uint16_t
// num_logs Total number of logs uint16_t
// last_log_num High log number uint16_t
// time_utc UTC timestamp of log since 1970, or 0 if not available uint32_t
// size Size of the log (may be approximate) uint32_t
export class LogEntry extends MAVLinkMessage {
	public id!: number;
	public num_logs!: number;
	public last_log_num!: number;
	public time_utc!: number;
	public size!: number;
	public _message_id: number = 118;
	public _message_name: string = 'LOG_ENTRY';
	public _crc_extra: number = 56;
	public _message_fields: [string, string, boolean][] = [
		['time_utc', 'uint32_t', false],
		['size', 'uint32_t', false],
		['id', 'uint16_t', false],
		['num_logs', 'uint16_t', false],
		['last_log_num', 'uint16_t', false],
	];
}