export enum FailureType {
	FAILURE_TYPE_OK = 0, // No failure injected, used to reset a previous failure.
	FAILURE_TYPE_OFF = 1, // Sets unit off, so completely non-responsive.
	FAILURE_TYPE_STUCK = 2, // Unit is stuck e.g. keeps reporting the same value.
	FAILURE_TYPE_GARBAGE = 3, // Unit is reporting complete garbage.
	FAILURE_TYPE_WRONG = 4, // Unit is consistently wrong.
	FAILURE_TYPE_SLOW = 5, // Unit is slow, so e.g. reporting at slower than expected rate.
	FAILURE_TYPE_DELAYED = 6, // Data of unit is delayed in time.
	FAILURE_TYPE_INTERMITTENT = 7, // Unit is sometimes working, sometimes not.
	FAILURE_TYPE_ENUM_END = 8, // 
}