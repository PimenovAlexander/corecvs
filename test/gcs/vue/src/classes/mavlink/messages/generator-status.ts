import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {MavGeneratorStatusFlag} from '../enums/mav-generator-status-flag';
/*
Telemetry of power generation system. Alternator or mechanical generator.
*/
// status Status flags. uint64_t
// generator_speed Speed of electrical generator or alternator. UINT16_MAX: field not provided. uint16_t
// battery_current Current into/out of battery. Positive for out. Negative for in. NaN: field not provided. float
// load_current Current going to the UAV. If battery current not available this is the DC current from the generator. Positive for out. Negative for in. NaN: field not provided float
// power_generated The power being generated. NaN: field not provided float
// bus_voltage Voltage of the bus seen at the generator, or battery bus if battery bus is controlled by generator and at a different voltage to main bus. float
// rectifier_temperature The temperature of the rectifier or power converter. INT16_MAX: field not provided. int16_t
// bat_current_setpoint The target battery current. Positive for out. Negative for in. NaN: field not provided float
// generator_temperature The temperature of the mechanical motor, fuel cell core or generator. INT16_MAX: field not provided. int16_t
// runtime Seconds this generator has run since it was rebooted. UINT32_MAX: field not provided. uint32_t
// time_until_maintenance Seconds until this generator requires maintenance.  A negative value indicates maintenance is past-due. INT32_MAX: field not provided. int32_t
export class GeneratorStatus extends MAVLinkMessage {
	public status!: MavGeneratorStatusFlag;
	public generator_speed!: number;
	public battery_current!: number;
	public load_current!: number;
	public power_generated!: number;
	public bus_voltage!: number;
	public rectifier_temperature!: number;
	public bat_current_setpoint!: number;
	public generator_temperature!: number;
	public runtime!: number;
	public time_until_maintenance!: number;
	public _message_id: number = 373;
	public _message_name: string = 'GENERATOR_STATUS';
	public _crc_extra: number = 117;
	public _message_fields: [string, string, boolean][] = [
		['status', 'uint64_t', false],
		['battery_current', 'float', false],
		['load_current', 'float', false],
		['power_generated', 'float', false],
		['bus_voltage', 'float', false],
		['bat_current_setpoint', 'float', false],
		['runtime', 'uint32_t', false],
		['time_until_maintenance', 'int32_t', false],
		['generator_speed', 'uint16_t', false],
		['rectifier_temperature', 'int16_t', false],
		['generator_temperature', 'int16_t', false],
	];
}