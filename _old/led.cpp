#include "api/Common.h"
#include "led.h"

#include "pins.h"

#include "libSm.h"

//#include <Adafruit_DotStar.h>

//Adafruit_DotStar rgbLed(1, 41, 40, DOTSTAR_BGR);

typedef enum {
  st_off,
  st_blink_on,
  st_blink_off,
} eSt_t;

sm_t sm;

void vdLed_Init(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  pinMode(LED_R, OUTPUT);   
  pinMode(LED_G, OUTPUT);  
  pinMode(LED_B, OUTPUT);  
  sm_initInst(&sm, st_off, 0);
}

void vdLed_Task(void)
{
  sm_processEnteringSt(&sm);
  switch(sm)
  {
    default:
      case st_off: {
        if(sm_isEnteringFirstSt(&sm)) {
          vdLed_Off();
        }
        break;
      }

    case st_blink_on:
      if(sm_isEnteringFirstSt(&sm))
      {
        setColor(currentColor);
        drvTime_startTempo_1ms(&tempo, tempoOn_ms);
      }
      else if(drvTime_isElapsed(&tempo)) {
        sm.nextSt = ledSt_blink_off;
      }
      break; 
  }
}

void vdLed_Off(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixRed(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixBlue(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void vdLed_FixPurple(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, HIGH);
}

void vdLed_FixGreen(void)
{
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}

void vdLed_FixOrange(void)
{
  digitalWrite(LED_R, HIGH);
  digitalWrite(LED_G, HIGH);
  digitalWrite(LED_B, LOW);
}