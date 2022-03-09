/**
 * @file comKey.c
 * @brief 
 * @author ZheWana
 * @date 2022/3/6 006
 */

#include "comKey.h"

comkey_t key0, key1;

__attribute__((weak)) void ComKey_SyncValue(comkey_t *key) {
    //if your key was pressed,"key->val" should be 1.
    if (key == &key0)
        key->val = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    if (key == &key1) {
        key->val = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    }

    //IMPORTANT！！！DO NOT MODIFIED!!!
    key->preVal = key->val;
}

__attribute__((weak)) void ComKey_LongHoldCallback(comkey_t *key, uint32_t holdTime) {
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("hold %ldms\n", holdTime);
}

__attribute__((weak)) void ComKey_MultipleClickCallback(comkey_t *key, uint8_t times) {
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("click: %d\n", times);
}

__attribute__((weak)) void ComKey_KeyReleaseCallback(comkey_t *key) {
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("keyRelease!\n");
}

__attribute__((weak)) void ComKey_KeyPressCallback(comkey_t *key) {
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("keyPress!\n");
}

static pcomkey_t head = NULL, tail = NULL;
static uint8_t ITPeriod = 1;

void ComKey_Init(comkey_t *key, int pollingPeriod) {
    ITPeriod = pollingPeriod;
    key->state = Release;
    key->next = NULL;

    if (head == NULL) {
        head = key;
        tail = key;
    } else {
        tail->next = key;
        tail = tail->next;
    }
}

void ComKey_Handler() {

    for (pcomkey_t key = head; key != NULL; key = key->next) {
        //键值同步
        ComKey_SyncValue(key);

        //按下计时
        if (!key->val) {
            if (key->state == LongHold) {
                key->holdTime = key->preTimer;
            }
            key->preTimer = 0;
        }
        if (key->preVal & key->val) {
            key->preTimer += ITPeriod;
        }
        //间隔计时
        if (key->state == MultiClick) {
            key->intervalTimer += ITPeriod;
        } else {
            key->intervalTimer = 0;
        }

        //事件生成
        switch (key->state) {
            case Release:
                key->clickCnt = 0;

                if (key->val) {
                    key->state = PrePress;
                }
                break;
            case PrePress:

                if (!key->val) {
                    key->state = Release;
                } else if (key->preTimer > COMKEY_ClickThreshold) {
                    key->state = Prelong;
                    ComKey_KeyPressCallback(key);
                }
                break;
            case Prelong:

                if (!key->val) {
                    key->state = MultiClick;
                    key->clickCnt++;
                } else if (key->preTimer > COMKEY_HoldThreshold) {
                    key->state = LongHold;
                }
                break;
            case LongHold:

                if (!key->val) {
                    key->state = Release;
                    ComKey_LongHoldCallback(key, key->holdTime);
                    ComKey_KeyReleaseCallback(key);
                }
                break;
            case MultiClick:

                if (key->intervalTimer > COMKEY_IntervalVal) {
                    key->state = Release;
                    ComKey_MultipleClickCallback(key, key->clickCnt);
                    ComKey_KeyReleaseCallback(key);
                } else if (key->preTimer > COMKEY_ClickThreshold) {
                    key->state = Prelong;
                }
                break;

            default:
#ifdef USE_HAL_DRIVER
                Error_Handler();
#endif//USE_HAL_DRIVER
                break;
        }
    }

}