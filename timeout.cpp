
#include "timeout.h"

uint32_t u32TimeSaved;
uint32_t u32TimeoutSaved_ms;

void vdTimeoutSet(uint32_t u32Timeout_ms)
{
	u32TimeSaved = millis();
  u32TimeoutSaved_ms = u32Timeout_ms;
}

void vdTimeoutReload(void)
{
  vdTimeoutSet(u32TimeoutSaved_ms);
}

bool bTimeoutExpired(void)
{
  return (millis() - u32TimeSaved >= u32TimeoutSaved_ms);
}
