export enum CameraZoomType {
	ZOOM_TYPE_STEP = 0, // Zoom one step increment (-1 for wide, 1 for tele)
	ZOOM_TYPE_CONTINUOUS = 1, // Continuous zoom up/down until stopped (-1 for wide, 1 for tele, 0 to stop zooming)
	ZOOM_TYPE_RANGE = 2, // Zoom value as proportion of full camera range (a value between 0.0 and 100.0)
	ZOOM_TYPE_FOCAL_LENGTH = 3, // Zoom value/variable focal length in milimetres. Note that there is no message to get the valid zoom range of the camera, so this can type can only be used for cameras where the zoom range is known (implying that this cannot reliably be used in a GCS for an arbitrary camera)
	CAMERA_ZOOM_TYPE_ENUM_END = 4, // 
}