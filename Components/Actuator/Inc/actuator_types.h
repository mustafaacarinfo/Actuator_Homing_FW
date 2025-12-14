#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "debounce.h"

/* API LIST */
typedef enum {
    ACT_API_ID_NONE = 0,
    ACT_API_ID_INIT_OBJ,
    ACT_API_ID_START,
    ACT_API_ID_STOP,
    ACT_API_ID_ACTUATOR_PROCESS,
	NUMBER_OF_API
} ActuatorApiId_t;

typedef enum {
    ACTUATOR_STATE_IDLE,
    ACTUATOR_STATE_EXTEND,
    ACTUATOR_STATE_RETRACT_MEASURE,
    ACTUATOR_STATE_CALCULATE,
	ACTUATOR_STATE_DIR_CHANGE_WAIT,
    ACTUATOR_STATE_MOVE_TO_MID,
    ACTUATOR_STATE_DONE,
    ACTUATOR_STATE_ERROR
} ActuatorState_t;

typedef enum {
	ACTUATOR_DIR_FORWARD,
	ACTUATOR_DIR_BACKWARD
} ActuatorDir_t;

typedef struct {
    uint32_t faultMask;
    ActuatorApiId_t faultSourceApiId;
}ActuatorFaultCtx_t;

typedef struct {
    ActuatorState_t state;
    bool startCommand;

    uint32_t startTime;
    uint32_t totalTravelTime;
    uint32_t middleTravelTime;

    uint16_t speedExtendRatio;
    uint16_t speedRetractRatio;

    uint32_t       waitStartTick;
    ActuatorState_t waitNextState;

    /* FOR DEBOUNCE */
    Debounce_t limitA_Debounce;
    Debounce_t limitB_Debounce;

    ActuatorFaultCtx_t faultInfo;

} Actuator_t;

typedef enum
{
    ACTUATOR_ENDPOINT_LIMIT_A = 0,
    ACTUATOR_ENDPOINT_LIMIT_B
} ActuatorEndpoint_t;
