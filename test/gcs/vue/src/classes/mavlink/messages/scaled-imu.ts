import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
The RAW IMU readings for the usual 9DOF sensor setup. This message should contain the scaled values to the described units
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// xacc X acceleration int16_t
// yacc Y acceleration int16_t
// zacc Z acceleration int16_t
// xgyro Angular speed around X axis int16_t
// ygyro Angular speed around Y axis int16_t
// zgyro Angular speed around Z axis int16_t
// xmag X Magnetic field int16_t
// ymag Y Magnetic field int16_t
// zmag Z Magnetic field int16_t
// temperature Temperature, 0: IMU does not provide temperature values. If the IMU is at 0C it must send 1 (0.01C). int16_t
export class ScaledImu extends MAVLinkMessage {
	public time_boot_ms!: number;
	public xacc!: number;
	public yacc!: number;
	public zacc!: number;
	public xgyro!: number;
	public ygyro!: number;
	public zgyro!: number;
	public xmag!: number;
	public ymag!: number;
	public zmag!: number;
	public temperature!: number;
	public _message_id: number = 26;
	public _message_name: string = 'SCALED_IMU';
	public _crc_extra: number = 170;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['xacc', 'int16_t', false],
		['yacc', 'int16_t', false],
		['zacc', 'int16_t', false],
		['xgyro', 'int16_t', false],
		['ygyro', 'int16_t', false],
		['zgyro', 'int16_t', false],
		['xmag', 'int16_t', false],
		['ymag', 'int16_t', false],
		['zmag', 'int16_t', false],
		['temperature', 'int16_t', true],
	];
}