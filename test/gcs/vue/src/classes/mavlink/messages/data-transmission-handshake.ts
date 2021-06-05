import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavlinkDataStreamType} from '../enums/mavlink-data-stream-type';
/*
Handshake message to initiate, control and stop image streaming when using the Image Transmission Protocol: https://mavlink.io/en/services/image_transmission.html.
*/
// type Type of requested/acknowledged data. uint8_t
// size total data size (set on ACK only). uint32_t
// width Width of a matrix or image. uint16_t
// height Height of a matrix or image. uint16_t
// packets Number of packets being sent (set on ACK only). uint16_t
// payload Payload size per packet (normally 253 byte, see DATA field size in message ENCAPSULATED_DATA) (set on ACK only). uint8_t
// jpg_quality JPEG quality. Values: [1-100]. uint8_t
export class DataTransmissionHandshake extends MAVLinkMessage {
	public type!: MavlinkDataStreamType;
	public size!: number;
	public width!: number;
	public height!: number;
	public packets!: number;
	public payload!: number;
	public jpg_quality!: number;
	public _message_id: number = 130;
	public _message_name: string = 'DATA_TRANSMISSION_HANDSHAKE';
	public _crc_extra: number = 29;
	public _message_fields: [string, string, boolean][] = [
		['size', 'uint32_t', false],
		['width', 'uint16_t', false],
		['height', 'uint16_t', false],
		['packets', 'uint16_t', false],
		['type', 'uint8_t', false],
		['payload', 'uint8_t', false],
		['jpg_quality', 'uint8_t', false],
	];
}