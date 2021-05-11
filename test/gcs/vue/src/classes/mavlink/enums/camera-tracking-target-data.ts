export enum CameraTrackingTargetData {
	CAMERA_TRACKING_TARGET_DATA_NONE = 0, // No target data
	CAMERA_TRACKING_TARGET_DATA_EMBEDDED = 1, // Target data embedded in image data (proprietary)
	CAMERA_TRACKING_TARGET_DATA_RENDERED = 2, // Target data rendered in image
	CAMERA_TRACKING_TARGET_DATA_IN_STATUS = 4, // Target data within status message (Point or Rectangle)
	CAMERA_TRACKING_TARGET_DATA_ENUM_END = 5, // 
}