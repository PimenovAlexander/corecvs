import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Request a data stream.
*/
// target_system The target requested to send the message stream. uint8_t
// target_component The target requested to send the message stream. uint8_t
// req_stream_id The ID of the requested data stream uint8_t
// req_message_rate The requested message rate uint16_t
// start_stop 1 to start sending, 0 to stop sending. uint8_t
export class RequestDataStream extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public req_stream_id!: number;
	public req_message_rate!: number;
	public start_stop!: number;
	public _message_id: number = 66;
	public _message_name: string = 'REQUEST_DATA_STREAM';
	public _crc_extra: number = 148;
	public _message_fields: [string, string, boolean][] = [
		['req_message_rate', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['req_stream_id', 'uint8_t', false],
		['start_stop', 'uint8_t', false],
	];
}