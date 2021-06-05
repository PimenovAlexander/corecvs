import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavOdidOperatorLocationType} from '../enums/mav-odid-operator-location-type';
import {MavOdidClassificationType} from '../enums/mav-odid-classification-type';
import {MavOdidCategoryEu} from '../enums/mav-odid-category-eu';
import {MavOdidClassEu} from '../enums/mav-odid-class-eu';
/*
Data for filling the OpenDroneID System message. The System Message contains general system information including the operator location and possible aircraft group information.
*/
// target_system System ID (0 for broadcast). uint8_t
// target_component Component ID (0 for broadcast). uint8_t
// id_or_mac Only used for drone ID data received from other UAs. See detailed description at https://mavlink.io/en/services/opendroneid.html. uint8_t
// operator_location_type Specifies the operator location type. uint8_t
// classification_type Specifies the classification type of the UA. uint8_t
// operator_latitude Latitude of the operator. If unknown: 0 (both Lat/Lon). int32_t
// operator_longitude Longitude of the operator. If unknown: 0 (both Lat/Lon). int32_t
// area_count Number of aircraft in the area, group or formation (default 1). uint16_t
// area_radius Radius of the cylindrical area of the group or formation (default 0). uint16_t
// area_ceiling Area Operations Ceiling relative to WGS84. If unknown: -1000 m. float
// area_floor Area Operations Floor relative to WGS84. If unknown: -1000 m. float
// category_eu When classification_type is MAV_ODID_CLASSIFICATION_TYPE_EU, specifies the category of the UA. uint8_t
// class_eu When classification_type is MAV_ODID_CLASSIFICATION_TYPE_EU, specifies the class of the UA. uint8_t
export class OpenDroneIdSystem extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public id_or_mac!: number;
	public operator_location_type!: MavOdidOperatorLocationType;
	public classification_type!: MavOdidClassificationType;
	public operator_latitude!: number;
	public operator_longitude!: number;
	public area_count!: number;
	public area_radius!: number;
	public area_ceiling!: number;
	public area_floor!: number;
	public category_eu!: MavOdidCategoryEu;
	public class_eu!: MavOdidClassEu;
	public _message_id: number = 12904;
	public _message_name: string = 'OPEN_DRONE_ID_SYSTEM';
	public _crc_extra: number = 203;
	public _message_fields: [string, string, boolean][] = [
		['operator_latitude', 'int32_t', false],
		['operator_longitude', 'int32_t', false],
		['area_ceiling', 'float', false],
		['area_floor', 'float', false],
		['area_count', 'uint16_t', false],
		['area_radius', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['id_or_mac', 'uint8_t', false],
		['operator_location_type', 'uint8_t', false],
		['classification_type', 'uint8_t', false],
		['category_eu', 'uint8_t', false],
		['class_eu', 'uint8_t', false],
	];
}