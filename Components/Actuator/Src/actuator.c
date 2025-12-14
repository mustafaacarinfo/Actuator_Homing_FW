#include "actuator.h"
#include "actuator_cfg.h"
#include "actuator_fault.h"

void Actuator_InitObj(Actuator_t *const hActuator) {
	if (hActuator == NULL) {
		return;
	}

	hActuator->state = ACTUATOR_STATE_IDLE;
	hActuator->speedExtendRatio = ACTUATOR_CFG_SPEED_RATIO_EXTEND;
	hActuator->speedRetractRatio = ACTUATOR_CFG_SPEED_RATIO_RETRACT;
	hActuator->startCommand = false;
	hActuator->totalTravelTime = 0;
	hActuator->middleTravelTime = 0;
	hActuator->waitStartTick = 0UL;
	hActuator->waitNextState = ACTUATOR_STATE_IDLE;
	hActuator->startTime = 0UL;

	/* INIT ERROR CHECKING*/
	hActuator->faultInfo.faultMask = ACT_FAULT_NONE;

	/* INIT DEBOUNCE CHECKING*/
	uint32_t currentTick = ACTUATOR_GET_TICK();

	Debounce_Init(&hActuator->limitA_Debounce, READ_ACTUATOR_LIMIT_A_IS_ACTIVE(), ACTUATOR_CFG_DEBOUNCE_TIME_MS,currentTick);
	Debounce_Init(&hActuator->limitB_Debounce, READ_ACTUATOR_LIMIT_B_IS_ACTIVE(), ACTUATOR_CFG_DEBOUNCE_TIME_MS,currentTick);

	ACTUATOR_CMD_STOP();
}

void Actuator_Start(Actuator_t *const hActuator) {
	if (hActuator == NULL) {
		return;
	}

	if (hActuator->state != ACTUATOR_STATE_ERROR) {
		hActuator->startCommand = true;
	}
}

void Actuator_Stop(Actuator_t *const hActuator) {
	if (hActuator == NULL) {
		return;
	}

	hActuator->state = ACTUATOR_STATE_IDLE;
	hActuator->startCommand = false;
	hActuator->waitStartTick = 0UL;
	hActuator->waitNextState = ACTUATOR_STATE_IDLE;

	ACTUATOR_CMD_STOP();
}

