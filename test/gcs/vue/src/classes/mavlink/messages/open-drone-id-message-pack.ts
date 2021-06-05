import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
An OpenDroneID message pack is a container for multiple encoded OpenDroneID messages (i.e. not in the format given for the above messages descriptions but after encoding into the compressed OpenDroneID byte format). Used e.g. when transmitting on Bluetooth 5.0 Long Range/Extended Advertising or on WiFi Neighbor Aware Networking.
*/
// target_system System ID (0 for broadcast). uint8_t
// target_component Component ID (0 for broadcast). uint8_t
// single_message_size This field must currently always be equal to 25 (bytes), since all encoded OpenDroneID messages are specificed to have this length. uint8_t
// msg_pack_size Number of encoded messages in the pack (not the number of bytes). Allowed range is 1 - 10. uint8_t
// messages Concatenation of encoded OpenDroneID messages. Shall be filled with nulls in the unused portion of the field. uint8_t
export class OpenDroneIdMessagePack extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public single_message_size!: number;
	public msg_pack_size!: number;
	public messages!: number;
	public _message_id: number = 12915;
	public _message_name: string = 'OPEN_DRONE_ID_MESSAGE_PACK';
	public _crc_extra: number = 62;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['single_message_size', 'uint8_t', false],
		['msg_pack_size', 'uint8_t', false],
		['messages', 'uint8_t', false],
	];
}