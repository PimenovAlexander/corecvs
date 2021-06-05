import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavCmd} from '../enums/mav-cmd';
/*
Cancel a long running command. The target system should respond with a COMMAND_ACK to the original command with result=MAV_RESULT_CANCELLED if the long running process was cancelled. If it has already completed, the cancel action can be ignored. The cancel action can be retried until some sort of acknowledgement to the original command has been received. The command microservice is documented at https://mavlink.io/en/services/command.html
*/
// target_system System executing long running command. Should not be broadcast (0). uint8_t
// target_component Component executing long running command. uint8_t
// command Command ID (of command to cancel). uint16_t
export class CommandCancel extends MAVLinkMessage {
	public target_system!: number;
	public target_component!: number;
	public command!: MavCmd;
	public _message_id: number = 80;
	public _message_name: string = 'COMMAND_CANCEL';
	public _crc_extra: number = 14;
	public _message_fields: [string, string, boolean][] = [
		['command', 'uint16_t', false],
		['target_system', 'uint8_t', false],
		['target_component', 'uint8_t', false],
	];
}