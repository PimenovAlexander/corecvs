export enum ParamTransactionAction {
	PARAM_TRANSACTION_ACTION_START = 0, // Commit the current parameter transaction.
	PARAM_TRANSACTION_ACTION_COMMIT = 1, // Commit the current parameter transaction.
	PARAM_TRANSACTION_ACTION_CANCEL = 2, // Cancel the current parameter transaction.
	PARAM_TRANSACTION_ACTION_ENUM_END = 3, // 
}