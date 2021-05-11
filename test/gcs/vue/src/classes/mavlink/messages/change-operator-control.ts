import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Request to control this MAV
*/
// target_system System the GCS requests control for uint8_t
// control_request 0: request control of this MAV, 1: Release control of this MAV uint8_t
// version 0: key as plaintext, 1-255: future, different hashing/encryption variants. The GCS should in general use the safest mode possible initially and then gradually move down the encryption level if it gets a NACK message indicating an encryption mismatch. uint8_t
// passkey Password / Key, depending on version plaintext or encrypted. 25 or less characters, NULL terminated. The characters may involve A-Z, a-z, 0-9, and "!?,.-" char
export class ChangeOperatorControl extends MAVLinkMessage {
	public target_system!: number;
	public control_request!: number;
	public version!: number;
	public passkey!: string;
	public _message_id: number = 5;
	public _message_name: string = 'CHANGE_OPERATOR_CONTROL';
	public _crc_extra: number = 217;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['control_request', 'uint8_t', false],
		['version', 'uint8_t', false],
		['passkey', 'char', false],
	];
}