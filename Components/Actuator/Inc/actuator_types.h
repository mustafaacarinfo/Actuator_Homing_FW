#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ACTUATOR_STATE_IDLE,
    ACTUATOR_STATE_EXTEND,
    ACTUATOR_STATE_RETRACT_MEASURE,
    ACTUATOR_STATE_CALCULATE,
    ACTUATOR_STATE_MOVE_TO_MID,
    ACTUATOR_STATE_DONE,
    ACTUATOR_STATE_ERROR
} ActuatorState_t;

typedef enum {
	ACTUATOR_DIR_FORWARD,
	ACTUATOR_DIR_BACKWARD
} ActuatorDir_t;

typedef struct {
    ActuatorState_t state;
    bool startCommand;

    uint32_t startTime;
    uint32_t totalTravelTime;
    uint32_t middleTravelTime;

    uint16_t speedExtendRatio;
    uint16_t speedRetractRatio;

} Actuator_t;
