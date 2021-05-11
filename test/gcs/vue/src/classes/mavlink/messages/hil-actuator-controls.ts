import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavModeFlag} from '../enums/mav-mode-flag';
/*
Sent from autopilot to simulation. Hardware in the loop control outputs (replacement for HIL_CONTROLS)
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// controls Control outputs -1 .. 1. Channel assignment depends on the simulated hardware. float
// mode System mode. Includes arming state. uint8_t
// flags Flags as bitfield, 1: indicate simulation using lockstep. uint64_t
export class HilActuatorControls extends MAVLinkMessage {
	public time_usec!: number;
	public controls!: number;
	public mode!: MavModeFlag;
	public flags!: number;
	public _message_id: number = 93;
	public _message_name: string = 'HIL_ACTUATOR_CONTROLS';
	public _crc_extra: number = 47;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['flags', 'uint64_t', false],
		['controls', 'float', false],
		['mode', 'uint8_t', false],
	];
}