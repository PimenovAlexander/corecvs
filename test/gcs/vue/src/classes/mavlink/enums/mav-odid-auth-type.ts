export enum MavOdidAuthType {
	MAV_ODID_AUTH_TYPE_NONE = 0, // No authentication type is specified.
	MAV_ODID_AUTH_TYPE_UAS_ID_SIGNATURE = 1, // Signature for the UAS (Unmanned Aircraft System) ID.
	MAV_ODID_AUTH_TYPE_OPERATOR_ID_SIGNATURE = 2, // Signature for the Operator ID.
	MAV_ODID_AUTH_TYPE_MESSAGE_SET_SIGNATURE = 3, // Signature for the entire message set.
	MAV_ODID_AUTH_TYPE_NETWORK_REMOTE_ID = 4, // Authentication is provided by Network Remote ID.
	MAV_ODID_AUTH_TYPE_ENUM_END = 5, // 
}