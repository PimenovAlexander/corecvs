import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {SerialControlDev} from '../enums/serial-control-dev';
import {SerialControlFlag} from '../enums/serial-control-flag';
/*
Control a serial port. This can be used for raw access to an onboard serial peripheral such as a GPS or telemetry radio. It is designed to make it possible to update the devices firmware via MAVLink messages or change the devices settings. A message with zero bytes can be used to change just the baudrate.
*/
// device Serial control device type. uint8_t
// flags Bitmap of serial control flags. uint8_t
// timeout Timeout for reply data uint16_t
// baudrate Baudrate of transfer. Zero means no change. uint32_t
// count how many bytes in this transfer uint8_t
// data serial data uint8_t
export class SerialControl extends MAVLinkMessage {
	public device!: SerialControlDev;
	public flags!: SerialControlFlag;
	public timeout!: number;
	public baudrate!: number;
	public count!: number;
	public data!: number;
	public _message_id: number = 126;
	public _message_name: string = 'SERIAL_CONTROL';
	public _crc_extra: number = 220;
	public _message_fields: [string, string, boolean][] = [
		['baudrate', 'uint32_t', false],
		['timeout', 'uint16_t', false],
		['device', 'uint8_t', false],
		['flags', 'uint8_t', false],
		['count', 'uint8_t', false],
		['data', 'uint8_t', false],
	];
}