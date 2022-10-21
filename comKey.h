/**
 * @file comKey.h
 * @author ZheWana
 * @date 2022/3/6 006
 */

#ifndef COMKEY_H
#define COMKEY_H

#include "Debug.h"
#include "stdint.h"

#ifdef USE_HAL_DRIVER

#include "gpio.h"

#endif // USE_HAL_DRIVER

#define COMKEY_ClickThreshold 20
#define COMKEY_HoldThreshold 800
#define COMKEY_IntervalVal 200
#define COMKEY_HoldTriggerThreshold 300

typedef struct comKeyTypedef {
    uint8_t preVal : 1;
    uint8_t val : 1;

    uint32_t preTimer; // 按下计时
    uint32_t intervalTimer; // 放开计时
    uint16_t triggerTimer; // 长按触发计时

    struct comKeyTypedef* next;

    enum {
        Release = 0,
        PrePress,
        Prelong,
        LongHold,
        MultiClick
    } state; // 状态枚举

    uint32_t holdTime; // 长按计时
    uint8_t clickCnt; // 按下计数
} comkey_t, *pcomkey_t;

// TODO:Rewrite the "ComKey_SyncValue" function by user.
void ComKey_SyncValue(comkey_t* key);

void ComKey_Init(comkey_t* key, int pollingPeriod);

void ComKey_Handler();

void ComKey_FirstLongTriggerCallback(comkey_t* key);

void ComKey_LongHoldCallback(comkey_t* key);

void ComKey_HoldTriggerCallback(comkey_t* key);

void ComKey_MultipleClickCallback(comkey_t* key);

void ComKey_KeyReleaseCallback(comkey_t* key);

void ComKey_KeyPressCallback(comkey_t* key);

#endif // COMKEY_H
