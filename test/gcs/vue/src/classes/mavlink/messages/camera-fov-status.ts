import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Information about the field of view of a camera. Can be requested with a MAV_CMD_REQUEST_MESSAGE command.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// lat_camera Latitude of camera (INT32_MAX if unknown). int32_t
// lon_camera Longitude of camera (INT32_MAX if unknown). int32_t
// alt_camera Altitude (MSL) of camera (INT32_MAX if unknown). int32_t
// lat_image Latitude of center of image (INT32_MAX if unknown, INT32_MIN if at infinity, not intersecting with horizon). int32_t
// lon_image Longitude of center of image (INT32_MAX if unknown, INT32_MIN if at infinity, not intersecting with horizon). int32_t
// alt_image Altitude (MSL) of center of image (INT32_MAX if unknown, INT32_MIN if at infinity, not intersecting with horizon). int32_t
// q Quaternion of camera orientation (w, x, y, z order, zero-rotation is 1, 0, 0, 0) float
// hfov Horizontal field of view (NaN if unknown). float
// vfov Vertical field of view (NaN if unknown). float
export class CameraFovStatus extends MAVLinkMessage {
	public time_boot_ms!: number;
	public lat_camera!: number;
	public lon_camera!: number;
	public alt_camera!: number;
	public lat_image!: number;
	public lon_image!: number;
	public alt_image!: number;
	public q!: number;
	public hfov!: number;
	public vfov!: number;
	public _message_id: number = 271;
	public _message_name: string = 'CAMERA_FOV_STATUS';
	public _crc_extra: number = 22;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['lat_camera', 'int32_t', false],
		['lon_camera', 'int32_t', false],
		['alt_camera', 'int32_t', false],
		['lat_image', 'int32_t', false],
		['lon_image', 'int32_t', false],
		['alt_image', 'int32_t', false],
		['q', 'float', false],
		['hfov', 'float', false],
		['vfov', 'float', false],
	];
}