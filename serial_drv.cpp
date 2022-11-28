#include "serial_drv.h"

#include "timeout.h"

void vdSerialDrv_Init(const char* strVersion)
{
  
  Serial.begin(9600);
  vdTimeoutSet(2000);
  while ((!Serial) && (!bTimeoutExpired()));
  Serial.println();
  Serial.print(F(__TIME__));
  Serial.print(F(" "));
  Serial.println(F(__DATE__));
  Serial.println(F(strVersion));
  Serial.println();
  Serial.println(F("Controller Starting"));
  
}