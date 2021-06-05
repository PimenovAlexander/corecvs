export enum FenceAction {
	FENCE_ACTION_NONE = 0, // Disable fenced mode
	FENCE_ACTION_GUIDED = 1, // Switched to guided mode to return point (fence point 0)
	FENCE_ACTION_REPORT = 2, // Report fence breach, but don't take action
	FENCE_ACTION_GUIDED_THR_PASS = 3, // Switched to guided mode to return point (fence point 0) with manual throttle control
	FENCE_ACTION_RTL = 4, // Switch to RTL (return to launch) mode and head for the return point.
	FENCE_ACTION_ENUM_END = 5, // 
}