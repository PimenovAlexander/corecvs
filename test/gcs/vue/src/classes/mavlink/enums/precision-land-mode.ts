export enum PrecisionLandMode {
	PRECISION_LAND_MODE_DISABLED = 0, // Normal (non-precision) landing.
	PRECISION_LAND_MODE_OPPORTUNISTIC = 1, // Use precision landing if beacon detected when land command accepted, otherwise land normally.
	PRECISION_LAND_MODE_REQUIRED = 2, // Use precision landing, searching for beacon if not found when land command accepted (land normally if beacon cannot be found).
	PRECISION_LAND_MODE_ENUM_END = 3, // 
}