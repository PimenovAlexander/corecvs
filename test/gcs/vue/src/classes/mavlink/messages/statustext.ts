import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavSeverity} from '../enums/mav-severity';
/*
Status text message. These messages are printed in yellow in the COMM console of QGroundControl. WARNING: They consume quite some bandwidth, so use only for important status and error messages. If implemented wisely, these messages are buffered on the MCU and sent only at a limited rate (e.g. 10 Hz).
*/
// severity Severity of status. Relies on the definitions within RFC-5424. uint8_t
// text Status text message, without null termination character char
// id Unique (opaque) identifier for this statustext message.  May be used to reassemble a logical long-statustext message from a sequence of chunks.  A value of zero indicates this is the only chunk in the sequence and the message can be emitted immediately. uint16_t
// chunk_seq This chunk's sequence number; indexing is from zero.  Any null character in the text field is taken to mean this was the last chunk. uint8_t
export class Statustext extends MAVLinkMessage {
	public severity!: MavSeverity;
	public text!: string;
	public id!: number;
	public chunk_seq!: number;
	public _message_id: number = 253;
	public _message_name: string = 'STATUSTEXT';
	public _crc_extra: number = 83;
	public _message_fields: [string, string, boolean][] = [
		['severity', 'uint8_t', false],
		['text', 'char', false],
		['id', 'uint16_t', true],
		['chunk_seq', 'uint8_t', true],
	];
}