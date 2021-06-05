import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {UavcanNodeHealth} from '../enums/uavcan-node-health';
import {UavcanNodeMode} from '../enums/uavcan-node-mode';
/*
General status information of an UAVCAN node. Please refer to the definition of the UAVCAN message "uavcan.protocol.NodeStatus" for the background information. The UAVCAN specification is available at http://uavcan.org.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// uptime_sec Time since the start-up of the node. uint32_t
// health Generalized node health status. uint8_t
// mode Generalized operating mode. uint8_t
// sub_mode Not used currently. uint8_t
// vendor_specific_status_code Vendor-specific status information. uint16_t
export class UavcanNodeStatus extends MAVLinkMessage {
	public time_usec!: number;
	public uptime_sec!: number;
	public health!: UavcanNodeHealth;
	public mode!: UavcanNodeMode;
	public sub_mode!: number;
	public vendor_specific_status_code!: number;
	public _message_id: number = 310;
	public _message_name: string = 'UAVCAN_NODE_STATUS';
	public _crc_extra: number = 28;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['uptime_sec', 'uint32_t', false],
		['vendor_specific_status_code', 'uint16_t', false],
		['health', 'uint8_t', false],
		['mode', 'uint8_t', false],
		['sub_mode', 'uint8_t', false],
	];
}