#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "actuator.h"
#include "actuator_cfg.h"

/* Fault bits */
#define ACT_FAULT_NONE                  		(0UL)
#define ACT_FAULT_BOTH_LIMITS_ACTIVE    		(1UL << 0)
#define ACT_FAULT_TIMEOUT_LIMIT_A       		(1UL << 1)
#define ACT_FAULT_TIMEOUT_LIMIT_B       		(1UL << 2)
#define ACT_FAULT_TIMEOUT_TO_MID        		(1UL << 3)
#define ACT_FAULT_INVALID_MEASUREMENT   		(1UL << 4)
#define ACT_FAULT_INVALID_CONFIGURATION 		(1UL << 5)
#define ACT_FAULT_UNEXPECTED_LIMIT_DURING_MOVE  (1UL << 6)

void Actuator_RaiseFault(Actuator_t * const hActuator, uint32_t faultBit, ActuatorApiId_t sourceApi);
bool Actuator_ResetFaults(Actuator_t *const hActuator);

