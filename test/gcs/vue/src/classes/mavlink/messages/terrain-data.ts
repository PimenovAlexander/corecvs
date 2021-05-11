import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
/*
Terrain data sent from GCS. The lat/lon and grid_spacing must be the same as a lat/lon from a TERRAIN_REQUEST. See terrain protocol docs: https://mavlink.io/en/services/terrain.html
*/
// lat Latitude of SW corner of first grid int32_t
// lon Longitude of SW corner of first grid int32_t
// grid_spacing Grid spacing uint16_t
// gridbit bit within the terrain request mask uint8_t
// data Terrain data MSL int16_t
export class TerrainData extends MAVLinkMessage {
	public lat!: number;
	public lon!: number;
	public grid_spacing!: number;
	public gridbit!: number;
	public data!: number;
	public _message_id: number = 134;
	public _message_name: string = 'TERRAIN_DATA';
	public _crc_extra: number = 229;
	public _message_fields: [string, string, boolean][] = [
		['lat', 'int32_t', false],
		['lon', 'int32_t', false],
		['grid_spacing', 'uint16_t', false],
		['data', 'int16_t', false],
		['gridbit', 'uint8_t', false],
	];
}