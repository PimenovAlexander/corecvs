import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CellularConfigResponse} from '../enums/cellular-config-response';
/*
Configure cellular modems. This message is re-emitted as an acknowledgement by the modem. The message may also be explicitly requested using MAV_CMD_REQUEST_MESSAGE.
*/
// enable_lte Enable/disable LTE. 0: setting unchanged, 1: disabled, 2: enabled. Current setting when sent back as a response. uint8_t
// enable_pin Enable/disable PIN on the SIM card. 0: setting unchanged, 1: disabled, 2: enabled. Current setting when sent back as a response. uint8_t
// pin PIN sent to the SIM card. Blank when PIN is disabled. Empty when message is sent back as a response. char
// new_pin New PIN when changing the PIN. Blank to leave it unchanged. Empty when message is sent back as a response. char
// apn Name of the cellular APN. Blank to leave it unchanged. Current APN when sent back as a response. char
// puk Required PUK code in case the user failed to authenticate 3 times with the PIN. Empty when message is sent back as a response. char
// roaming Enable/disable roaming. 0: setting unchanged, 1: disabled, 2: enabled. Current setting when sent back as a response. uint8_t
// response Message acceptance response (sent back to GS). uint8_t
export class CellularConfig extends MAVLinkMessage {
	public enable_lte!: number;
	public enable_pin!: number;
	public pin!: string;
	public new_pin!: string;
	public apn!: string;
	public puk!: string;
	public roaming!: number;
	public response!: CellularConfigResponse;
	public _message_id: number = 336;
	public _message_name: string = 'CELLULAR_CONFIG';
	public _crc_extra: number = 245;
	public _message_fields: [string, string, boolean][] = [
		['enable_lte', 'uint8_t', false],
		['enable_pin', 'uint8_t', false],
		['pin', 'char', false],
		['new_pin', 'char', false],
		['apn', 'char', false],
		['puk', 'char', false],
		['roaming', 'uint8_t', false],
		['response', 'uint8_t', false],
	];
}