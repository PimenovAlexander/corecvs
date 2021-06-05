import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavTunnelPayloadType} from '../enums/mav-tunnel-payload-type';
/*
Message for transporting "arbitrary" variable-length data from one component to another (broadcast is not forbidden, but discouraged). The encoding of the data is usually extension specific, i.e. determined by the source, and is usually not documented as part of the MAVLink specification.
*/
// target_system System ID (can be 0 for broadcast, but this is discouraged) uint8_t
// target_component Component ID (can be 0 for broadcast, but this is discouraged) uint8_t
// payload_type A code that identifies the content of the payload (0 for unknown, which is the default). If this code is less than 32768, it is a 'registered' payload type and the corresponding code should be added to the MAV_TUNNEL_PAYLOAD_TYPE enum. Software creators can register blocks of types as needed. Codes greater than 32767 are considered local experiments and should not be checked in to any widely distributed codebase. uint16_t
// payload_length Length of the data transported in payload uint8_t
// payload Variable length payload. The payload length is defined by payload_length. The entire content of this block is opaque unless you understand the encoding specified by payload_type. uint8_t
export class Tunnel extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public payload_type!: MavTunnelPayloadType;
	public payload_length!: number;
	public payload!: number;
	public _message_id: number = 385;
	public _message_name: string = 'TUNNEL';
	public _crc_extra: number = 147;
	public _message_fields: [string, string, boolean][] = [
		['payload_type', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['payload_length', 'uint8_t', false],
		['payload', 'uint8_t', false],
	];
}