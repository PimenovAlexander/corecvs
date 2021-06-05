import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
RTCM message for injecting into the onboard GPS (used for DGPS)
*/
// flags LSB: 1 means message is fragmented, next 2 bits are the fragment ID, the remaining 5 bits are used for the sequence ID. Messages are only to be flushed to the GPS when the entire message has been reconstructed on the autopilot. The fragment ID specifies which order the fragments should be assembled into a buffer, while the sequence ID is used to detect a mismatch between different buffers. The buffer is considered fully reconstructed when either all 4 fragments are present, or all the fragments before the first fragment with a non full payload is received. This management is used to ensure that normal GPS operation doesn't corrupt RTCM data, and to recover from a unreliable transport delivery order. uint8_t
// len data length uint8_t
// data RTCM message (may be fragmented) uint8_t
export class GpsRtcmData extends MAVLinkMessage {
	public flags!: number;
	public len!: number;
	public data!: number;
	public _message_id: number = 233;
	public _message_name: string = 'GPS_RTCM_DATA';
	public _crc_extra: number = 35;
	public _message_fields: [string, string, boolean][] = [
		['flags', 'uint8_t', false],
		['len', 'uint8_t', false],
		['data', 'uint8_t', false],
	];
}