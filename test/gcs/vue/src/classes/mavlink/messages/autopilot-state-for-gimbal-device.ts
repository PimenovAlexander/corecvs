import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {EstimatorStatusFlags} from '../enums/estimator-status-flags';
import {MavLandedState} from '../enums/mav-landed-state';
/*
Low level message containing autopilot state relevant for a gimbal device. This message is to be sent from the gimbal manager to the gimbal device component. The data of this message server for the gimbal's estimator corrections in particular horizon compensation, as well as the autopilot's control intention e.g. feed forward angular control in z-axis.
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// time_boot_us Timestamp (time since system boot). uint64_t
// q Quaternion components of autopilot attitude: w, x, y, z (1 0 0 0 is the null-rotation, Hamilton convention). float
// q_estimated_delay_us Estimated delay of the attitude data. uint32_t
// vx X Speed in NED (North, East, Down). float
// vy Y Speed in NED (North, East, Down). float
// vz Z Speed in NED (North, East, Down). float
// v_estimated_delay_us Estimated delay of the speed data. uint32_t
// feed_forward_angular_velocity_z Feed forward Z component of angular velocity, positive is yawing to the right, NaN to be ignored. This is to indicate if the autopilot is actively yawing. float
// estimator_status Bitmap indicating which estimator outputs are valid. uint16_t
// landed_state The landed state. Is set to MAV_LANDED_STATE_UNDEFINED if landed state is unknown. uint8_t
export class AutopilotStateForGimbalDevice extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public time_boot_us!: number;
	public q!: number;
	public q_estimated_delay_us!: number;
	public vx!: number;
	public vy!: number;
	public vz!: number;
	public v_estimated_delay_us!: number;
	public feed_forward_angular_velocity_z!: number;
	public estimator_status!: EstimatorStatusFlags;
	public landed_state!: MavLandedState;
	public _message_id: number = 286;
	public _message_name: string = 'AUTOPILOT_STATE_FOR_GIMBAL_DEVICE';
	public _crc_extra: number = 210;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_us', 'uint64_t', false],
		['q', 'float', false],
		['q_estimated_delay_us', 'uint32_t', false],
		['vx', 'float', false],
		['vy', 'float', false],
		['vz', 'float', false],
		['v_estimated_delay_us', 'uint32_t', false],
		['feed_forward_angular_velocity_z', 'float', false],
		['estimator_status', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['landed_state', 'uint8_t', false],
	];
}