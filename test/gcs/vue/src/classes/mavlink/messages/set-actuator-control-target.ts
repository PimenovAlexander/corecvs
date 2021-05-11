import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Set the vehicle attitude and body angular rates.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// group_mlx Actuator group. The "_mlx" indicates this is a multi-instance message and a MAVLink parser should use this field to difference between instances. uint8_t
// target_system System ID uint8_t
// target_component Component ID uint8_t
// controls Actuator controls. Normed to -1..+1 where 0 is neutral position. Throttle for single rotation direction motors is 0..1, negative range for reverse direction. Standard mapping for attitude controls (group 0): (index 0-7): roll, pitch, yaw, throttle, flaps, spoilers, airbrakes, landing gear. Load a pass-through mixer to repurpose them as generic outputs. float
export class SetActuatorControlTarget extends MAVLinkMessage {
	public time_usec!: number;
	public group_mlx!: number;
	public target_system!: number;
	public target_component!: number;
	public controls!: number;
	public _message_id: number = 139;
	public _message_name: string = 'SET_ACTUATOR_CONTROL_TARGET';
	public _crc_extra: number = 168;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['controls', 'float', false],
		['group_mlx', 'uint8_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}