export enum CompMetadataType {
	COMP_METADATA_TYPE_VERSION = 0, // Version information which also includes information on other optional supported COMP_METADATA_TYPE's. Must be supported. Only downloadable from vehicle.
	COMP_METADATA_TYPE_PARAMETER = 1, // Parameter meta data.
	COMP_METADATA_TYPE_COMMANDS = 2, // Meta data which specifies the commands the vehicle supports. (WIP)
	COMP_METADATA_TYPE_ENUM_END = 3, // 
}