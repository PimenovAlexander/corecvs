import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The autopilot is requesting a resource (file, binary, other type of data)
*/
// request_id Request ID. This ID should be re-used when sending back URI contents uint8_t
// uri_type The type of requested URI. 0 = a file via URL. 1 = a UAVCAN binary uint8_t
// uri The requested unique resource identifier (URI). It is not necessarily a straight domain name (depends on the URI type enum) uint8_t
// transfer_type The way the autopilot wants to receive the URI. 0 = MAVLink FTP. 1 = binary stream. uint8_t
// storage The storage path the autopilot wants the URI to be stored in. Will only be valid if the transfer_type has a storage associated (e.g. MAVLink FTP). uint8_t
export class ResourceRequest extends MAVLinkMessage {
	public request_id!: number;
	public uri_type!: number;
	public uri!: number;
	public transfer_type!: number;
	public storage!: number;
	public _message_id: number = 142;
	public _message_name: string = 'RESOURCE_REQUEST';
	public _crc_extra: number = 72;
	public _message_fields: [string, string, boolean][] = [
		['request_id', 'uint8_t', false],
		['uri_type', 'uint8_t', false],
		['uri', 'uint8_t', false],
		['transfer_type', 'uint8_t', false],
		['storage', 'uint8_t', false],
	];
}