import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {WifiConfigApMode} from '../enums/wifi-config-ap-mode';
import {WifiConfigApResponse} from '../enums/wifi-config-ap-response';
/*
Configure WiFi AP SSID, password, and mode. This message is re-emitted as an acknowledgement by the AP. The message may also be explicitly requested using MAV_CMD_REQUEST_MESSAGE
*/
// ssid Name of Wi-Fi network (SSID). Blank to leave it unchanged when setting. Current SSID when sent back as a response. char
// password Password. Blank for an open AP. MD5 hash when message is sent back as a response. char
// mode WiFi Mode. int8_t
// response Message acceptance response (sent back to GS). int8_t
export class WifiConfigAp extends MAVLinkMessage {
	public ssid!: string;
	public password!: string;
	public mode!: WifiConfigApMode;
	public response!: WifiConfigApResponse;
	public _message_id: number = 299;
	public _message_name: string = 'WIFI_CONFIG_AP';
	public _crc_extra: number = 19;
	public _message_fields: [string, string, boolean][] = [
		['ssid', 'char', false],
		['password', 'char', false],
		['mode', 'int8_t', true],
		['response', 'int8_t', true],
	];
}