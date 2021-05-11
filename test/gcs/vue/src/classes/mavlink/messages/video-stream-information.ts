import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {VideoStreamType} from '../enums/video-stream-type';
import {VideoStreamStatusFlags} from '../enums/video-stream-status-flags';
/*
Information about video stream. It may be requested using MAV_CMD_REQUEST_MESSAGE, where param2 indicates the video stream id: 0 for all streams, 1 for first, 2 for second, etc.
*/
// stream_id Video Stream ID (1 for first, 2 for second, etc.) uint8_t
// count Number of streams available. uint8_t
// type Type of stream. uint8_t
// flags Bitmap of stream status flags. uint16_t
// framerate Frame rate. float
// resolution_h Horizontal resolution. uint16_t
// resolution_v Vertical resolution. uint16_t
// bitrate Bit rate. uint32_t
// rotation Video image rotation clockwise. uint16_t
// hfov Horizontal Field of view. uint16_t
// name Stream name. char
// uri Video stream URI (TCP or RTSP URI ground station should connect to) or port number (UDP port ground station should listen to). char
export class VideoStreamInformation extends MAVLinkMessage {
	public stream_id!: number;
	public count!: number;
	public type!: VideoStreamType;
	public flags!: VideoStreamStatusFlags;
	public framerate!: number;
	public resolution_h!: number;
	public resolution_v!: number;
	public bitrate!: number;
	public rotation!: number;
	public hfov!: number;
	public name!: string;
	public uri!: string;
	public _message_id: number = 269;
	public _message_name: string = 'VIDEO_STREAM_INFORMATION';
	public _crc_extra: number = 109;
	public _message_fields: [string, string, boolean][] = [
		['framerate', 'float', false],
		['bitrate', 'uint32_t', false],
		['flags', 'uint16_t', false],
		['resolution_h', 'uint16_t', false],
		['resolution_v', 'uint16_t', false],
		['rotation', 'uint16_t', false],
		['hfov', 'uint16_t', false],
		['stream_id', 'uint8_t', false],
		['count', 'uint8_t', false],
		['type', 'uint8_t', false],
		['name', 'char', false],
		['uri', 'char', false],
	];
}