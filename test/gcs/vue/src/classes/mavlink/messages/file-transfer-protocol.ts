import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
File transfer message
*/
// target_network Network ID (0 for broadcast) uint8_t
// target_system System ID (0 for broadcast) uint8_t
// target_component Component ID (0 for broadcast) uint8_t
// payload Variable length payload. The length is defined by the remaining message length when subtracting the header and other fields.  The entire content of this block is opaque unless you understand any the encoding message_type.  The particular encoding used can be extension specific and might not always be documented as part of the mavlink specification. uint8_t
export class FileTransferProtocol extends MAVLinkMessage {
	public target_network!: number;
	public target_system!: number;
	public target_component!: number;
	public payload!: number;
	public _message_id: number = 110;
	public _message_name: string = 'FILE_TRANSFER_PROTOCOL';
	public _crc_extra: number = 84;
	public _message_fields: [string, string, boolean][] = [
		['target_network', 'uint8_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['payload', 'uint8_t', false],
	];
}