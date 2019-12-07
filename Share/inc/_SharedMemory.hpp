#ifndef _SHAREDMEMORY_HPP
#define _SHAREDMEMORY_HPP
#include "_StdFunc.hpp"
#include "_Config.hpp"

/*-----------------------------------------------
ACTUATOR
-----------------------------------------------*/
#define SHM_NAME_ACTUATOR_VALUE "/ActuatorValue"
#define SHM_NAME_ACTUATOR_COMPLETE "/ActuatorComplete"
#ifdef TEAM_A
#define NUM_ACTUATOR 10
#endif
#ifdef TEAM_B
#define NUM_ACTUATOR 15
#endif
/*-----------------------------------------------
LINE
-----------------------------------------------*/
#define SHM_NAME_LINE "/Line"
#define SHM_NAME_PRE_LINE "/PreLine"
#define NUM_LINE 3
/*-----------------------------------------------
LIMIT
-----------------------------------------------*/
#define SHM_NAME_LIMIT "/Limit"
#ifdef TEAM_A
#define NUM_LIMIT 5
#endif
#ifdef TEAM_B
#define NUM_LIMIT 7
#endif
/*-----------------------------------------------
CHASSIS
-----------------------------------------------*/
#define SHM_NAME_SPIN "/Spin"
#define SHM_NAME_TARGET_SPIN "/TargetSpin"
#define SHM_NAME_SPIN_OFFSET "/SpinOffset"
#define SHM_NAME_MOTION "/Motion"
/*-----------------------------------------------
ODOMETRY
-----------------------------------------------*/
#define SHM_NAME_ODOMETRY "/Odometry"
/*-----------------------------------------------
EXECUTING
-----------------------------------------------*/
#define SHM_NAME_EXECUTING "/Executing"
#define NUM_EXECUTING MAX_STRING_LENGTH
/*-----------------------------------------------
ORDER_PATH
-----------------------------------------------*/
#define SHM_NAME_ORDER_PATH "/OrderPath"
#define NUM_ORDER_PATH MAX_STRING_LENGTH
/*-----------------------------------------------
FLAGS
-----------------------------------------------*/
#define SHM_NAME_FLAGS "/Flags"
#define NUM_FLAGS 14
#define FLAG_KILL 0
#define FLAG_START 1
#define FLAG_KEY 2
#define FLAG_IMU_RESET 3
#define FLAG_GND_ENCODER_RESET 4
#define FLAG_OPERATE 5
#define FLAG_COLLECT 6
#define FLAG_SHEET 7
#define FLAG_BATH1 8
#define FLAG_BATH2 9
#define FLAG_FIELD 10
#define FLAG_EMERGENCY 11
#define FLAG_MAIN_END 12
#define FLAG_PROGRAM 13
/*-----------------------------------------------
COM_STATE
-----------------------------------------------*/
#define SHM_NAME_COM_STATE "/ComState"
#define NUM_COM_STATE 10
#define COM_STATE_ANGLE 0
#define COM_STATE_CHASSIS 1
#define COM_STATE_EMERGENCY 2
#define COM_STATE_GND_ENCODER 3
#define COM_STATE_IMU 4
#define COM_STATE_LIMIT 5
#define COM_STATE_LINE_TRACER 6
#define COM_STATE_SERVO 7
#define COM_STATE_SWITCH 8
#define COM_STATE_MOTOR 9
/*-----------------------------------------------
SWITCH
-----------------------------------------------*/
#define SHM_NAME_SWITCH "/Switch"

#define NOShm 15
typedef enum
{
    SHM_ACTUATOR_VALUE,
    SHM_ACTUATOR_COMPLETE,
    SHM_LINE,
    SHM_PRE_LINE,
    SHM_LIMIT,
    SHM_SPIN,
    SHM_TARGET_SPIN,
    SHM_SPIN_OFFSET,
    SHM_MOTION,
    SHM_ODOMETRY,
    SHM_EXECUTING,
    SHM_ORDER_PATH,
    SHM_FLAGS,
    SHM_COM_STATE,
    SHM_SWITCH,
} SHM_KIND;

/*-----------------------------------------------
*
* プロトタイプ宣言
*
-----------------------------------------------*/
void *shmOpen(SHM_KIND shm_kind, int shm_size, const char *mem_name);
void shmClose(void);
void shmClose(SHM_KIND shm_kind);

#endif