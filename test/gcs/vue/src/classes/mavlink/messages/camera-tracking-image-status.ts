import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CameraTrackingStatusFlags} from '../enums/camera-tracking-status-flags';
import {CameraTrackingMode} from '../enums/camera-tracking-mode';
import {CameraTrackingTargetData} from '../enums/camera-tracking-target-data';
/*
Camera tracking status, sent while in active tracking. Use MAV_CMD_SET_MESSAGE_INTERVAL to define message interval.
*/
// tracking_status Current tracking status uint8_t
// tracking_mode Current tracking mode uint8_t
// target_data Defines location of target data uint8_t
// point_x Current tracked point x value if CAMERA_TRACKING_MODE_POINT (normalized 0..1, 0 is left, 1 is right), NAN if unknown float
// point_y Current tracked point y value if CAMERA_TRACKING_MODE_POINT (normalized 0..1, 0 is top, 1 is bottom), NAN if unknown float
// radius Current tracked radius if CAMERA_TRACKING_MODE_POINT (normalized 0..1, 0 is image left, 1 is image right), NAN if unknown float
// rec_top_x Current tracked rectangle top x value if CAMERA_TRACKING_MODE_RECTANGLE (normalized 0..1, 0 is left, 1 is right), NAN if unknown float
// rec_top_y Current tracked rectangle top y value if CAMERA_TRACKING_MODE_RECTANGLE (normalized 0..1, 0 is top, 1 is bottom), NAN if unknown float
// rec_bottom_x Current tracked rectangle bottom x value if CAMERA_TRACKING_MODE_RECTANGLE (normalized 0..1, 0 is left, 1 is right), NAN if unknown float
// rec_bottom_y Current tracked rectangle bottom y value if CAMERA_TRACKING_MODE_RECTANGLE (normalized 0..1, 0 is top, 1 is bottom), NAN if unknown float
export class CameraTrackingImageStatus extends MAVLinkMessage {
	public tracking_status!: CameraTrackingStatusFlags;
	public tracking_mode!: CameraTrackingMode;
	public target_data!: CameraTrackingTargetData;
	public point_x!: number;
	public point_y!: number;
	public radius!: number;
	public rec_top_x!: number;
	public rec_top_y!: number;
	public rec_bottom_x!: number;
	public rec_bottom_y!: number;
	public _message_id: number = 275;
	public _message_name: string = 'CAMERA_TRACKING_IMAGE_STATUS';
	public _crc_extra: number = 126;
	public _message_fields: [string, string, boolean][] = [
		['point_x', 'float', false],
		['point_y', 'float', false],
		['radius', 'float', false],
		['rec_top_x', 'float', false],
		['rec_top_y', 'float', false],
		['rec_bottom_x', 'float', false],
		['rec_bottom_y', 'float', false],
		['tracking_status', 'uint8_t', false],
		['tracking_mode', 'uint8_t', false],
		['target_data', 'uint8_t', false],
	];
}