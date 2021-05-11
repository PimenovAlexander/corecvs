import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Version and capability of protocol version. This message can be requested with MAV_CMD_REQUEST_MESSAGE and is used as part of the handshaking to establish which MAVLink version should be used on the network. Every node should respond to a request for PROTOCOL_VERSION to enable the handshaking. Library implementers should consider adding this into the default decoding state machine to allow the protocol core to respond directly.
*/
// version Currently active MAVLink version number * 100: v1.0 is 100, v2.0 is 200, etc. uint16_t
// min_version Minimum MAVLink version supported uint16_t
// max_version Maximum MAVLink version supported (set to the same value as version by default) uint16_t
// spec_version_hash The first 8 bytes (not characters printed in hex!) of the git hash. uint8_t
// library_version_hash The first 8 bytes (not characters printed in hex!) of the git hash. uint8_t
export class ProtocolVersion extends MAVLinkMessage {
	public version!: number;
	public min_version!: number;
	public max_version!: number;
	public spec_version_hash!: number;
	public library_version_hash!: number;
	public _message_id: number = 300;
	public _message_name: string = 'PROTOCOL_VERSION';
	public _crc_extra: number = 217;
	public _message_fields: [string, string, boolean][] = [
		['version', 'uint16_t', false],
		['min_version', 'uint16_t', false],
		['max_version', 'uint16_t', false],
		['spec_version_hash', 'uint8_t', false],
		['library_version_hash', 'uint8_t', false],
	];
}