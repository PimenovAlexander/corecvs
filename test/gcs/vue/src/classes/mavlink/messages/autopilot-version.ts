import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavProtocolCapability} from '../enums/mav-protocol-capability';
/*
Version and capability of autopilot software. This should be emitted in response to a request with MAV_CMD_REQUEST_MESSAGE.
*/
// capabilities Bitmap of capabilities uint64_t
// flight_sw_version Firmware version number uint32_t
// middleware_sw_version Middleware version number uint32_t
// os_sw_version Operating system version number uint32_t
// board_version HW / board version (last 8 bytes should be silicon ID, if any) uint32_t
// flight_custom_version Custom version field, commonly the first 8 bytes of the git hash. This is not an unique identifier, but should allow to identify the commit using the main version number even for very large code bases. uint8_t
// middleware_custom_version Custom version field, commonly the first 8 bytes of the git hash. This is not an unique identifier, but should allow to identify the commit using the main version number even for very large code bases. uint8_t
// os_custom_version Custom version field, commonly the first 8 bytes of the git hash. This is not an unique identifier, but should allow to identify the commit using the main version number even for very large code bases. uint8_t
// vendor_id ID of the board vendor uint16_t
// product_id ID of the product uint16_t
// uid UID if provided by hardware (see uid2) uint64_t
// uid2 UID if provided by hardware (supersedes the uid field. If this is non-zero, use this field, otherwise use uid) uint8_t
export class AutopilotVersion extends MAVLinkMessage {
	public capabilities!: MavProtocolCapability;
	public flight_sw_version!: number;
	public middleware_sw_version!: number;
	public os_sw_version!: number;
	public board_version!: number;
	public flight_custom_version!: number;
	public middleware_custom_version!: number;
	public os_custom_version!: number;
	public vendor_id!: number;
	public product_id!: number;
	public uid!: number;
	public uid2!: number;
	public _message_id: number = 148;
	public _message_name: string = 'AUTOPILOT_VERSION';
	public _crc_extra: number = 178;
	public _message_fields: [string, string, boolean][] = [
		['capabilities', 'uint64_t', false],
		['uid', 'uint64_t', false],
		['flight_sw_version', 'uint32_t', false],
		['middleware_sw_version', 'uint32_t', false],
		['os_sw_version', 'uint32_t', false],
		['board_version', 'uint32_t', false],
		['vendor_id', 'uint16_t', false],
		['product_id', 'uint16_t', false],
		['flight_custom_version', 'uint8_t', false],
		['middleware_custom_version', 'uint8_t', false],
		['os_custom_version', 'uint8_t', false],
		['uid2', 'uint8_t', true],
	];
}