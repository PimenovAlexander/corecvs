import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Camera-IMU triggering and synchronisation message.
*/
// time_usec Timestamp for image frame (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. uint64_t
// seq Image frame sequence uint32_t
export class CameraTrigger extends MAVLinkMessage {
	public time_usec!: number;
	public seq!: number;
	public _message_id: number = 112;
	public _message_name: string = 'CAMERA_TRIGGER';
	public _crc_extra: number = 174;
	public _message_fields: [string, string, boolean][] = [
		['time_usec', 'uint64_t', false],
		['seq', 'uint32_t', false],
	];
}