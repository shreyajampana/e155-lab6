// STM32L432KC_FLASH.c
// Source code for FLASH functions

#include "/Users/shreyajampana/e155-lab6/mcu/lib/STM32L432KC_FLASH.h"

void configureFlash() {
  FLASH->ACR |= FLASH_ACR_LATENCY_4WS;
  FLASH->ACR |= FLASH_ACR_PRFTEN;
}