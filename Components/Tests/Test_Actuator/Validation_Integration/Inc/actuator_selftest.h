#pragma once
#include "actuator.h"
#include "actuator_cfg.h"

#if (ACTUATOR_TEST_MODE_ENABLED == 1)

typedef enum {
    TEST_STAGE_IDLE,
    TEST_STAGE_WAITING_FOR_LIMIT_A,
    TEST_STAGE_WAITING_FOR_RETRACT,
    TEST_STAGE_WAITING_FOR_LIMIT_B,
    TEST_STAGE_WAITING_FOR_MID,
    TEST_STAGE_COMPLETE
} TestStage_t;

typedef enum
{
    ACT_TEST_FAIL_NONE = 0,
    ACT_TEST_FAIL_FAULT_FROM_ACTUATOR = 1,
    ACT_TEST_FAIL_MIDDLE_TIME_ZERO = 2,
    ACT_TEST_FAIL_UNEXPECTED_SEQUENCE = 3
} ActuatorTestFail_t;

typedef struct {
    TestStage_t currentStage;
    bool isPassed;
    uint32_t failCode;

    uint32_t measuredTime_A_to_B;
    uint32_t calculatedMiddleTime;
} ActuatorTestResult_t;

void ActuatorSelfTest_Init(void);
ActuatorTestResult_t Actuator_RunValidation(Actuator_t *hActuator);
#endif
