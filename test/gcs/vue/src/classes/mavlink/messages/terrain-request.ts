import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Request for terrain data and terrain status. See terrain protocol docs: https://mavlink.io/en/services/terrain.html
*/
// lat Latitude of SW corner of first grid int32_t
// lon Longitude of SW corner of first grid int32_t
// grid_spacing Grid spacing uint16_t
// mask Bitmask of requested 4x4 grids (row major 8x7 array of grids, 56 bits) uint64_t
export class TerrainRequest extends MAVLinkMessage {
	public lat!: number;
	public lon!: number;
	public grid_spacing!: number;
	public mask!: number;
	public _message_id: number = 133;
	public _message_name: string = 'TERRAIN_REQUEST';
	public _crc_extra: number = 6;
	public _message_fields: [string, string, boolean][] = [
		['mask', 'uint64_t', false],
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['grid_spacing', 'uint16_t', false],
	];
}