export enum MavOdidIdType {
	MAV_ODID_ID_TYPE_NONE = 0, // No type defined.
	MAV_ODID_ID_TYPE_SERIAL_NUMBER = 1, // Manufacturer Serial Number (ANSI/CTA-2063 format).
	MAV_ODID_ID_TYPE_CAA_REGISTRATION_ID = 2, // CAA (Civil Aviation Authority) registered ID. Format: [ICAO Country Code].[CAA Assigned ID].
	MAV_ODID_ID_TYPE_UTM_ASSIGNED_UUID = 3, // UTM (Unmanned Traffic Management) assigned UUID (RFC4122).
	MAV_ODID_ID_TYPE_ENUM_END = 4, // 
}