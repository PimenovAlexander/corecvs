import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Emit an encrypted signature / key identifying this system. PLEASE NOTE: This protocol has been kept simple, so transmitting the key requires an encrypted channel for true safety.
*/
// key key char
export class AuthKey extends MAVLinkMessage {
	public key!: string;
	public _message_id: number = 7;
	public _message_name: string = 'AUTH_KEY';
	public _crc_extra: number = 119;
	public _message_fields: [string, string, boolean][] = [
		['key', 'char', false],
	];
}