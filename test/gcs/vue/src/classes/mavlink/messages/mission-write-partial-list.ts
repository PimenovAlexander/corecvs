import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavMissionType} from '../enums/mav-mission-type';
/*
This message is sent to the MAV to write a partial list. If start index == end index, only one item will be transmitted / updated. If the start index is NOT 0 and above the current list size, this request should be REJECTED!
*/
// target_system System ID uint8_t
// target_component Component ID uint8_t
// start_index Start index. Must be smaller / equal to the largest index of the current onboard list. int16_t
// end_index End index, equal or greater than start index. int16_t
// mission_type Mission type. uint8_t
export class MissionWritePartialList extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public start_index!: number;
	public end_index!: number;
	public mission_type!: MavMissionType;
	public _message_id: number = 38;
	public _message_name: string = 'MISSION_WRITE_PARTIAL_LIST';
	public _crc_extra: number = 9;
	public _message_fields: [string, string, boolean][] = [
		['start_index', 'int16_t', false],
		['end_index', 'int16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
		['mission_type', 'uint8_t', true],
	];
}