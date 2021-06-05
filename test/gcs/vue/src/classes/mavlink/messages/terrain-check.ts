import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Request that the vehicle report terrain height at the given location (expected response is a TERRAIN_REPORT). Used by GCS to check if vehicle has all terrain data needed for a mission.
*/
// lat Latitude int32_t
// lon Longitude int32_t
export class TerrainCheck extends MAVLinkMessage {
	public lat!: number;
	public lon!: number;
	public _message_id: number = 135;
	public _message_name: string = 'TERRAIN_CHECK';
	public _crc_extra: number = 203;
	public _message_fields: [string, string, boolean][] = [
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
	];
}