void Actuator_Process(Actuator_t *hActuator) {
	if (hActuator == NULL) {
		return;
	}

	/* If any fault latched -> keep safe state */
	if (hActuator->faultInfo.faultMask != ACT_FAULT_NONE) {
		ACTUATOR_CMD_STOP();
		hActuator->state = ACTUATOR_STATE_ERROR;
		return;
	}

	uint32_t currentTick = ACTUATOR_GET_TICK();

	Debounce_Update(&hActuator->limitA_Debounce, READ_ACTUATOR_LIMIT_A_IS_ACTIVE(), currentTick);
	Debounce_Update(&hActuator->limitB_Debounce, READ_ACTUATOR_LIMIT_B_IS_ACTIVE(), currentTick);

	const bool limitAStable = Debounce_GetStableActive(&hActuator->limitA_Debounce);
	const bool limitBStable = Debounce_GetStableActive(&hActuator->limitB_Debounce);

	if ((limitAStable == true) && (limitBStable == true)) {
        Actuator_RaiseFault(hActuator, ACT_FAULT_BOTH_LIMITS_ACTIVE,ACT_API_ID_ACTUATOR_PROCESS);
		ACTUATOR_CMD_STOP();
		hActuator->state = ACTUATOR_STATE_ERROR;
		return;
	}

	switch (hActuator->state) {
	case ACTUATOR_STATE_IDLE: {
	    if (hActuator->startCommand == true)
	    {
	        hActuator->startCommand = false;
	        hActuator->startTime = currentTick;

	        if (limitAStable == true)
	        {
	            /* Already at A -> go toward B and measure A->B ..  */
	            hActuator->state = ACTUATOR_STATE_RETRACT_MEASURE;
	        }
	        else if (limitBStable == true)
	        {
	            /* ...  Already at B -> go toward A first (no measurement yet) */
	            hActuator->state = ACTUATOR_STATE_EXTEND;
	        }
	        else
	        {
	            /* Somewhere in between -> go to A first */
	            hActuator->state = ACTUATOR_STATE_EXTEND;
	        }
	    }
	    break;
	}
	case ACTUATOR_STATE_EXTEND: {

		ACTUATOR_CMD_MOVE_FWD();

		/* Timeout: never reached Limit A */
		if ((uint32_t) (currentTick - hActuator->startTime) > ACTUATOR_CFG_TIMEOUT_TO_LIMIT_A_MS) {
			Actuator_RaiseFault(hActuator, ACT_FAULT_TIMEOUT_LIMIT_A, ACT_API_ID_ACTUATOR_PROCESS);
			break;
		}

		if (limitAStable == true) {
			ACTUATOR_CMD_STOP();

			hActuator->waitStartTick = currentTick;
			hActuator->waitNextState = ACTUATOR_STATE_RETRACT_MEASURE;
			hActuator->state = ACTUATOR_STATE_DIR_CHANGE_WAIT;
		}
		break;
	}
	case ACTUATOR_STATE_DIR_CHANGE_WAIT: {
	    ACTUATOR_CMD_STOP();

	    if ((uint32_t)(currentTick - hActuator->waitStartTick) >= ACTUATOR_CFG_DIR_CHANGE_DEADTIME_MS)
	    {
	        hActuator->startTime = currentTick;
	        hActuator->state = hActuator->waitNextState;
	    }
	    break;
	}
	case ACTUATOR_STATE_RETRACT_MEASURE: {
		ACTUATOR_CMD_MOVE_BWD();

		/* Timeout: never reached Limit B */
		if ((uint32_t) (currentTick - hActuator->startTime) > ACTUATOR_CFG_TIMEOUT_TO_LIMIT_B_MS) {
			Actuator_RaiseFault(hActuator, ACT_FAULT_TIMEOUT_LIMIT_B, ACT_API_ID_ACTUATOR_PROCESS);
			break;
		}

		if (limitBStable == true) {
			ACTUATOR_CMD_STOP();
			hActuator->totalTravelTime = currentTick - hActuator->startTime;
			hActuator->state = ACTUATOR_STATE_CALCULATE;
		}
		break;
	}
	case ACTUATOR_STATE_CALCULATE: {
		uint32_t t_total = hActuator->totalTravelTime;
		uint32_t v_ext = hActuator->speedExtendRatio;
		uint32_t v_ret = hActuator->speedRetractRatio;

		#if (ACTUATOR_CFG_USE_U64_MATH == 1U)
		{
			const uint64_t travelTimeScaled_u64 = (uint64_t)t_total * (uint64_t)v_ret;
			const uint64_t speedScaledDivisor_u64 = 2ULL * (uint64_t)v_ext;
			const uint32_t middleTravelTimeMs = (uint32_t)(travelTimeScaled_u64 / speedScaledDivisor_u64);

			if ((middleTravelTimeMs == 0UL) || (middleTravelTimeMs > t_total))
			{
				Actuator_RaiseFault(hActuator, ACT_FAULT_INVALID_MEASUREMENT, ACT_API_ID_ACTUATOR_PROCESS);
				break;
			}

			hActuator->middleTravelTime = middleTravelTimeMs;
		}
		#else
		{
			const uint32_t speedScaledDivisor_u32 = 2UL * v_ext;

			if (t_total > (UINT32_MAX / v_ret))
			{
				Actuator_RaiseFault(hActuator, ACT_FAULT_INVALID_MEASUREMENT, ACT_API_ID_ACTUATOR_PROCESS);
				break;
			}

			hActuator->middleTravelTime = (t_total * v_ret) / speedScaledDivisor_u32;

			if ((hActuator->middleTravelTime == 0UL) || (hActuator->middleTravelTime > t_total))
			{
				Actuator_RaiseFault(hActuator, ACT_FAULT_INVALID_MEASUREMENT, ACT_API_ID_ACTUATOR_PROCESS);
				break;
			}
		}
		#endif

		hActuator->waitStartTick = currentTick;
		hActuator->waitNextState = ACTUATOR_STATE_MOVE_TO_MID;
		hActuator->state = ACTUATOR_STATE_DIR_CHANGE_WAIT;
		break;

	}
	case ACTUATOR_STATE_MOVE_TO_MID: {
	    if (limitAStable == true)
	    {
	        Actuator_RaiseFault(hActuator, ACT_FAULT_UNEXPECTED_LIMIT_DURING_MOVE, ACT_API_ID_ACTUATOR_PROCESS);
	        break;
	    }

		ACTUATOR_CMD_MOVE_FWD();

		if ((uint32_t) (currentTick - hActuator->startTime) > ACTUATOR_CFG_TIMEOUT_MOVE_TO_MID_MS) {
			Actuator_RaiseFault(hActuator, ACT_FAULT_TIMEOUT_TO_MID, ACT_API_ID_ACTUATOR_PROCESS);
			break;
		}

		if ((currentTick - hActuator->startTime) >= hActuator->middleTravelTime) {
			ACTUATOR_CMD_STOP();
			hActuator->state = ACTUATOR_STATE_DONE;
		}

		break;
	}
	case ACTUATOR_STATE_DONE:
	case ACTUATOR_STATE_ERROR:
	default:
	    ACTUATOR_CMD_STOP();
	    break;
	}
}
