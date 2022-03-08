/**
 * @file comKey.c
 * @brief 
 * @author ZheWana
 * @date 2022/3/6 006
 */

#include "comKey.h"

__attribute__((weak)) void ComKey_SyncValue(comkey_t *key) {
    //if your key was pressed,"key->val" should be 1.
    key->val = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

    //IMPORTANT！！！DO NOT MODIFIED!!!
    key->preVal = key->val;
}

__attribute__((weak)) void ComKey_LongHoldCallback(comkey_t *key, uint32_t holdTime) {
    printf("hold %ldms\n", holdTime);
}

__attribute__((weak)) void ComKey_MultipleClickCallback(comkey_t *key, uint8_t times) {
    printf("click: %d\n", times);
}

__attribute__((weak)) void ComKey_KeyReleaseCallback(comkey_t *key) {
    printf("keyRelease!\n");
}

__attribute__((weak)) void ComKey_KeyPressCallback(comkey_t *key) {
    printf("keyPress!\n");
}

void ComKey_Init(comkey_t *key, int pollingPeriod) {
    ITPeriod = pollingPeriod;
    key->state = KeyState_Release;
}

void ComKey_Handler(comkey_t *key) {
    //键值同步
    ComKey_SyncValue(key);

    //按下计时
    if (!key->val) {
        if (key->state == KeyState_LongHold) {
            key->holdTime = key->preTimer;
        }
        key->preTimer = 0;
    }
    if (key->preVal & key->val) {
        key->preTimer += ITPeriod;
    }
    //间隔计时
    if (key->state == KeyState_MultipleClick) {
        key->intervalTimer += ITPeriod;
    } else {
        key->intervalTimer = 0;
    }

    //事件生成
    switch (key->state) {
        case KeyState_Release:
            key->clickCnt = 0;

            if (key->val) {
                key->state = KeyState_PrePress;
            }
            break;
        case KeyState_PrePress:

            if (!key->val) {
                key->state = KeyState_Release;
            } else if (key->preTimer > COMKEY_ClickThreshold) {
                key->state = KeyState_PreLong;
                ComKey_KeyPressCallback(key);
            }
            break;
        case KeyState_PreLong:

            if (!key->val) {
                key->state = KeyState_MultipleClick;
                key->clickCnt++;
            } else if (key->preTimer > COMKEY_HoldThreshold) {
                key->state = KeyState_LongHold;
            }
            break;
        case KeyState_LongHold:

            if (!key->val) {
                key->state = KeyState_Release;
                ComKey_LongHoldCallback(key, key->holdTime);
                ComKey_KeyReleaseCallback(key);
            }
            break;
        case KeyState_MultipleClick:

            if (key->intervalTimer > COMKEY_IntervalVal) {
                key->state = KeyState_Release;
                ComKey_MultipleClickCallback(key, key->clickCnt);
                ComKey_KeyReleaseCallback(key);
            } else if (key->preTimer > COMKEY_ClickThreshold) {
                key->state = KeyState_PreLong;
            }
            break;

        default:
#ifdef USE_HAL_DRIVER
            Error_Handler();
#endif//USE_HAL_DRIVER
            break;
    }

}