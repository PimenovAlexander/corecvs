#ifndef  _ERRORS_H_
#define  _ERRORS_H_

#define ERR_NO                                                                                                                                                                          1
#define ERR_NULL_INPUT_PARAMETER                                                                                                 -1
#define ERR_MEMORY                                                                                                                                                         -2
#define ERR_OPEN_FILE_FOR_WRITE                                                                                                   -3
#define ERR_WRITING_HISTORY_ELEMENT_TO_FILE                                                         -4
#define ERR_WRITING_BALLS_DESCRIPTOR_TO_FILE                                                 -5
#define ERR_WRITING_BALL_DESCRIPTOR_TO_FILE                                                         -6
#define ERR_OVERFLOW_HISTORY                                                                                                           -7 
#define ERR_OVERFLOW_BALLS                                                                                                                    -8 
#define ERR_COMPARE_BALLS_DESCRIPTOR                                                                            -9
#define ERR_OPEN_FILE_FOR_READ                                                                                                   -10
#define ERR_UNKNOWN_COLOR                                                                                                                           -11

#define DETECTOR_NOT_TERMINATED                                                                                                   -12
#define DETECTOR_NOT_INITIALIZED                                                                                                -13
#define DETECTOR_NOT_STARTED                                                                                                    -14
#define DETECTOR_ALREADY_STARTED                                                                                                -15
#define DETECT_THREAD_WAIT_FAILED                                                                                                -16
#define DETECT_TIME_FOR_TERMINATION_THREAD_IS_EXCEEDED        -17
#define DETECT_THREAD_WAIT_ABANDONED                                                                                -18
#define ERR_NO_NEW_DATA                                                                                                                                        -19
#endif /* _ERRORS_H_ */