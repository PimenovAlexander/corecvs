import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Information about a captured image. This is emitted every time a message is captured. It may be re-requested using MAV_CMD_REQUEST_MESSAGE, using param2 to indicate the sequence number for the missing image.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// time_utc Timestamp (time since UNIX epoch) in UTC. 0 for unknown. uint64_t
// camera_id Deprecated/unused. Component IDs are used to differentiate multiple cameras. uint8_t
// lat Latitude where image was taken int32_t
// lon Longitude where capture was taken int32_t
// alt Altitude (MSL) where image was taken int32_t
// relative_alt Altitude above ground int32_t
// q Quaternion of camera orientation (w, x, y, z order, zero-rotation is 1, 0, 0, 0) float
// image_index Zero based index of this image (i.e. a new image will have index CAMERA_CAPTURE_STATUS.image count -1) int32_t
// capture_result Boolean indicating success (1) or failure (0) while capturing this image. int8_t
// file_url URL of image taken. Either local storage or http://foo.jpg if camera provides an HTTP interface. char
export class CameraImageCaptured extends MAVLinkMessage {
	public time_boot_ms!: number;
	public time_utc!: number;
	public camera_id!: number;
	public lat!: number;
	public lon!: number;
	public alt!: number;
	public relative_alt!: number;
	public q!: number;
	public image_index!: number;
	public capture_result!: number;
	public file_url!: string;
	public _message_id: number = 263;
	public _message_name: string = 'CAMERA_IMAGE_CAPTURED';
	public _crc_extra: number = 133;
	public _message_fields: [string, string, boolean][] = [
		['time_utc', 'uint64_t', false],
		['time_boot_ms', 'uint32_t', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['alt', 'int32_t', false],
		['relative_alt', 'int32_t', false],
		['q', 'float', false],
		['image_index', 'int32_t', false],
		['camera_id', 'uint8_t', false],
		['capture_result', 'int8_t', false],
		['file_url', 'char', false],
	];
}