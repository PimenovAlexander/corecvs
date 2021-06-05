import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Status generated in each node in the communication chain and injected into MAVLink stream.
*/
// timestamp Timestamp (time since system boot). uint64_t
// tx_buf Remaining free transmit buffer space uint8_t
// rx_buf Remaining free receive buffer space uint8_t
// tx_rate Transmit rate uint32_t
// rx_rate Receive rate uint32_t
// rx_parse_err Number of bytes that could not be parsed correctly. uint16_t
// tx_overflows Transmit buffer overflows. This number wraps around as it reaches UINT16_MAX uint16_t
// rx_overflows Receive buffer overflows. This number wraps around as it reaches UINT16_MAX uint16_t
// messages_sent Messages sent uint32_t
// messages_received Messages received (estimated from counting seq) uint32_t
// messages_lost Messages lost (estimated from counting seq) uint32_t
export class LinkNodeStatus extends MAVLinkMessage {
	public timestamp!: number;
	public tx_buf!: number;
	public rx_buf!: number;
	public tx_rate!: number;
	public rx_rate!: number;
	public rx_parse_err!: number;
	public tx_overflows!: number;
	public rx_overflows!: number;
	public messages_sent!: number;
	public messages_received!: number;
	public messages_lost!: number;
	public _message_id: number = 8;
	public _message_name: string = 'LINK_NODE_STATUS';
	public _crc_extra: number = 117;
	public _message_fields: [string, string, boolean][] = [
		['timestamp', 'uint64_t', false],
		['tx_rate', 'uint32_t', false],
		['rx_rate', 'uint32_t', false],
		['messages_sent', 'uint32_t', false],
		['messages_received', 'uint32_t', false],
		['messages_lost', 'uint32_t', false],
		['rx_parse_err', 'uint16_t', false],
		['tx_overflows', 'uint16_t', false],
		['rx_overflows', 'uint16_t', false],
		['tx_buf', 'uint8_t', false],
		['rx_buf', 'uint8_t', false],
	];
}