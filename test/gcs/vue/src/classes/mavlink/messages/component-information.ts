import {MAVLinkMessage} from '@ifrunistuttgart/node-mavlink';
import {readInt64LE, readUInt64LE} from '@ifrunistuttgart/node-mavlink';
import {CompMetadataType} from '../enums/comp-metadata-type';
/*
Information about a component. For camera components instead use CAMERA_INFORMATION, and for autopilots use AUTOPILOT_VERSION. Components including GCSes should consider supporting requests of this message via MAV_CMD_REQUEST_MESSAGE.
*/
// time_boot_ms Timestamp (time since system boot). uint32_t
// metadata_type The type of metadata being requested. uint32_t
// metadata_uid Unique uid for this metadata which a gcs can use for created cached metadata and understanding whether it's cache it up to date or whether it needs to download new data. uint32_t
// metadata_uri Component definition URI. If prefix mavlinkftp:// file is downloaded from vehicle over mavlink ftp protocol. If prefix http[s]:// file is downloaded over internet. Files are json format. They can end in .gz to indicate file is in gzip format. char
// translation_uid Unique uid for the translation file associated with the metadata. uint32_t
// translation_uri The translations for strings within the metadata file. If null the either do not exist or may be included in the metadata file itself. The translations specified here supercede any which may be in the metadata file itself. The uri format is the same as component_metadata_uri . Files are in Json Translation spec format. Empty string indicates no tranlsation file. char
export class ComponentInformation extends MAVLinkMessage {
	public time_boot_ms!: number;
	public metadata_type!: CompMetadataType;
	public metadata_uid!: number;
	public metadata_uri!: string;
	public translation_uid!: number;
	public translation_uri!: string;
	public _message_id: number = 395;
	public _message_name: string = 'COMPONENT_INFORMATION';
	public _crc_extra: number = 163;
	public _message_fields: [string, string, boolean][] = [
		['time_boot_ms', 'uint32_t', false],
		['metadata_type', 'uint32_t', false],
		['metadata_uid', 'uint32_t', false],
		['translation_uid', 'uint32_t', false],
		['metadata_uri', 'char', false],
		['translation_uri', 'char', false],
	];
}