export enum VtolTransitionHeading {
	VTOL_TRANSITION_HEADING_VEHICLE_DEFAULT = 0, // Respect the heading configuration of the vehicle.
	VTOL_TRANSITION_HEADING_NEXT_WAYPOINT = 1, // Use the heading pointing towards the next waypoint.
	VTOL_TRANSITION_HEADING_TAKEOFF = 2, // Use the heading on takeoff (while sitting on the ground).
	VTOL_TRANSITION_HEADING_SPECIFIED = 3, // Use the specified heading in parameter 4.
	VTOL_TRANSITION_HEADING_ANY = 4, // Use the current heading when reaching takeoff altitude (potentially facing the wind when weather-vaning is active).
	VTOL_TRANSITION_HEADING_ENUM_END = 5, // 
}