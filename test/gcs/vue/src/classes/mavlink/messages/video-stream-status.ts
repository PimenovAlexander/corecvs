import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {VideoStreamStatusFlags} from '../enums/video-stream-status-flags';
/*
Information about the status of a video stream. It may be requested using MAV_CMD_REQUEST_MESSAGE.
*/
// stream_id Video Stream ID (1 for first, 2 for second, etc.) uint8_t
// flags Bitmap of stream status flags uint16_t
// framerate Frame rate float
// resolution_h Horizontal resolution uint16_t
// resolution_v Vertical resolution uint16_t
// bitrate Bit rate uint32_t
// rotation Video image rotation clockwise uint16_t
// hfov Horizontal Field of view uint16_t
export class VideoStreamStatus extends MAVLinkMessage {
	public stream_id!: number;
	public flags!: VideoStreamStatusFlags;
	public framerate!: number;
	public resolution_h!: number;
	public resolution_v!: number;
	public bitrate!: number;
	public rotation!: number;
	public hfov!: number;
	public _message_id: number = 270;
	public _message_name: string = 'VIDEO_STREAM_STATUS';
	public _crc_extra: number = 59;
	public _message_fields: [string, string, boolean][] = [
		['framerate', 'float', false],
		['bitrate', 'uint32_t', false],
		['flags', 'uint16_t', false],
		['resolution_h', 'uint16_t', false],
		['resolution_v', 'uint16_t', false],
		['rotation', 'uint16_t', false],
		['hfov', 'uint16_t', false],
		['stream_id', 'uint8_t', false],
	];
}