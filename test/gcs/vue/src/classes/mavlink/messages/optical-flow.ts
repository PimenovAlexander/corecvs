import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Optical flow from a flow sensor (e.g. optical mouse sensor)
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// sensor_id Sensor ID uint8_t
// flow_x Flow in x-sensor direction int16_t
// flow_y Flow in y-sensor direction int16_t
// flow_comp_m_x Flow in x-sensor direction, angular-speed compensated float
// flow_comp_m_y Flow in y-sensor direction, angular-speed compensated float
// quality Optical flow quality / confidence. 0: bad, 255: maximum quality uint8_t
// ground_distance Ground distance. Positive value: distance known. Negative value: Unknown distance float
// flow_rate_x Flow rate about X axis float
// flow_rate_y Flow rate about Y axis float
export class OpticalFlow extends MAVLinkMessage {
	public time_usec!: number;
	public sensor_id!: number;
	public flow_x!: number;
	public flow_y!: number;
	public flow_comp_m_x!: number;
	public flow_comp_m_y!: number;
	public quality!: number;
	public ground_distance!: number;
	public flow_rate_x!: number;
	public flow_rate_y!: number;
	public _message_id: number = 100;
	public _message_name: string = 'OPTICAL_FLOW';
	public _crc_extra: number = 175;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['flow_comp_m_x', 'float', false],
		['flow_comp_m_y', 'float', false],
		['ground_distance', 'float', false],
		['flow_x', 'int16_t', false],
		['flow_y', 'int16_t', false],
		['sensor_id', 'uint8_t', false],
		['quality', 'uint8_t', false],
		['flow_rate_x', 'float', true],
		['flow_rate_y', 'float', true],
	];
}