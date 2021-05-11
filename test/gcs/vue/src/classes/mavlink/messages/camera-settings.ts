import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CameraMode} from '../enums/camera-mode';
/*
Settings of a camera. Can be requested with a MAV_CMD_REQUEST_MESSAGE command.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// mode_id Camera mode uint8_t
// zoomLevel Current zoom level (0.0 to 100.0, NaN if not known) float
// focusLevel Current focus level (0.0 to 100.0, NaN if not known) float
export class CameraSettings extends MAVLinkMessage {
	public time_boot_ms!: number;
	public mode_id!: CameraMode;
	public zoomLevel!: number;
	public focusLevel!: number;
	public _message_id: number = 260;
	public _message_name: string = 'CAMERA_SETTINGS';
	public _crc_extra: number = 146;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['mode_id', 'uint8_t', false],
		['zoomLevel', 'float', true],
		['focusLevel', 'float', true],
	];
}