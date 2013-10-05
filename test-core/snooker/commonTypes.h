#ifndef  _COMMON_TYPES_H_
#define  _COMMON_TYPES_H_

#include "errors.h"

//Default size of history
#define HISTORY_SIZE  30

//max of possible number of balls //15(red)+7(color) = 22
#define MAX_BALLS_ON_TABLE  75
//max of possible number of balls witn different colors//1(red)+7(color) = 8
//#define MAX_NUBER_OF_DIFFENT_COLORS_FOR_BALLS  8
//max of possible number of balls for calibration
#define MAX_BALLS_ON_TABLE_FOR_CALIBRATION  75

//default values
#define BALL_RADIUS 5.25f/2
#define SNOOKER_TABLE_WIDTH 142.0f
#define SNOOKER_TABLE_LENGTH 254.0f

//default time between frames
#define MAGIC_NUMBER 6.0 //msec
#define DEFAULT_TIME_BETWEEN_FRAMES (DWORD)(40.0-MAGIC_NUMBER) // 40 msec = 1/25 times per second  - %

#define MAX_LENTHG_OF_STRING 100

//Structure for position of ball 
typedef struct Position
{
        float x;
        float y;
} Position;

enum ColorType {
    BALL_FIRST_COLOR = 0,
    BALL_WHITE = BALL_FIRST_COLOR,
    BALL_RED,
    BALL_YELLOW,
    BALL_GREEN,
    BALL_BROWN,
    BALL_BLUE,
    BALL_PINK,
    BALL_BLACK,
    BALL_UNKNOWN,
    BALL_LAST_COLOR = BALL_UNKNOWN
};

typedef struct MovieParameters
{
        //TODO:
        bool isEnable;
        unsigned int numberOfInputs;
        char name [MAX_LENTHG_OF_STRING];
} MovieParameters;

typedef struct CameraParameters
{
        bool isEnable;
  int camId;
  int widthHint;
  int heightHint;
} CameraParameters;

typedef struct VideoInputParameters
{
        CameraParameters cameraParameters;
        MovieParameters movieParameters;
} VideoInputParameters;

typedef struct SnookerTable
{
        float length;
        float width;
} SnookerTable;

typedef struct BallSize
{
        float radius;
} BallSize;

typedef struct BallParameters
{
        unsigned int color;
} BallParameters;


typedef struct BallsParameters
{
        unsigned int number; //number of elements in ballParameters (number of different color for balls)
        BallSize ballSize;
        BallParameters * ballParameters;
} BallsParameters;

//TODO:Add comments
struct AdditionParameters
{
        int minimumBallClusterSize;
        int reflectionPercentile;
        int reflectionLumaMin;
        int tableHue;
        int tableHueInterval;
        int tableLumaMin;
        int tableLumaMax;
        char nameOfFileForCalibration [MAX_LENTHG_OF_STRING];
};

struct Settings
{
    AdditionParameters parameters;
        VideoInputParameters videoInputParameters;
        SnookerTable snookerTable;
        BallsParameters ballsParameters;
};


//Structure for color and position of ball 
typedef struct BallDescriptor
{
        unsigned int color;
        Position ballPosition; // Position of thr center of the ball
        Position speckPosition; // Position of the speck fjr current ball
        Position tableBallPosition; // Position ball on table //user set in calibration tool
        bool isCenterMesuared; //Was ball center measured or interpoleted by speck
} BallDescriptor;

//Structure for keep information about all balls on table
typedef struct BallsDescriptor
{
        unsigned int number; //current number of balls on table (<=MAX_BALLS_ON_TABLE) and (<=MAX_BALLS_ON_TABLE_FOR_CALIBRATION for calibration)
        BallDescriptor* ballDescriptor;
} BallsDescriptor;

//Structure for keep information about all balls on table and time for this event
typedef struct HistoryElement
{
        unsigned int time; //time for current description
        BallsDescriptor * ballsDescriptor; //indformation about balls for current time
} HistoryElement;

typedef struct History
{
        unsigned int number; // number of real elements in history
        unsigned int maxNumber; // number of max possible elements in history
        HistoryElement * historyElement; 
} History;

typedef struct CalibrationInfo
{
        //Position for snooker table (ABCD)
        Position VertexA;
        Position VertexB;
        Position VertexC;
        Position VertexD;
  //balls information (balls positions and speck positions for each ball)
        BallsDescriptor ballsDescriptor; 
} CalibrationInfo;


#endif /* _COMMON_TYPES_H_ */