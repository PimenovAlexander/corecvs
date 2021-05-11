export enum UavcanNodeHealth {
	UAVCAN_NODE_HEALTH_OK = 0, // The node is functioning properly.
	UAVCAN_NODE_HEALTH_WARNING = 1, // A critical parameter went out of range or the node has encountered a minor failure.
	UAVCAN_NODE_HEALTH_ERROR = 2, // The node has encountered a major failure.
	UAVCAN_NODE_HEALTH_CRITICAL = 3, // The node has suffered a fatal malfunction.
	UAVCAN_NODE_HEALTH_ENUM_END = 4, // 
}