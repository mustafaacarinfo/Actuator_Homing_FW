#include "actuator.h"
#include "actuator_cfg.h"

void Actuator_InitObj(Actuator_t *hActuator)
{
    hActuator->state = ACTUATOR_STATE_IDLE;
    hActuator->speedExtendRatio  = ACTUATOR_CFG_SPEED_RATIO_EXTEND;
    hActuator->speedRetractRatio = ACTUATOR_CFG_SPEED_RATIO_RETRACT;
    hActuator->startCommand = false;
    hActuator->totalTravelTime = 0;
    hActuator->middleTravelTime = 0;

    ACTUATOR_CMD_STOP();
}

void Actuator_Start(Actuator_t *hActuator)
{
    if (hActuator->state != ACTUATOR_STATE_ERROR)
    {
        hActuator->startCommand = true;
    }
}

void Actuator_Stop(Actuator_t *hActuator)
{
    hActuator->state = ACTUATOR_STATE_IDLE;
    hActuator->startCommand = false;

    ACTUATOR_CMD_STOP();
}

void Actuator_Process(Actuator_t *hActuator)
{
    uint32_t currentTick = ACTUATOR_GET_TICK();

    switch (hActuator->state)
    {
        case ACTUATOR_STATE_IDLE:
            if (hActuator->startCommand == true) {
                hActuator->startCommand = false;
                hActuator->state = ACTUATOR_STATE_EXTEND;
            }
            break;

        case ACTUATOR_STATE_EXTEND:
            ACTUATOR_CMD_MOVE_FWD();

            if (ACTUATOR_READ_SW1() == true) {
                ACTUATOR_CMD_STOP();
                hActuator->startTime = currentTick;
                hActuator->state = ACTUATOR_STATE_RETRACT_MEASURE;
            }
            break;

        case ACTUATOR_STATE_RETRACT_MEASURE:
            ACTUATOR_CMD_MOVE_BWD();

            if (ACTUATOR_READ_SW2() == true) {
                ACTUATOR_CMD_STOP();
                hActuator->totalTravelTime = currentTick - hActuator->startTime;
                hActuator->state = ACTUATOR_STATE_CALCULATE;
            }
            break;

        case ACTUATOR_STATE_CALCULATE:
            {
                uint32_t t_total = hActuator->totalTravelTime;
                uint32_t v_ext   = hActuator->speedExtendRatio;
                uint32_t v_ret   = hActuator->speedRetractRatio;

                if (v_ext == 0) v_ext = 1;

                hActuator->middleTravelTime = (t_total * v_ret) / (2 * v_ext);

                hActuator->startTime = currentTick;
                hActuator->state = ACTUATOR_STATE_MOVE_TO_MID;
            }
            break;

        case ACTUATOR_STATE_MOVE_TO_MID:
            ACTUATOR_CMD_MOVE_FWD();

            if ((currentTick - hActuator->startTime) >= hActuator->middleTravelTime) {
                ACTUATOR_CMD_STOP();
                hActuator->state = ACTUATOR_STATE_DONE;
            }
            break;

        case ACTUATOR_STATE_DONE:
            break;

        case ACTUATOR_STATE_ERROR:
        default:
            ACTUATOR_CMD_STOP();
            break;
    }
}
