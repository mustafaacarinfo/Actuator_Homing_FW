#include "debounce.h"

void Debounce_Init(Debounce_t *const debounce, bool initialRawActive, uint32_t debounceTimeMs, uint32_t nowTick) {
	if (debounce == NULL) {
		return;
	}

	if (debounceTimeMs == 0U) {
		debounceTimeMs = 1U;
	}

	debounce->stableActive = initialRawActive;
	debounce->lastRawActive = initialRawActive;
	debounce->lastRawChangeTick = nowTick;
	debounce->debounceTimeMs = debounceTimeMs;
	debounce->pendingEvent = DEBOUNCE_EVENT_NONE;
}

void Debounce_Update(Debounce_t *const debounce, bool rawActive, uint32_t nowTick) {
	uint32_t elapsedMs;

	if (debounce == NULL) {
		return;
	}

	if (rawActive != debounce->lastRawActive) {
		debounce->lastRawActive = rawActive;
		debounce->lastRawChangeTick = nowTick;
	}

	elapsedMs = (uint32_t) (nowTick - debounce->lastRawChangeTick);

	if ((elapsedMs >= debounce->debounceTimeMs)
			&& (debounce->stableActive != debounce->lastRawActive)) {
		debounce->stableActive = debounce->lastRawActive;

		/* latch only one event until it is consumed */
		if (debounce->pendingEvent == DEBOUNCE_EVENT_NONE) {
			debounce->pendingEvent =
					(debounce->stableActive == true) ?
							DEBOUNCE_EVENT_ACTIVATED :
							DEBOUNCE_EVENT_DEACTIVATED;
		}
	}
}

bool Debounce_GetStableActive(const Debounce_t *const debounce) {
	if (debounce == NULL) {
		return false;
	}

	return debounce->stableActive;
}
