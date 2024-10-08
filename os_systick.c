/**************************************************************************//**
 * @file     os_systick.c
 * @brief    CMSIS OS Tick SysTick implementation
 * @version  V1.0.1
 * @date     29. November 2017
 ******************************************************************************/
/*
 * Copyright (c) 2017-2017 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <rtthread.h>
#include "cmsis_rtthread.h"
#include "os_tick.h"
#include "board.h"

#ifdef  SysTick

#ifndef SYSTICK_IRQ_PRIORITY
#define SYSTICK_IRQ_PRIORITY    0xFFU
#endif

static uint8_t PendST __attribute__((section(".bss.os")));

// Setup OS Tick.
RT_WEAK int32_t OS_Tick_Setup (uint32_t freq, IRQHandler_t handler) {
  uint32_t load;
  (void)handler;

  if (freq == 0U) {
    //lint -e{904} "Return statement before end of function"
    return (-1);
  }

  load = (SystemCoreClock / freq) - 1U;
  if (load > 0x00FFFFFFU) {
    //lint -e{904} "Return statement before end of function"
    return (-1);
  }

  NVIC_SetPriority(SysTick_IRQn, SYSTICK_IRQ_PRIORITY);

  SysTick->CTRL =  SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
  SysTick->LOAD =  load;
  SysTick->VAL  =  0U;

  PendST = 0U;

  return (0);
}

/// Enable OS Tick.
RT_WEAK void OS_Tick_Enable (void) {

  if (PendST != 0U) {
    PendST = 0U;
    SCB->ICSR = SCB_ICSR_PENDSTSET_Msk;
  }

  SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}

/// Disable OS Tick.
RT_WEAK void OS_Tick_Disable (void) {

  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

  if ((SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) != 0U) {
    SCB->ICSR = SCB_ICSR_PENDSTCLR_Msk;
    PendST = 1U;
  }
}

// Acknowledge OS Tick IRQ.
RT_WEAK void OS_Tick_AcknowledgeIRQ (void) {
  (void)SysTick->CTRL;
}

// Get OS Tick IRQ number.
RT_WEAK int32_t  OS_Tick_GetIRQn (void) {
  return ((int32_t)SysTick_IRQn);
}

// Get OS Tick clock.
RT_WEAK uint32_t OS_Tick_GetClock (void) {
  return (SystemCoreClock);
}

// Get OS Tick interval.
RT_WEAK uint32_t OS_Tick_GetInterval (void) {
  return (SysTick->LOAD + 1U);
}

// Get OS Tick count value.
RT_WEAK uint32_t OS_Tick_GetCount (void) {
  uint32_t load = SysTick->LOAD;
  return  (load - SysTick->VAL);
}

// Get OS Tick overflow status.
RT_WEAK uint32_t OS_Tick_GetOverflow (void) {
  return ((SCB->ICSR & SCB_ICSR_PENDSTSET_Msk) >> SCB_ICSR_PENDSTSET_Pos);
}

#endif  // SysTick
