import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Hardware status sent by an onboard computer.
*/
// time_usec Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// uptime Time since system boot. uint32_t
// type Type of the onboard computer: 0: Mission computer primary, 1: Mission computer backup 1, 2: Mission computer backup 2, 3: Compute node, 4-5: Compute spares, 6-9: Payload computers. uint8_t
// cpu_cores CPU usage on the component in percent (100 - idle). A value of UINT8_MAX implies the field is unused. uint8_t
// cpu_combined Combined CPU usage as the last 10 slices of 100 MS (a histogram). This allows to identify spikes in load that max out the system, but only for a short amount of time. A value of UINT8_MAX implies the field is unused. uint8_t
// gpu_cores GPU usage on the component in percent (100 - idle). A value of UINT8_MAX implies the field is unused. uint8_t
// gpu_combined Combined GPU usage as the last 10 slices of 100 MS (a histogram). This allows to identify spikes in load that max out the system, but only for a short amount of time. A value of UINT8_MAX implies the field is unused. uint8_t
// temperature_board Temperature of the board. A value of INT8_MAX implies the field is unused. int8_t
// temperature_core Temperature of the CPU core. A value of INT8_MAX implies the field is unused. int8_t
// fan_speed Fan speeds. A value of INT16_MAX implies the field is unused. int16_t
// ram_usage Amount of used RAM on the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// ram_total Total amount of RAM on the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// storage_type Storage type: 0: HDD, 1: SSD, 2: EMMC, 3: SD card (non-removable), 4: SD card (removable). A value of UINT32_MAX implies the field is unused. uint32_t
// storage_usage Amount of used storage space on the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// storage_total Total amount of storage space on the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// link_type Link type: 0-9: UART, 10-19: Wired network, 20-29: Wifi, 30-39: Point-to-point proprietary, 40-49: Mesh proprietary uint32_t
// link_tx_rate Network traffic from the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// link_rx_rate Network traffic to the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// link_tx_max Network capacity from the component system. A value of UINT32_MAX implies the field is unused. uint32_t
// link_rx_max Network capacity to the component system. A value of UINT32_MAX implies the field is unused. uint32_t
export class OnboardComputerStatus extends MAVLinkMessage {
	public time_usec!: number;
	public uptime!: number;
	public type!: number;
	public cpu_cores!: number;
	public cpu_combined!: number;
	public gpu_cores!: number;
	public gpu_combined!: number;
	public temperature_board!: number;
	public temperature_core!: number;
	public fan_speed!: number;
	public ram_usage!: number;
	public ram_total!: number;
	public storage_type!: number;
	public storage_usage!: number;
	public storage_total!: number;
	public link_type!: number;
	public link_tx_rate!: number;
	public link_rx_rate!: number;
	public link_tx_max!: number;
	public link_rx_max!: number;
	public _message_id: number = 390;
	public _message_name: string = 'ONBOARD_COMPUTER_STATUS';
	public _crc_extra: number = 156;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['uptime', 'uint32_t', false],
		['ram_usage', 'uint32_t', false],
		['ram_total', 'uint32_t', false],
		['storage_type', 'uint32_t', false],
		['storage_usage', 'uint32_t', false],
		['storage_total', 'uint32_t', false],
		['link_type', 'uint32_t', false],
		['link_tx_rate', 'uint32_t', false],
		['link_rx_rate', 'uint32_t', false],
		['link_tx_max', 'uint32_t', false],
		['link_rx_max', 'uint32_t', false],
		['fan_speed', 'int16_t', false],
		['type', 'uint8_t', false],
		['cpu_cores', 'uint8_t', false],
		['cpu_combined', 'uint8_t', false],
		['gpu_cores', 'uint8_t', false],
		['gpu_combined', 'uint8_t', false],
		['temperature_board', 'int8_t', false],
		['temperature_core', 'int8_t', false],
	];
}