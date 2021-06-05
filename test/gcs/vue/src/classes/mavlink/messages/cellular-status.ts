import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CellularStatusFlag} from '../enums/cellular-status-flag';
import {CellularNetworkFailedReason} from '../enums/cellular-network-failed-reason';
import {CellularNetworkRadioType} from '../enums/cellular-network-radio-type';
/*
Report current used cellular network status
*/
// status Cellular modem status uint8_t
// failure_reason Failure reason when status in in CELLUAR_STATUS_FAILED uint8_t
// type Cellular network radio type: gsm, cdma, lte... uint8_t
// quality Signal quality in percent. If unknown, set to UINT8_MAX uint8_t
// mcc Mobile country code. If unknown, set to UINT16_MAX uint16_t
// mnc Mobile network code. If unknown, set to UINT16_MAX uint16_t
// lac Location area code. If unknown, set to 0 uint16_t
export class CellularStatus extends MAVLinkMessage {
	public status!: CellularStatusFlag;
	public failure_reason!: CellularNetworkFailedReason;
	public type!: CellularNetworkRadioType;
	public quality!: number;
	public mcc!: number;
	public mnc!: number;
	public lac!: number;
	public _message_id: number = 334;
	public _message_name: string = 'CELLULAR_STATUS';
	public _crc_extra: number = 72;
	public _message_fields: [string, string, boolean][] = [
		['mcc', 'uint16_t', false],
		['mnc', 'uint16_t', false],
		['lac', 'uint16_t', false],
		['status', 'uint8_t', false],
		['failure_reason', 'uint8_t', false],
		['type', 'uint8_t', false],
		['quality', 'uint8_t', false],
	];
}