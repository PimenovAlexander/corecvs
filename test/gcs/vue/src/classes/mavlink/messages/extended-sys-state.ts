import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavVtolState} from '../enums/mav-vtol-state';
import {MavLandedState} from '../enums/mav-landed-state';
/*
Provides state for additional features
*/
// vtol_state The VTOL state if applicable. Is set to MAV_VTOL_STATE_UNDEFINED if UAV is not in VTOL configuration. uint8_t
// landed_state The landed state. Is set to MAV_LANDED_STATE_UNDEFINED if landed state is unknown. uint8_t
export class ExtendedSysState extends MAVLinkMessage {
	public vtol_state!: MavVtolState;
	public landed_state!: MavLandedState;
	public _message_id: number = 245;
	public _message_name: string = 'EXTENDED_SYS_STATE';
	public _crc_extra: number = 130;
	public _message_fields: [string, string, boolean][] = [
		['vtol_state', 'uint8_t', false],
		['landed_state', 'uint8_t', false],
	];
}