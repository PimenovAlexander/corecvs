import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavOdidOperatorIdType} from '../enums/mav-odid-operator-id-type';
/*
Data for filling the OpenDroneID Operator ID message, which contains the CAA (Civil Aviation Authority) issued operator ID.
*/
// target_system System ID (0 for broadcast). uint8_t
// target_component Component ID (0 for broadcast). uint8_t
// id_or_mac Only used for drone ID data received from other UAs. See detailed description at https://mavlink.io/en/services/opendroneid.html. uint8_t
// operator_id_type Indicates the type of the operator_id field. uint8_t
// operator_id Text description or numeric value expressed as ASCII characters. Shall be filled with nulls in the unused portion of the field. char
export class OpenDroneIdOperatorId extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public id_or_mac!: number;
	public operator_id_type!: MavOdidOperatorIdType;
	public operator_id!: string;
	public _message_id: number = 12905;
	public _message_name: string = 'OPEN_DRONE_ID_OPERATOR_ID';
	public _crc_extra: number = 49;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['id_or_mac', 'uint8_t', false],
		['operator_id_type', 'uint8_t', false],
		['operator_id', 'char', false],
	];
}