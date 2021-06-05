import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Status of the Iridium SBD link.
*/
// timestamp Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// last_heartbeat Timestamp of the last successful sbd session. The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// failed_sessions Number of failed SBD sessions. uint16_t
// successful_sessions Number of successful SBD sessions. uint16_t
// signal_quality Signal quality equal to the number of bars displayed on the ISU signal strength indicator. Range is 0 to 5, where 0 indicates no signal and 5 indicates maximum signal strength. uint8_t
// ring_pending 1: Ring call pending, 0: No call pending. uint8_t
// tx_session_pending 1: Transmission session pending, 0: No transmission session pending. uint8_t
// rx_session_pending 1: Receiving session pending, 0: No receiving session pending. uint8_t
export class IsbdLinkStatus extends MAVLinkMessage {
	public timestamp!: number;
	public last_heartbeat!: number;
	public failed_sessions!: number;
	public successful_sessions!: number;
	public signal_quality!: number;
	public ring_pending!: number;
	public tx_session_pending!: number;
	public rx_session_pending!: number;
	public _message_id: number = 335;
	public _message_name: string = 'ISBD_LINK_STATUS';
	public _crc_extra: number = 225;
	public _message_fields: [string, string, boolean][] = [
		['timestamp', 'uint64_t', false],
		['last_heartbeat', 'uint64_t', false],
		['failed_sessions', 'uint16_t', false],
		['successful_sessions', 'uint16_t', false],
		['signal_quality', 'uint8_t', false],
		['ring_pending', 'uint8_t', false],
		['tx_session_pending', 'uint8_t', false],
		['rx_session_pending', 'uint8_t', false],
	];
}