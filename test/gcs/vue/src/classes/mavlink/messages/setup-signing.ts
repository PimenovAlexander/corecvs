import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Setup a MAVLink2 signing key. If called with secret_key of all zero and zero initial_timestamp will disable signing
*/
// target_system system id of the target uint8_t
// target_component component ID of the target uint8_t
// secret_key signing key uint8_t
// initial_timestamp initial timestamp uint64_t
export class SetupSigning extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public secret_key!: number;
	public initial_timestamp!: number;
	public _message_id: number = 256;
	public _message_name: string = 'SETUP_SIGNING';
	public _crc_extra: number = 71;
	public _message_fields: [string, string, boolean][] = [
		['initial_timestamp', 'uint64_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['secret_key', 'uint8_t', false],
	];
}