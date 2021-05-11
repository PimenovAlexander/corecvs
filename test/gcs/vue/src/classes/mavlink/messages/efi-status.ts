import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
EFI status output
*/
// health EFI health status uint8_t
// ecu_index ECU index float
// rpm RPM float
// fuel_consumed Fuel consumed float
// fuel_flow Fuel flow rate float
// engine_load Engine load float
// throttle_position Throttle position float
// spark_dwell_time Spark dwell time float
// barometric_pressure Barometric pressure float
// intake_manifold_pressure Intake manifold pressure( float
// intake_manifold_temperature Intake manifold temperature float
// cylinder_head_temperature Cylinder head temperature float
// ignition_timing Ignition timing (Crank angle degrees) float
// injection_time Injection time float
// exhaust_gas_temperature Exhaust gas temperature float
// throttle_out Output throttle float
// pt_compensation Pressure/temperature compensation float
export class EfiStatus extends MAVLinkMessage {
	public health!: number;
	public ecu_index!: number;
	public rpm!: number;
	public fuel_consumed!: number;
	public fuel_flow!: number;
	public engine_load!: number;
	public throttle_position!: number;
	public spark_dwell_time!: number;
	public barometric_pressure!: number;
	public intake_manifold_pressure!: number;
	public intake_manifold_temperature!: number;
	public cylinder_head_temperature!: number;
	public ignition_timing!: number;
	public injection_time!: number;
	public exhaust_gas_temperature!: number;
	public throttle_out!: number;
	public pt_compensation!: number;
	public _message_id: number = 225;
	public _message_name: string = 'EFI_STATUS';
	public _crc_extra: number = 208;
	public _message_fields: [string, string, boolean][] = [
		['ecu_index', 'float', false],
		['rpm', 'float', false],
		['fuel_consumed', 'float', false],
		['fuel_flow', 'float', false],
		['engine_load', 'float', false],
		['throttle_position', 'float', false],
		['spark_dwell_time', 'float', false],
		['barometric_pressure', 'float', false],
		['intake_manifold_pressure', 'float', false],
		['intake_manifold_temperature', 'float', false],
		['cylinder_head_temperature', 'float', false],
		['ignition_timing', 'float', false],
		['injection_time', 'float', false],
		['exhaust_gas_temperature', 'float', false],
		['throttle_out', 'float', false],
		['pt_compensation', 'float', false],
		['health', 'uint8_t', false],
	];
}