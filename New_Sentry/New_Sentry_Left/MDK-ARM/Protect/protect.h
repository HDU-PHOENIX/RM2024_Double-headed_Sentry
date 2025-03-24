#ifndef PROTECT_H
#define PROTECT_H
#include "main.h"
#include "cmsis_os.h"

extern osThreadId ShowRunningTaskHandle;
extern osThreadId defaultTaskHandle;
extern osThreadId MotorTaskHandle;
extern osThreadId GimbalTaskHandle;
extern osThreadId ShootTaskHandle;
extern osThreadId OutControl_TaskHandle;
extern osThreadId ChassisTaskHandle;

void PID_clear_all(void);
void soft_reset(void);

#define SAFE_MODE 1

#define    TIME_STAMP_1MS        1
#define    TIME_STAMP_2MS        2
#define    TIME_STAMP_4MS        4
#define    TIME_STAMP_10MS      10
#define	   TIME_STAMP_15MS		15
#define    TIME_STAMP_20MS      20
#define    TIME_STAMP_30MS      30
#define    TIME_STAMP_40MS      40
#define    TIME_STAMP_50MS      50
#define    TIME_STAMP_60MS      60
#define    TIME_STAMP_80MS      80
#define    TIME_STAMP_100MS    100
#define    TIME_STAMP_150MS    150
#define    TIME_STAMP_200MS    200
#define    TIME_STAMP_250MS    250
#define    TIME_STAMP_300MS    300
#define    TIME_STAMP_400MS    400
#define    TIME_STAMP_500MS    500
#define    TIME_STAMP_1000MS  1000
#define    TIME_STAMP_2000MS  2000
#define    TIME_STAMP_10S     10000

#define    FALSE    0
#define    TRUE     1


#endif


