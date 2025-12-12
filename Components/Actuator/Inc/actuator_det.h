#pragma once

#include "actuator_cfg.h"
#include "Det.h"

#define ACTUATOR_MODULE_ID     (123U)
#define ACTUATOR_INSTANCE_ID   (0U)

typedef enum {
    ACTUATOR_API_INIT          = 0x01,
    ACTUATOR_API_START_HOMING  = 0x02,
    /* ... */
} Actuator_ApiId_t;

typedef enum {
    ACTUATOR_E_UNINIT          = 0x10,
} Actuator_ErrorId_t;

#if (ACTUATOR_DEV_ERROR_DETECT == STD_ON)

    #define Actuator_ReportError(ApiId, ErrorId) \
        Det_ReportError((uint16)ACTUATOR_MODULE_ID, (uint8)ACTUATOR_INSTANCE_ID, (uint8)(ApiId), (uint8)(ErrorId))

#else

    #define Actuator_ReportError(ApiId, ErrorId) \
        do {} while(0)

#endif
