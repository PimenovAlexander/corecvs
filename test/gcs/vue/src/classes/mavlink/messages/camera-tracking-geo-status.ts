import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CameraTrackingStatusFlags} from '../enums/camera-tracking-status-flags';
/*
Camera tracking status, sent while in active tracking. Use MAV_CMD_SET_MESSAGE_INTERVAL to define message interval.
*/
// tracking_status Current tracking status uint8_t
// lat Latitude of tracked object int32_t
// lon Longitude of tracked object int32_t
// alt Altitude of tracked object(AMSL, WGS84) float
// h_acc Horizontal accuracy. NAN if unknown float
// v_acc Vertical accuracy. NAN if unknown float
// vel_n North velocity of tracked object. NAN if unknown float
// vel_e East velocity of tracked object. NAN if unknown float
// vel_d Down velocity of tracked object. NAN if unknown float
// vel_acc Velocity accuracy. NAN if unknown float
// dist Distance between camera and tracked object. NAN if unknown float
// hdg Heading in radians, in NED. NAN if unknown float
// hdg_acc Accuracy of heading, in NED. NAN if unknown float
export class CameraTrackingGeoStatus extends MAVLinkMessage {
	public tracking_status!: CameraTrackingStatusFlags;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public h_acc!: number;
	public v_acc!: number;
	public vel_n!: number;
	public vel_e!: number;
	public vel_d!: number;
	public vel_acc!: number;
	public dist!: number;
	public hdg!: number;
	public hdg_acc!: number;
	public _message_id: number = 276;
	public _message_name: string = 'CAMERA_TRACKING_GEO_STATUS';
	public _crc_extra: number = 18;
	public _message_fields: [string, string, boolean][] = [
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'float', false],
		['h_acc', 'float', false],
		['v_acc', 'float', false],
		['vel_n', 'float', false],
		['vel_e', 'float', false],
		['vel_d', 'float', false],
		['vel_acc', 'float', false],
		['dist', 'float', false],
		['hdg', 'float', false],
		['hdg_acc', 'float', false],
		['tracking_status', 'uint8_t', false],
	];
}