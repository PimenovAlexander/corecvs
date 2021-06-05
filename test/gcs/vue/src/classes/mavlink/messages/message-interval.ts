import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The interval between messages for a particular MAVLink message ID. This message is the response to the MAV_CMD_GET_MESSAGE_INTERVAL command. This interface replaces DATA_STREAM.
*/
// message_id The ID of the requested MAVLink message. v1.0 is limited to 254 messages. uint16_t
// interval_us The interval between two messages. A value of -1 indicates this stream is disabled, 0 indicates it is not available, > 0 indicates the interval at which it is sent. int32_t
export class MessageInterval extends MAVLinkMessage {
	public message_id!: number;
	public interval_us!: number;
	public _message_id: number = 244;
	public _message_name: string = 'MESSAGE_INTERVAL';
	public _crc_extra: number = 95;
	public _message_fields: [string, string, boolean][] = [
		['interval_us', 'int32_t', false],
		['message_id', 'uint16_t', false],
	];
}