export enum LandingTargetType {
	LANDING_TARGET_TYPE_LIGHT_BEACON = 0, // Landing target signaled by light beacon (ex: IR-LOCK)
	LANDING_TARGET_TYPE_RADIO_BEACON = 1, // Landing target signaled by radio beacon (ex: ILS, NDB)
	LANDING_TARGET_TYPE_VISION_FIDUCIAL = 2, // Landing target represented by a fiducial marker (ex: ARTag)
	LANDING_TARGET_TYPE_VISION_OTHER = 3, // Landing target represented by a pre-defined visual shape/feature (ex: X-marker, H-marker, square)
	LANDING_TARGET_TYPE_ENUM_END = 4, // 
}