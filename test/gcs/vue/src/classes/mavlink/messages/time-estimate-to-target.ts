import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Time/duration estimates for various events and actions given the current vehicle state and position.
*/
// safe_return Estimated time to complete the vehicle's configured "safe return" action from its current position (e.g. RTL, Smart RTL, etc.). -1 indicates that the vehicle is landed, or that no time estimate available. int32_t
// land Estimated time for vehicle to complete the LAND action from its current position. -1 indicates that the vehicle is landed, or that no time estimate available. int32_t
// mission_next_item Estimated time for reaching/completing the currently active mission item. -1 means no time estimate available. int32_t
// mission_end Estimated time for completing the current mission. -1 means no mission active and/or no estimate available. int32_t
// commanded_action Estimated time for completing the current commanded action (i.e. Go To, Takeoff, Land, etc.). -1 means no action active and/or no estimate available. int32_t
export class TimeEstimateToTarget extends MAVLinkMessage {
	public safe_return!: number;
	public land!: number;
	public mission_next_item!: number;
	public mission_end!: number;
	public commanded_action!: number;
	public _message_id: number = 380;
	public _message_name: string = 'TIME_ESTIMATE_TO_TARGET';
	public _crc_extra: number = 232;
	public _message_fields: [string, string, boolean][] = [
		['safe_return', 'int32_t', false],
		['land', 'int32_t', false],
		['mission_next_item', 'int32_t', false],
		['mission_end', 'int32_t', false],
		['commanded_action', 'int32_t', false],
	];
}