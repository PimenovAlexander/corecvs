export enum ParamAck {
	PARAM_ACK_ACCEPTED = 0, // Parameter value ACCEPTED and SET
	PARAM_ACK_VALUE_UNSUPPORTED = 1, // Parameter value UNKNOWN/UNSUPPORTED
	PARAM_ACK_FAILED = 2, // Parameter failed to set
	PARAM_ACK_IN_PROGRESS = 3, // Parameter value received but not yet set/accepted. A subsequent PARAM_ACK_TRANSACTION or PARAM_EXT_ACK with the final result will follow once operation is completed. This is returned immediately for parameters that take longer to set, indicating taht the the parameter was recieved and does not need to be resent.
	PARAM_ACK_ENUM_END = 4, // 
}