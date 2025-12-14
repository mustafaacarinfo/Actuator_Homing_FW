#include "actuator_fault.h"

void Actuator_RaiseFault(Actuator_t * const hActuator, uint32_t faultBit, ActuatorApiId_t sourceApi)
{
    if (hActuator == NULL)
    {
        return;
    }

    hActuator->faultInfo.faultMask |= faultBit;
    hActuator->faultInfo.faultSourceApiId = sourceApi;
    ACTUATOR_CMD_STOP();
    hActuator->state = ACTUATOR_STATE_ERROR;
}

bool Actuator_ResetFaults(Actuator_t *const hActuator) {
    if (hActuator == NULL) { return false; }
    if (hActuator->state != ACTUATOR_STATE_ERROR) { return false; }
    hActuator->faultInfo.faultMask = ACT_FAULT_NONE;
    hActuator->state = ACTUATOR_STATE_IDLE;

    return true;
}
