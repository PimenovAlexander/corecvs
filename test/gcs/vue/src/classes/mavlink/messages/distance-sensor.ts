import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavDistanceSensor} from '../enums/mav-distance-sensor';
import {MavSensorOrientation} from '../enums/mav-sensor-orientation';
/*
Distance sensor information for an onboard rangefinder.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// min_distance Minimum distance the sensor can measure uint16_t
// max_distance Maximum distance the sensor can measure uint16_t
// current_distance Current distance reading uint16_t
// type Type of distance sensor. uint8_t
// id Onboard ID of the sensor uint8_t
// orientation Direction the sensor faces. downward-facing: ROTATION_PITCH_270, upward-facing: ROTATION_PITCH_90, backward-facing: ROTATION_PITCH_180, forward-facing: ROTATION_NONE, left-facing: ROTATION_YAW_90, right-facing: ROTATION_YAW_270 uint8_t
// covariance Measurement variance. Max standard deviation is 6cm. 255 if unknown. uint8_t
// horizontal_fov Horizontal Field of View (angle) where the distance measurement is valid and the field of view is known. Otherwise this is set to 0. float
// vertical_fov Vertical Field of View (angle) where the distance measurement is valid and the field of view is known. Otherwise this is set to 0. float
// quaternion Quaternion of the sensor orientation in vehicle body frame (w, x, y, z order, zero-rotation is 1, 0, 0, 0). Zero-rotation is along the vehicle body x-axis. This field is required if the orientation is set to MAV_SENSOR_ROTATION_CUSTOM. Set it to 0 if invalid." float
// signal_quality Signal quality of the sensor. Specific to each sensor type, representing the relation of the signal strength with the target reflectivity, distance, size or aspect, but normalised as a percentage. 0 = unknown/unset signal quality, 1 = invalid signal, 100 = perfect signal. uint8_t
export class DistanceSensor extends MAVLinkMessage {
	public time_boot_ms!: number;
	public min_distance!: number;
	public max_distance!: number;
	public current_distance!: number;
	public type!: MavDistanceSensor;
	public id!: number;
	public orientation!: MavSensorOrientation;
	public covariance!: number;
	public horizontal_fov!: number;
	public vertical_fov!: number;
	public quaternion!: number;
	public signal_quality!: number;
	public _message_id: number = 132;
	public _message_name: string = 'DISTANCE_SENSOR';
	public _crc_extra: number = 85;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['min_distance', 'uint16_t', false],
		['max_distance', 'uint16_t', false],
		['current_distance', 'uint16_t', false],
		['type', 'uint8_t', false],
		['id', 'uint8_t', false],
		['orientation', 'uint8_t', false],
		['covariance', 'uint8_t', false],
		['horizontal_fov', 'float', true],
		['vertical_fov', 'float', true],
		['quaternion', 'float', true],
		['signal_quality', 'uint8_t', true],
	];
}