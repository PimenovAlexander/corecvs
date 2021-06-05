import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CameraCapFlags} from '../enums/camera-cap-flags';
/*
Information about a camera. Can be requested with a MAV_CMD_REQUEST_MESSAGE command.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// vendor_name Name of the camera vendor uint8_t
// model_name Name of the camera model uint8_t
// firmware_version Version of the camera firmware, encoded as: (Dev & 0xff) << 24 | (Patch & 0xff) << 16 | (Minor & 0xff) << 8 | (Major & 0xff) uint32_t
// focal_length Focal length float
// sensor_size_h Image sensor size horizontal float
// sensor_size_v Image sensor size vertical float
// resolution_h Horizontal image resolution uint16_t
// resolution_v Vertical image resolution uint16_t
// lens_id Reserved for a lens ID uint8_t
// flags Bitmap of camera capability flags. uint32_t
// cam_definition_version Camera definition version (iteration) uint16_t
// cam_definition_uri Camera definition URI (if any, otherwise only basic functions will be available). HTTP- (http://) and MAVLink FTP- (mavlinkftp://) formatted URIs are allowed (and both must be supported by any GCS that implements the Camera Protocol). char
export class CameraInformation extends MAVLinkMessage {
	public time_boot_ms!: number;
	public vendor_name!: number;
	public model_name!: number;
	public firmware_version!: number;
	public focal_length!: number;
	public sensor_size_h!: number;
	public sensor_size_v!: number;
	public resolution_h!: number;
	public resolution_v!: number;
	public lens_id!: number;
	public flags!: CameraCapFlags;
	public cam_definition_version!: number;
	public cam_definition_uri!: string;
	public _message_id: number = 259;
	public _message_name: string = 'CAMERA_INFORMATION';
	public _crc_extra: number = 92;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['firmware_version', 'uint32_t', false],
		['focal_length', 'float', false],
		['sensor_size_h', 'float', false],
		['sensor_size_v', 'float', false],
		['flags', 'uint32_t', false],
		['resolution_h', 'uint16_t', false],
		['resolution_v', 'uint16_t', false],
		['cam_definition_version', 'uint16_t', false],
		['vendor_name', 'uint8_t', false],
		['model_name', 'uint8_t', false],
		['lens_id', 'uint8_t', false],
		['cam_definition_uri', 'char', false],
	];
}