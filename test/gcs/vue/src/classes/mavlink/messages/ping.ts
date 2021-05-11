import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
A ping message either requesting or responding to a ping. This allows to measure the system latencies, including serial port, radio modem and UDP connections. The ping microservice is documented at https://mavlink.io/en/services/ping.html
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// seq PING sequence uint32_t
// target_system 0: request ping from all receiving systems. If greater than 0: message is a ping response and number is the system id of the requesting system uint8_t
// target_component 0: request ping from all receiving components. If greater than 0: message is a ping response and number is the component id of the requesting component. uint8_t
export class Ping extends MAVLinkMessage {
	public time_usec!: number;
	public seq!: number;
	public target_system!: number;
	public target_component!: number;
	public _message_id: number = 4;
	public _message_name: string = 'PING';
	public _crc_extra: number = 237;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['seq', 'uint32_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}