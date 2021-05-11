import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavOdidIdType} from '../enums/mav-odid-id-type';
import {MavOdidUaType} from '../enums/mav-odid-ua-type';
/*
Data for filling the OpenDroneID Basic ID message. This and the below messages are primarily meant for feeding data to/from an OpenDroneID implementation. E.g. https://github.com/opendroneid/opendroneid-core-c. These messages are compatible with the ASTM Remote ID standard at https://www.astm.org/Standards/F3411.htm and the ASD-STAN Direct Remote ID standard. The usage of these messages is documented at https://mavlink.io/en/services/opendroneid.html.
*/
// target_system System ID (0 for broadcast). uint8_t
// target_component Component ID (0 for broadcast). uint8_t
// id_or_mac Only used for drone ID data received from other UAs. See detailed description at https://mavlink.io/en/services/opendroneid.html. uint8_t
// id_type Indicates the format for the uas_id field of this message. uint8_t
// ua_type Indicates the type of UA (Unmanned Aircraft). uint8_t
// uas_id UAS (Unmanned Aircraft System) ID following the format specified by id_type. Shall be filled with nulls in the unused portion of the field. uint8_t
export class OpenDroneIdBasicId extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public id_or_mac!: number;
	public id_type!: MavOdidIdType;
	public ua_type!: MavOdidUaType;
	public uas_id!: number;
	public _message_id: number = 12900;
	public _message_name: string = 'OPEN_DRONE_ID_BASIC_ID';
	public _crc_extra: number = 114;
	public _message_fields: [string, string, boolean][] = [
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['id_or_mac', 'uint8_t', false],
		['id_type', 'uint8_t', false],
		['ua_type', 'uint8_t', false],
		['uas_id', 'uint8_t', false],
	];
}