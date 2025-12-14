#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum
{
    DEBOUNCE_EVENT_NONE = 0,
    DEBOUNCE_EVENT_ACTIVATED,
    DEBOUNCE_EVENT_DEACTIVATED
} DebounceEvent_t;

typedef struct
{
    bool           stableActive;
    bool           lastRawActive;
    uint32_t       lastRawChangeTick;
    uint32_t       debounceTimeMs;
    DebounceEvent_t pendingEvent;
} Debounce_t;

void Debounce_Init(Debounce_t * const debounce,
                   bool initialRawActive,
                   uint32_t debounceTimeMs,
                   uint32_t nowTick);

void Debounce_Update(Debounce_t * const debounce,
                     bool rawActive,
                     uint32_t nowTick);

bool Debounce_GetStableActive(const Debounce_t * const debounce);
