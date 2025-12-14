#include "actuator_selftest.h"
#include "actuator_selftest_ram.h"

#if (ACTUATOR_TEST_MODE_ENABLED == 1)

void ActuatorSelfTest_Init(void) {
#if (ACTUATOR_TEST_MODE_ENABLED == 1)
    testResult.currentStage = TEST_STAGE_IDLE;
    testResult.isPassed = false;
    testResult.failCode = 0UL;
#endif
}

ActuatorTestResult_t Actuator_RunValidation(Actuator_t *hActuator) {
	if (hActuator == NULL) {
		testResult.isPassed = false;
		testResult.failCode = (uint32_t) ACT_TEST_FAIL_UNEXPECTED_SEQUENCE;
		testResult.currentStage = TEST_STAGE_COMPLETE;
		return testResult;
	}

	/* If actuator entered error -> test failed */
	if (hActuator->state == ACTUATOR_STATE_ERROR) {
		testResult.isPassed = false;
		testResult.failCode = hActuator->faultInfo.faultMask;
		testResult.currentStage = TEST_STAGE_COMPLETE;
		return testResult;
	}

	switch (testResult.currentStage) {
	case TEST_STAGE_IDLE: {
		/* Test starts when actuator leaves IDLE */
		if (hActuator->state != ACTUATOR_STATE_IDLE) {
			testResult.isPassed = false; /* not decided yet */
			testResult.failCode = (uint32_t) ACT_TEST_FAIL_NONE;

			testResult.measuredTime_A_to_B = 0UL;
			testResult.calculatedMiddleTime = 0UL;

			/* If it starts by going to A first */
			if (hActuator->state == ACTUATOR_STATE_EXTEND) {
				testResult.currentStage = TEST_STAGE_WAITING_FOR_LIMIT_A;
			}
			/* If already at A, it may directly start retract measure */
			else if (hActuator->state == ACTUATOR_STATE_RETRACT_MEASURE) {
				testResult.currentStage = TEST_STAGE_WAITING_FOR_LIMIT_B;
			} else {
				testResult.failCode =
						(uint32_t) ACT_TEST_FAIL_UNEXPECTED_SEQUENCE;
				testResult.currentStage = TEST_STAGE_COMPLETE;
			}
		}
		break;
	}

	case TEST_STAGE_WAITING_FOR_LIMIT_A: {
		/* Expected path: EXTEND -> (WAIT) -> RETRACT_MEASURE */
		if (hActuator->state == ACTUATOR_STATE_RETRACT_MEASURE) {
			testResult.currentStage = TEST_STAGE_WAITING_FOR_LIMIT_B;
		} else {
			/* still moving forward or waiting: do nothing */
		}
		break;
	}

	case TEST_STAGE_WAITING_FOR_LIMIT_B: {
			/* 1. Ölçülen süreyi al (Bunu mecbur Actuator'dan alacağız, çünkü zamanı o tutuyor) */
			if ((testResult.measuredTime_A_to_B == 0UL) && (hActuator->totalTravelTime != 0UL)) {
				testResult.measuredTime_A_to_B = hActuator->totalTravelTime;
			}

			/* 2. Actuator hesaplamayı bitirdi mi? */
			if (hActuator->state == ACTUATOR_STATE_MOVE_TO_MID) {

				/* Actuator'ın bulduğu sonucu al */
				uint32_t moduleResult = hActuator->middleTravelTime;
				testResult.calculatedMiddleTime = moduleResult;

				/* --- KRİTİK DÜZELTME: KENDİ HESABIMIZI YAPIYORUZ --- */
				/* Formül: (T_total * V_ret) / (2 * V_ext) */
				/* Not: Test modülü en güvenli yolu (64-bit) kullanarak doğrulasın */

				uint64_t calcNum = (uint64_t)testResult.measuredTime_A_to_B * (uint64_t)ACTUATOR_CFG_SPEED_RATIO_RETRACT;
				uint64_t calcDenom = 2ULL * (uint64_t)ACTUATOR_CFG_SPEED_RATIO_EXTEND;
				uint32_t expectedResult = (uint32_t)(calcNum / calcDenom);

				/* 3. KARŞILAŞTIRMA (VALIDATION) */

				/* Önce 0 kontrolü (Eski kontrol) */
				if (moduleResult == 0UL) {
					testResult.isPassed = false;
					testResult.failCode = (uint32_t) ACT_TEST_FAIL_MIDDLE_TIME_ZERO;
					testResult.currentStage = TEST_STAGE_COMPLETE;
				}
				/* Şimdi matematiksel doğruluk kontrolü */
				else if (moduleResult != expectedResult) {
					/* Actuator yanlış hesaplamış! Bug yakaladık! */
					testResult.isPassed = false;
					testResult.failCode = (uint32_t) ACT_TEST_FAIL_CALCULATION_MISMATCH; // Yeni bir hata kodu
					/* Debug için breakpoint koyarsan: expectedResult vs moduleResult farkını görürsün */
					testResult.currentStage = TEST_STAGE_COMPLETE;
				}
				else {
					/* Hesap doğru, orta noktaya gitmesini beklemeye devam et */
					testResult.currentStage = TEST_STAGE_WAITING_FOR_MID;
				}
			}
			break;
	}
	case TEST_STAGE_WAITING_FOR_MID: {
		if (hActuator->state == ACTUATOR_STATE_DONE) {
			testResult.isPassed = true;
			testResult.failCode = (uint32_t) ACT_TEST_FAIL_NONE;
			testResult.currentStage = TEST_STAGE_COMPLETE;
		}
		break;
	}

	case TEST_STAGE_COMPLETE:
	default: {
		/* Optional: allow re-arm after returning to IDLE */
		if (hActuator->state == ACTUATOR_STATE_IDLE) {
			testResult.currentStage = TEST_STAGE_IDLE;
		}
		break;
	}
	}
	return testResult;
}

#endif
