import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Accept / deny control of this MAV
*/
// gcs_system_id ID of the GCS this message uint8_t
// control_request 0: request control of this MAV, 1: Release control of this MAV uint8_t
// ack 0: ACK, 1: NACK: Wrong passkey, 2: NACK: Unsupported passkey encryption method, 3: NACK: Already under control uint8_t
export class ChangeOperatorControlAck extends MAVLinkMessage {
	public gcs_system_id!: number;
	public control_request!: number;
	public ack!: number;
	public _message_id: number = 6;
	public _message_name: string = 'CHANGE_OPERATOR_CONTROL_ACK';
	public _crc_extra: number = 104;
	public _message_fields: [string, string, boolean][] = [
		['gcs_system_id', 'uint8_t', false],
		['control_request', 'uint8_t', false],
		['ack', 'uint8_t', false],
	];
}