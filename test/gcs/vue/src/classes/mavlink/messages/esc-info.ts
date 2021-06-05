import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {EscConnectionType} from '../enums/esc-connection-type';
import {EscFailureFlags} from '../enums/esc-failure-flags';
/*
ESC information for lower rate streaming. Recommended streaming rate 1Hz. See ESC_STATUS for higher-rate ESC data.
*/
// index Index of the first ESC in this message. minValue = 0, maxValue = 60, increment = 4. uint8_t
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number. uint64_t
// counter Counter of data packets received. uint16_t
// count Total number of ESCs in all messages of this type. Message fields with an index higher than this should be ignored because they contain invalid data. uint8_t
// connection_type Connection type protocol for all ESC. uint8_t
// info Information regarding online/offline status of each ESC. uint8_t
// failure_flags Bitmap of ESC failure flags. uint16_t
// error_count Number of reported errors by each ESC since boot. uint32_t
// temperature Temperature measured by each ESC. UINT8_MAX if data not supplied by ESC. uint8_t
export class EscInfo extends MAVLinkMessage {
	public index!: number;
	public time_usec!: number;
	public counter!: number;
	public count!: number;
	public connection_type!: EscConnectionType;
	public info!: number;
	public failure_flags!: EscFailureFlags;
	public error_count!: number;
	public temperature!: number;
	public _message_id: number = 290;
	public _message_name: string = 'ESC_INFO';
	public _crc_extra: number = 221;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['error_count', 'uint32_t', false],
		['counter', 'uint16_t', false],
		['failure_flags', 'uint16_t', false],
		['index', 'uint8_t', false],
		['count', 'uint8_t', false],
		['connection_type', 'uint8_t', false],
		['info', 'uint8_t', false],
		['temperature', 'uint8_t', false],
	];
}