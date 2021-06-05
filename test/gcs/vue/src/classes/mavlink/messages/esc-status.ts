import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
ESC information for higher rate streaming. Recommended streaming rate is ~10 Hz. Information that changes more slowly is sent in ESC_INFO. It should typically only be streamed on high-bandwidth links (i.e. to a companion computer).
*/
// index Index of the first ESC in this message. minValue = 0, maxValue = 60, increment = 4. uint8_t
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number. uint64_t
// rpm Reported motor RPM from each ESC (negative for reverse rotation). int32_t
// voltage Voltage measured from each ESC. float
// current Current measured from each ESC. float
export class EscStatus extends MAVLinkMessage {
	public index!: number;
	public time_usec!: number;
	public rpm!: number;
	public voltage!: number;
	public current!: number;
	public _message_id: number = 291;
	public _message_name: string = 'ESC_STATUS';
	public _crc_extra: number = 10;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['rpm', 'int32_t', false],
		['voltage', 'float', false],
		['current', 'float', false],
		['index', 'uint8_t', false],
	];
}