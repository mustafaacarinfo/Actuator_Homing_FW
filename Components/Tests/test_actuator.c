#include "actuator.h"
#include "actuator_ram.h"
#include "actuator_cfg.h"

#define ASSERT_TEST(condition)  if(!(condition)) { while(1); }

void Test_Homing_Start(void)
{
    Actuator_Init();

    Actuator_StartHoming();

    ASSERT_TEST(act_runtime_data.current_state == ACT_STATE_EXTEND);

    ASSERT_TEST(act_runtime_data.is_homing_active == true);
}
