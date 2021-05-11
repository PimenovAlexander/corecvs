export enum FirmwareVersionType {
	FIRMWARE_VERSION_TYPE_DEV = 0, // development release
	FIRMWARE_VERSION_TYPE_ALPHA = 64, // alpha release
	FIRMWARE_VERSION_TYPE_BETA = 128, // beta release
	FIRMWARE_VERSION_TYPE_RC = 192, // release candidate
	FIRMWARE_VERSION_TYPE_OFFICIAL = 255, // official stable release
	FIRMWARE_VERSION_TYPE_ENUM_END = 256, // 
}