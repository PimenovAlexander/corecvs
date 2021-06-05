export enum AisNavStatus {
	UNDER_WAY = 0, // Under way using engine.
	AIS_NAV_ANCHORED = 1, // 
	AIS_NAV_UN_COMMANDED = 2, // 
	AIS_NAV_RESTRICTED_MANOEUVERABILITY = 3, // 
	AIS_NAV_DRAUGHT_CONSTRAINED = 4, // 
	AIS_NAV_MOORED = 5, // 
	AIS_NAV_AGROUND = 6, // 
	AIS_NAV_FISHING = 7, // 
	AIS_NAV_SAILING = 8, // 
	AIS_NAV_RESERVED_HSC = 9, // 
	AIS_NAV_RESERVED_WIG = 10, // 
	AIS_NAV_RESERVED_1 = 11, // 
	AIS_NAV_RESERVED_2 = 12, // 
	AIS_NAV_RESERVED_3 = 13, // 
	AIS_NAV_AIS_SART = 14, // Search And Rescue Transponder.
	AIS_NAV_UNKNOWN = 15, // Not available (default).
	AIS_NAV_STATUS_ENUM_END = 16, // 
}