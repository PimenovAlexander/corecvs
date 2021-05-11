import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavDistanceSensor} from '../enums/mav-distance-sensor';
import {MavFrame} from '../enums/mav-frame';
/*
Obstacle distances in front of the sensor, starting from the left in increment degrees to the right
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// sensor_type Class id of the distance sensor type. uint8_t
// distances Distance of obstacles around the vehicle with index 0 corresponding to north + angle_offset, unless otherwise specified in the frame. A value of 0 is valid and means that the obstacle is practically touching the sensor. A value of max_distance +1 means no obstacle is present. A value of UINT16_MAX for unknown/not used. In a array element, one unit corresponds to 1cm. uint16_t
// increment Angular width in degrees of each array element. Increment direction is clockwise. This field is ignored if increment_f is non-zero. uint8_t
// min_distance Minimum distance the sensor can measure. uint16_t
// max_distance Maximum distance the sensor can measure. uint16_t
// increment_f Angular width in degrees of each array element as a float. If non-zero then this value is used instead of the uint8_t increment field. Positive is clockwise direction, negative is counter-clockwise. float
// angle_offset Relative angle offset of the 0-index element in the distances array. Value of 0 corresponds to forward. Positive is clockwise direction, negative is counter-clockwise. float
// frame Coordinate frame of reference for the yaw rotation and offset of the sensor data. Defaults to MAV_FRAME_GLOBAL, which is north aligned. For body-mounted sensors use MAV_FRAME_BODY_FRD, which is vehicle front aligned. uint8_t
export class ObstacleDistance extends MAVLinkMessage {
	public time_usec!: number;
	public sensor_type!: MavDistanceSensor;
	public distances!: number;
	public increment!: number;
	public min_distance!: number;
	public max_distance!: number;
	public increment_f!: number;
	public angle_offset!: number;
	public frame!: MavFrame;
	public _message_id: number = 330;
	public _message_name: string = 'OBSTACLE_DISTANCE';
	public _crc_extra: number = 23;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['distances', 'uint16_t', false],
		['min_distance', 'uint16_t', false],
		['max_distance', 'uint16_t', false],
		['sensor_type', 'uint8_t', false],
		['increment', 'uint8_t', false],
		['increment_f', 'float', true],
		['angle_offset', 'float', true],
		['frame', 'uint8_t', true],
	];
}