/**
 * @file comKey.h
 * @brief 
 * @author ZheWana
 * @date 2022/3/6 006
 */

#ifndef COMKEY_H
#define COMKEY_H

#include "Debug.h"
#include "stdint.h"

#ifdef USE_HAL_DRIVER

#include "gpio.h"

#endif //USE_HAL_DRIVER

#define COMKEY_ClickThreshold           20
#define COMKEY_HoldThreshold            800
#define COMKEY_IntervalVal              200

#define KeyState_Release                1<<0
#define KeyState_PrePress               1<<1
#define KeyState_PreLong                1<<2
#define KeyState_LongHold               1<<3
#define KeyState_MultipleClick          1<<4

typedef struct {
    const uint8_t pressLevel: 1;
    uint8_t preVal: 1;
    uint8_t val: 1;

    uint8_t state;

    uint32_t holdTime;// 长按计时
    uint8_t clickCnt;// 按下计数

    uint32_t preTimer;// 按下计时
    uint32_t intervalTimer;// 放开计时
} comkey_t;

static uint8_t ITPeriod = 1;

//TODO:Rewrite the "ComKey_SyncValue" function by user.
void ComKey_SyncValue(comkey_t *key);

void ComKey_Init(comkey_t *key, int pollingPeriod);

void ComKey_Handler(comkey_t *key);

void ComKey_LongHoldCallback(comkey_t *key, uint32_t holdTime);

void ComKey_MultipleClickCallback(comkey_t *key, uint8_t times);

void ComKey_KeyReleaseCallback(comkey_t *key);

void ComKey_KeyPressCallback(comkey_t *key);

#endif //COMKEY_H
