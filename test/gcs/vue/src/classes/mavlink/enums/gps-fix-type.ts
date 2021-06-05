export enum GpsFixType {
	GPS_FIX_TYPE_NO_GPS = 0, // No GPS connected
	GPS_FIX_TYPE_NO_FIX = 1, // No position information, GPS is connected
	GPS_FIX_TYPE_2D_FIX = 2, // 2D position
	GPS_FIX_TYPE_3D_FIX = 3, // 3D position
	GPS_FIX_TYPE_DGPS = 4, // DGPS/SBAS aided 3D position
	GPS_FIX_TYPE_RTK_FLOAT = 5, // RTK float, 3D position
	GPS_FIX_TYPE_RTK_FIXED = 6, // RTK Fixed, 3D position
	GPS_FIX_TYPE_STATIC = 7, // Static fixed, typically used for base stations
	GPS_FIX_TYPE_PPP = 8, // PPP, 3D position.
	GPS_FIX_TYPE_ENUM_END = 9, // 
}