# CommonKey

基础按键处理库，实现了一般按键的单点、连点、长按检测。可进行多按键的封装，采用回调的方式与主程序通讯。

## 使用说明

要使用[CommonKey][2]按键检测封装，你需要在你的工程中做出如下处理：

* 将文件加入你的工程路径中，按需修改时间阈值宏：

>    `COMKEY_ClickThreshold`：单击时间阈值，短于此时间视为抖动
>
>    `COMKEY_HoldThreshold`：长按时间阈值，长于此时间视为长按
>
>    `COMKEY_HoldTriggerThreshold`：长按触发阈值，长按时每隔该事件触发一次按键
>
>    `COMKEY_IntervalVal`：间隔时间阈值，短于此时间视为连按

* 为您的每个按键定义一个`comkey_t`句柄

* 为**每个按键**调用`ComKey_Init`初始化函数，参数为**按键句柄**和**检测时间**

* 改写`ComKey_SyncValue`函数

* 根据需要重写回调函数：

>    `ComKey_FirstLongTriggerCallback`：**首次长按触发回调函数**，参数为**按键句柄**
>
>    `ComKey_LongHoldCallback`：**长按回调函数**，参数为**按键句柄**
>
>    `ComKey_HoldTriggerCallback`：**长按触发回调函数**，参数为**按键句柄**
>
>    `ComKey_MultipleClickCallback`：**连点回调函数**，参数为**按键句柄**
>
>    `ComKey_KeyReleaseCallback`：**按键松开回调函数**，参数为**按键句柄**
>
>    `ComKey_KeyPressCallback`：**按键按下回调函数**，参数为**按键句柄**

* 以一个固定的间隔调用`ComKey_Handler`函数

* 测试实际效果

***

## 一些说明与示例

此部分提供了一些对于接口函数、回调函数等的说明，以及简单的使用示例。

### 接口函数的移植

库内部不提供对按键触发电平的检测，移植时**请务必**保证按键触发时传递到`key->val`的值为1；

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

此外**请不要删除**最后一句代码，这将影响程序的正常运行。

```C
void ComKey_SyncValue(comkey_t *key) {
    //if your key was pressed,"key->val" should be 1.
    if (key == &key0) {
        key->val = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);
    }
    if (key == &key1) {
        key->val = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
    }

    //IMPORTANT！！！DO NOT MODIFIED!!!
    key->preVal = key->val;
}
```

### 首次长按触发回调函数

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

该回调函数为进入长按状态的信号，此函数被调用意味着您的按键**进入长按状态**。

```C
__attribute__((weak)) void ComKey_FirstLongTriggerCallback(comkey_t* key)
{
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("was triggerred!\n", key->holdTime);
}
```

### 长按回调函数

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

此外，您可以通过按键句柄的`holdTime`成员来获取**当次长按的持续时间**（ms）供您使用.

```C
void ComKey_LongHoldCallback(comkey_t *key) {
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("hold %ldms\n", key->holdTime);
}
```

### 长按触发回调函数

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

当您采用按键单击进行类似数据加减的任务时，若想**通过长按来实现多次单击的效果**，请使用此回调函数；

该函数将会以`COMKEY_HoldTriggerThreshold`的间隔被调用。

```C
void ComKey_HoldTriggerCallback(comkey_t *key) {
    if (key == &key0)
        printf("key0: HoldTrigger\n");
    if (key == &key1) {
        printf("key1: HoldTrigger\n");
    }
}
```

### 连续点击回调函数

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

当您多次连续进行点击**后**，会触发该回调函数，连续点击间隔时间阈值可通过`COMKEY_IntervalVal`宏来进行修改；

此外，您可以通过按键句柄的`clickCnt`成员来获得当次**连按的次数**以供您使用。

```C
void ComKey_MultipleClickCallback(comkey_t *key) {
    if (key == &key0)
        printf("key0: ");
    if (key == &key1) {
        printf("key1: ");
    }
    printf("click: %d\n", key->clickCnt);
}
```

### 按键按下回调函数

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

任何按键按下的事件都会触发该回调函数。

```C
void ComKey_KeyPressCallback(comkey_t *key) {
    if (key == &key0) {
        printf("key0: ");
    }
    if (key == &key1) {
        printf("key1: ");
    }
    printf("keyPress!\n");
}
```

### 按键释放回调函数

如果您有多个按键，请通过**您自己定义的按键句柄**来区分您的触发按键；

此外，您可以通过按键句柄的`state`成员的两个枚举值：

`LongHold`和`MultiClick`

来区分当次释放按键的事件是**长按**还是**连击**（单击）。

```C
void ComKey_KeyReleaseCallback(comkey_t *key) {
    if (key == &key0) {
        printf("key0: ");
    }
    if (key == &key1) {
        printf("key1: ");
    }

    if (key->state == LongHold) {
        printf("LongHold");
    } else if (key->state == MultiClick) {
        printf("Click");
    }
    printf(" Release!\n");
}
```



***

## 实现思路

思路其实很简单，核心部分是两个计时器：**按下计时器**和**间隔计时器**。通过有限状态机的方式来实现对各个功能的检测，状态转移图大概如下：

![按键状态转移][3]

仅作分享，欢迎提出优化意见以及建议🥰


[1]: https://zhewana.cn/usr/uploads/2022/03/2707392082.png
[2]: https://github.com/Zhewana/CommonKey
[3]: https://zhewana.cn/usr/uploads/2022/03/3781171879.png
