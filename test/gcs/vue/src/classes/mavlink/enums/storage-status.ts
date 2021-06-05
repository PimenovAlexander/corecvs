export enum StorageStatus {
	STORAGE_STATUS_EMPTY = 0, // Storage is missing (no microSD card loaded for example.)
	STORAGE_STATUS_UNFORMATTED = 1, // Storage present but unformatted.
	STORAGE_STATUS_READY = 2, // Storage present and ready.
	STORAGE_STATUS_NOT_SUPPORTED = 3, // Camera does not supply storage status information. Capacity information in STORAGE_INFORMATION fields will be ignored.
	STORAGE_STATUS_ENUM_END = 4, // 
}