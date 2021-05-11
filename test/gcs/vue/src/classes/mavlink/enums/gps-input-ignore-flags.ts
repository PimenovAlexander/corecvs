export enum GpsInputIgnoreFlags {
	GPS_INPUT_IGNORE_FLAG_ALT = 1, // ignore altitude field
	GPS_INPUT_IGNORE_FLAG_HDOP = 2, // ignore hdop field
	GPS_INPUT_IGNORE_FLAG_VDOP = 4, // ignore vdop field
	GPS_INPUT_IGNORE_FLAG_VEL_HORIZ = 8, // ignore horizontal velocity field (vn and ve)
	GPS_INPUT_IGNORE_FLAG_VEL_VERT = 16, // ignore vertical velocity field (vd)
	GPS_INPUT_IGNORE_FLAG_SPEED_ACCURACY = 32, // ignore speed accuracy field
	GPS_INPUT_IGNORE_FLAG_HORIZONTAL_ACCURACY = 64, // ignore horizontal accuracy field
	GPS_INPUT_IGNORE_FLAG_VERTICAL_ACCURACY = 128, // ignore vertical accuracy field
	GPS_INPUT_IGNORE_FLAGS_ENUM_END = 129, // 
}