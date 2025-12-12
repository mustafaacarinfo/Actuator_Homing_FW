#pragma once
#include "actuator_types.h"

void Actuator_InitObj(Actuator_t *hActuator);
void Actuator_Process(Actuator_t *hActuator);

void Actuator_Start(Actuator_t *hActuator);
void Actuator_Stop(Actuator_t *hActuator);
