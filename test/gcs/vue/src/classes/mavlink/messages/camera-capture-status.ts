import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Information about the status of a capture. Can be requested with a MAV_CMD_REQUEST_MESSAGE command.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// image_status Current status of image capturing (0: idle, 1: capture in progress, 2: interval set but idle, 3: interval set and capture in progress) uint8_t
// video_status Current status of video capturing (0: idle, 1: capture in progress) uint8_t
// image_interval Image capture interval float
// recording_time_ms Time since recording started uint32_t
// available_capacity Available storage capacity. float
// image_count Total number of images captured ('forever', or until reset using MAV_CMD_STORAGE_FORMAT). int32_t
export class CameraCaptureStatus extends MAVLinkMessage {
	public time_boot_ms!: number;
	public image_status!: number;
	public video_status!: number;
	public image_interval!: number;
	public recording_time_ms!: number;
	public available_capacity!: number;
	public image_count!: number;
	public _message_id: number = 262;
	public _message_name: string = 'CAMERA_CAPTURE_STATUS';
	public _crc_extra: number = 12;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['image_interval', 'float', false],
		['recording_time_ms', 'uint32_t', false],
		['available_capacity', 'float', false],
		['image_status', 'uint8_t', false],
		['video_status', 'uint8_t', false],
		['image_count', 'int32_t', true],
	];
}