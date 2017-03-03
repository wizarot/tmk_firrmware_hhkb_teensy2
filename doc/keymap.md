Keymap framework - 如何自定义keymap
============================================
***注意: 这并不是最终版本的文档,偶尔会出现与源码不一致的情况,可能会很快被修改 ***

## 0. Keymap 和 层layers
**Keymap** 键位是由多个层叠加组成的, 你可以最多定义 **32(个) layers(层)** .
**Layer(层)** 是一个由 **keycodes** 组成的数组, **actions** 来定义每个按键的物理意义.
各层可以同时起作用. 层Layers 被定义标号从 0 到 31 数字越高其层越靠上.

    Keymap: 32 Layers                   Layer: Keycode matrix
    -----------------                   ---------------------
    stack of layers                     array_of_keycode[row][column]
           ____________ precedence               _______________________
          /           / | high                  / ESC / F1  / F2  / F3   ....
      31 /___________// |                      /-----/-----/-----/-----
      30 /___________// |                     / TAB /  Q  /  W  /  E   ....
      29 /___________/  |                    /-----/-----/-----/-----
       :   _:_:_:_:_:__ |               :   /LCtrl/  A  /  S  /  D   ....
       :  / : : : : : / |               :  /  :     :     :     :
       2 /___________// |               2 `--------------------------
       1 /___________// |               1 `--------------------------
       0 /___________/  V low           0 `--------------------------



### 0.1 Keymap 状态
Keymap 有两个参数状态:
**`default_layer`(默认层)** 表示一套基本的键位映射层(0-31) 中的哪层一直是有效的, **`keymap_stat`** 是 16bit 变量来控制每个点在层中的有效状态(通过16bit的每个位).

通常层 '0' 作为 `default_layer(默认层)` 当固件运行时这个默认层作为唯一有效的层而其余层是关闭状态的, 但是, 你可以在 `config.h`文件中自行配置.
来设置 `default_layer(默认层)` 这样你就可以按照你自己的需要来定义键盘例如使用德沃夏克键盘来代替Qwerty键盘键位.

    Initial state of Keymap          Change base layout              
    -----------------------          ------------------              

      31                               31
      30                               30
      29                               29
       :                                :
       :                                :   ____________
       2   ____________                 2  /           /
       1  /           /              ,->1 /___________/
    ,->0 /___________/               |  0
    |                                |
    `--- default_layer = 0           `--- default_layer = 1
         layer_state   = 0x00000001       layer_state   = 0x00000002

同时, 你可以利用层覆盖修改 `layer_state(层状态)` 来对基础层的一些键位进行覆盖来达成一些键位的小改动 例如 导航键, 功能键(F1-F12), 媒体键 或者其他多种不同的功能(actions).

    Overlay feature layer
    ---------------------      bit|status
           ____________        ---+------
      31  /           /        31 |   0
      30 /___________// -----> 30 |   1
      29 /___________/  -----> 29 |   1
       :                        : |   :
       :   ____________         : |   :
       2  /           /         2 |   0
    ,->1 /___________/  ----->  1 |   1
    |  0                        0 |   0
    |                                 +
    `--- default_layer = 1            |
         layer_state   = 0x60000002 <-'



### 0.2 层级优先关系和透明
请注意 ***层级越高,其键位优先级越高 ***, 即固件是从高向低依次查找每个层中对键位 的定义. 一旦找到对应 keycode(键位)即触发按键 ,如果不想在这层使用,可以用 **`KC_TRNS`**(透明)来代表这个层中此键位无效可向下一层查找.

详见下面的例子.


### 0.3 Keymap 例子
Keymap 是 **`keymaps[]`** C 数组,你可以在数组中定义层的键位或者 **`KEYMAP()`** c宏方法. 如果你想定义更加复杂的功能呢,可以使用 `Fn` 键位在  **`fn_actions[]`** 数组中进行定义.

这个例子是 [HHKB](http://en.wikipedia.org/wiki/Happy_Hacking_Keyboard) 键盘的键位.
例子中包含3层, 'Qwerty' 作为基础层, 'Cursor'(光标层) 和  'Mousekey'(鼠标键)作为另外两个高级层的例子.
在例子中,

 `Fn0` 是 **momentary layer switching(短暂层交换)** 控制键, 当你按住这个键不放时可以切换使用 Cursor 层.

 `Fn1` 是瞬间层的开关键,你可以点击 分号键 **';'** 来选择切换到这个层. 单词词 **'tap'(点击)** 或 **'tapping'** 意思是按下并快速放手.

 `Fn2` 是 **toggle layer switch(层切换按键)**,每按一次这个键,当前的层就会切换一次.

你可以在项目目录中的 `keymap.c` 文件里面找到更多键映射的定义方式.

    static const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
        /* 0: Qwerty
         * ,-----------------------------------------------------------.
         * |Esc|  1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|  \|  `|
         * |-----------------------------------------------------------|
         * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|Backs|
         * |-----------------------------------------------------------|
         * |Contro|  A|  S|  D|  F|  G|  H|  J|  K|  L|Fn1|  '|Enter   |
         * |-----------------------------------------------------------|
         * |Shift   |  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift |Fn0|
         * `-----------------------------------------------------------'
         *       |Gui|Alt  |Space                  |Alt  |Fn2|
         *       `-------------------------------------------'
         */
        KEYMAP(ESC, 1,   2,   3,   4,   5,   6,   7,   8,   9,   0,   MINS,EQL, BSLS,GRV, \
               TAB, Q,   W,   E,   R,   T,   Y,   U,   I,   O,   P,   LBRC,RBRC,BSPC, \
               LCTL,A,   S,   D,   F,   G,   H,   J,   K,   L,   FN1, QUOT,ENT, \
               LSFT,Z,   X,   C,   V,   B,   N,   M,   COMM,DOT, SLSH,RSFT,FN0, \
                    LGUI,LALT,          SPC,                RALT,FN2),
        /* 1: Cursor(HHKB mode)
         * ,-----------------------------------------------------------.
         * |Pwr| F1| F2| F3| F4| F5| F6| F7| F8| F9|F10|F11|F12|Ins|Del|
         * |-----------------------------------------------------------|
         * |Caps |   |   |   |   |   |   |   |Psc|Slk|Pus|Up |   |Backs|
         * |-----------------------------------------------------------|
         * |Contro|VoD|VoU|Mut|   |   |  *|  /|Hom|PgU|Lef|Rig|Enter   |
         * |-----------------------------------------------------------|
         * |Shift   |   |   |   |   |   |  +|  -|End|PgD|Dow|Shift |   |
         * `-----------------------------------------------------------'
         *      |Gui |Alt  |Space                  |Alt  |Gui|
         *      `--------------------------------------------'
         */ 
        KEYMAP(PWR, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL, \
               CAPS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,TRNS,PSCR,SLCK,PAUS,UP,  TRNS,BSPC, \
               LCTL,VOLD,VOLU,MUTE,TRNS,TRNS,PAST,PSLS,HOME,PGUP,LEFT,RGHT,ENT, \
               LSFT,TRNS,TRNS,TRNS,TRNS,TRNS,PPLS,PMNS,END, PGDN,DOWN,RSFT,TRNS, \
                    LGUI,LALT,          SPC,                RALT,RGUI),
        /* 2: Mousekey
         * ,-----------------------------------------------------------.
         * |Esc| F1| F2| F3| F4| F5| F6| F7| F8| F9|F10|F11|F12|Ins|Del|
         * |-----------------------------------------------------------|
         * |Tab  |   |   |   |   |   |MwL|MwD|MwU|MwR|   |   |   |Backs|
         * |-----------------------------------------------------------|
         * |Contro|   |   |   |   |   |McL|McD|McU|McR|   |   |Return  |
         * |-----------------------------------------------------------|
         * |Shift   |   |   |   |   |Mb3|Mb2|Mb1|Mb4|Mb5|   |Shift |   |
         * `-----------------------------------------------------------'
         *      |Gui |Alt  |Mb1                    |Alt  |   |
         *      `--------------------------------------------'
         * Mc: Mouse Cursor / Mb: Mouse Button / Mw: Mouse Wheel 
         */
        KEYMAP(ESC, F1,  F2,  F3,  F4,  F5,  F6,  F7,  F8,  F9,  F10, F11, F12, INS, DEL, \
               TAB, TRNS,TRNS,TRNS,TRNS,TRNS,WH_L,WH_D,WH_U,WH_R,TRNS,TRNS,TRNS,BSPC, \
               LCTL,TRNS,ACL0,ACL1,ACL2,TRNS,MS_L,MS_D,MS_U,MS_R,TRNS,QUOT,ENT, \
               LSFT,TRNS,TRNS,TRNS,TRNS,BTN3,BTN2,BTN1,BTN4,BTN5,SLSH,RSFT,TRNS, \
                    LGUI,LALT,          BTN1,               RALT,TRNS),
    };

    static const uint16_t PROGMEM fn_actions[] = {
        ACTION_LAYER_MOMENTARY(1),                  // FN0
        ACTION_LAYER_TAP_KEY(2, KC_SCLN),           // FN1
        ACTION_LAYER_TOGGLE(2),                     // FN2
    };




## 1. Keycode 键位码
查看文件 [`common/keycode.h`](../common/keycode.h) 或者下面的文档来了解详细键位定义. Keycode键位码 是整型 **8bit code** 值,用来定义每个按键的作用. 每个键位码都以 `KC_` 作为开头. 常见的按键如 `KC_A` 就代表最平常的键位按一下就触发这个件, 还有一些特殊按键如 `Fn` 键, 媒体控制建, 系统控制键 和鼠标键等.

 ***在 `KEYMAP()` 的宏定义中 你可以将 `KC_` 开头省略掉.*** 例如, 只用 `A` 来代替 `KC_A` 在 `KEYMAP()` 中. 一些四个字母的特殊按键 **short name** 你也一样可以在 `KEYMAP()`中省略缩短.

### 1.0 其它键
- `KC_NO` 意思是什么都不做
- `KC_TRNS` 意思是透传下一层 (在代码中是看上面一层keymap的数组)

### 1.1 常见键位
- `KC_A` 到 `KC_Z`, `KC_1` 到 `KC_0` 字母和数字键
- `KC_MINS`, `KC_EQL`, `KC_GRV`, `KC_RBRC`, `KC_LBRC`, `KC_COMM`, `KC_DOT`, `KC_BSLS`, `KC_SLSH`, `KC_SCLN`, `KC_QUOT`
- `KC_ESC`, `KC_TAB`, `KC_SPC`, `KC_BSPC`, `KC_ENT`, `KC_DEL`, `KC_INS`
- `KC_UP`, `KC_DOWN`, `KC_RGHT`, `KC_LEFT`, `KC_PGUP`, `KC_PGDN`, `KC_HOME`, `KC_END`
- `KC_CAPS`, `KC_NLCK`, `KC_SLCK`, `KC_PSCR`, `KC_PAUS`, `KC_APP`, `KC_F1` 到 `KC_F24`
- `KC_P1` 到 `KC_P0`, `KC_PDOT`, `KC_PCMM`, `KC_PSLS`, `KC_PAST`, `KC_PMNS`, `KC_PPLS`, `KC_PEQL`, `KC_PENT` for keypad.

### 1.2 Modifier
一共8个区分左右的键盘常见功能键. 

- `KC_LCTL` 和 `KC_RCTL` 左右 Control
- `KC_LSFT` 和 `KC_RSFT` 左右 Shift
- `KC_LALT` 和 `KC_RALT` 左右 Alt
- `KC_LGUI` 和 `KC_RGUI` 左右 Windows 键 或 Command 键 Mac

### 1.3 Mousekey 鼠标键
- `KC_MS_U`, `KC_MS_D`, `KC_MS_L`, `KC_MS_R` 鼠标箭头上下左右移动 cursor
- `KC_WH_U`, `KC_WH_D`, `KC_WH_L`, `KC_WH_R` 鼠标管轮
- `KC_BTN1`, `KC_BTN2`, `KC_BTN3`, `KC_BTN4`, `KC_BTN5` 几个鼠标按钮

### 1.4 System & Media 系统和媒体键
- `KC_PWR`, `KC_SLEP`, `KC_WAKE` 开机power, 休眠Sleep, 唤醒Wake
- `KC_MUTE`, `KC_VOLU`, `KC_VOLD` 音量增加减小静音 audio volume control
- `KC_MNXT`, `KC_MPRV`, `KC_MSTP`, `KC_MPLY`, `KC_MSEL` 上一首,下一首之类 
- `KC_MAIL`, `KC_CALC`, `KC_MYCM` for application launch
- `KC_WSCH`, `KC_WHOM`, `KC_WBAK`, `KC_WFWD`, `KC_WSTP`, `KC_WREF`, `KC_WFAV` 浏览器控制按钮

### 1.5 Fn key 功能键
`KC_FNnn` 这些案件都是 `Fn` 键,但是都没有被提前设定功能. 要在 `KEYMAP()` 中使用这些功能键,你需要自己定义需要的功能(Action). 这些 `Fn` 按键的功能需要在  `fn_actions[]` 数组中定义,而每个功能的数组角标(index),就对应了 `KC_FNnn` 中的那个第二个n值. 因此 `KC_FN0` 就代表了 fn_acions[] 数组中的第一个定义值. ***最多定义32个 `Fn` 值***

### 1.6 键位对应表 keycode
 请在文档 [`doc/keycode.txt`](./keycode.txt) 里查看详细的键位定义.

 在 [HID usage(使用蓝牙)][HID_usage](pdf) 发送数据过程中,这些键位码基本是相同的,但有一些虚拟键不同.详见
[HID_usage(使用蓝牙文档)]: http://www.usb.org/developers/hidpage/Hut1_12v2.pdf



## 2. Action(自定义功能)
详见文件 [`common/action_code.h`](../common/action_code.h). Action(动作功能) 是一个 **16bit code(16位数)** 用来定义当键位进行按下(press),抬起(release),按住(holding)和按一下(tapping)之后发生的动作(function).

大部分按键使用8位码映射记录,但这里需要使用16位码来完成更复杂的功能. However, using 16bit action codes in keymap results in double size in memory compared to using just keycodes.但这样做相当于记录键位都使用16位字节了,那么就造成了很大的蓝给. To avoid this waste 8bit keycodes are used in `KEYMAP()` instead of action codes.为了避免这样的浪费,在 `KEYMAP()` 中依然使用8位键位来代替这些action码.

***你可以在键盘映射表(keymap)中仅使用 `Normal key`, `Modifier`, `Mousekey` 和 `System & Media key` 这些键*** 来控制action,而不是自己定义动作码(Action). 当你 ***要使用自定义的特殊按键时可以用 `Fn` 来在 `fn_actions[]` 数组中定义.***


### 2.1 按键动作(Key Action)--[注:翻译的可能有问题]
这是一个简单的动作来注册(scancodes HID 在蓝牙中使用),用来在蓝牙中进行主机连接配对等(?).

#### 参数
+ **mods**: { ` MOD_LCTL`, ` MOD_LSFT`, ` MOD_LALT`, ` MOD_LGUI`,
              ` MOD_RCTL`, ` MOD_RSFT`, ` MOD_RALT`, ` MOD_RGUI` }
+ **key**: keycode


#### 2.1.1 标准按键和修改符(Modifier)
***这种方式通常并不使用*** 因为你可以直接用简单的键位码子啊 `KEYMAP()` 中替代来达成目的.

你可以这样将 *'A'* 键 和 *'left shift'* 键修改成:

    ACTION_KEY(KC_A)
    ACTION_KEY(KC_LSFT)

#### 2.1.2 Modified key
这样的动作由修饰键和普通键组成. `Macro(宏)` 可以用来定义完成更复杂的功能.

例如你想指定一个按键 `Shift + 1` 来输入一个字符 *'!'* 或者按 `Alt + Tab` 来在windows系统中切换应用.

    ACTION_MODS_KEY(MOD_LSFT, KC_1)
    ACTION_MODS_KEY(MOD_LALT, KC_TAB)

或者 `Alt,Shift + Tab` 这样也可以来定义. `ACTION_MODS_KEY(mods, key)` 定义需要 **4-bit modifier state** 和一个 **keycode** 作为参数. 查看文件 `keycode.h` 中的 `MOD_BIT()` 宏代码(macro).

    ACTION_MODS_KEY(MOD_LALT | MOD_LSFT, KC_TAB)

#### 2.1.3 多个 Modifiers(按键修饰符)一起按下的定义
注册同时按下多个按键修饰符. 就在多个修饰符中间加 `|`.

    ACTION_MODS(MOD_ALT | MOD_LSFT)

#### 2.1.3 Modifier with Tap key按键双重作用([Dual role][dual_role]双重做用?)
注册一个键拥有两种功能,分别在按住不放和点击一下马上放开两种情况下起作用.


    ACTION_MODS_TAP_KEY(MOD_RCTL, KC_ENT)



### 2.2 Layer层 Action功能
这些事关于动作的 keymap映射.

#### 参数
You can specify a **target layer** of action and **when the action is executed**. Some actions take a **bit value** for bitwise operation.


+ **layer**: `0`-`31`
+ **on**: { `ON_PRESS` | `ON_RELEASE` | `ON_BOTH` }
+ **bits**: 4-bit value and 1-bit mask bit


#### 2.2.1 默认层
如果其它层没有覆盖定义,那么正常我们工作在默认层.

这个用来设置默认层,参数 `layer` 就是代表您设定的默认层.

    ACTION_DEFAULT_LAYER_SET(layer)


#### 2.2.2 Momentary 瞬间切换
当按住时瞬间切换到这个 `layer层` , 但是当松开时就会释放这个层,回到之前的层去.

    ACTION_LAYER_MOMENTARY(layer)


#### 2.2.3 Toggle Switch切换层
当按这个定义的键(按一下并放开),就会切换到对应的层`layer`去.

    ACTION_LAYER_TOGGLE(layer)


#### 2.2.4 Momentary Switch with tap key瞬间按键切换
Turns on `layer` momentary while holding, but registers key on tap(press and release quickly).按住键时打开层,但是在击键时??

    ACTION_LAYER_TAP_KEY(layer, key)


#### 2.2.5 Momentary Switch with tap toggle
Turns on `layer` momentary while holding and toggles it with serial taps.

    ACTION_LAYER_TAP_TOGGLE(layer)


#### 2.2.6 Invert state of layer层反转状态
反转当前层的状态,如果是开则变成关闭.

    ACTION_LAYER_INVERT(layer, on)


#### 2.2.7 Turn On layer 开启层
将层的状态设置为开启

    ACTION_LAYER_ON(layer, on)

按下时开启层,放开时关闭层

    ACTION_LAYER_ON_OFF(layer)


#### 2.2.8 Turn Off layer关闭层
将层的状态设置为关闭

    ACTION_LAYER_OFF(layer, on)

按下时关闭层,放开时开启层

    ACTION_LAYER_OFF_ON(layer)


#### 2.2.9 Set layer设置曾
仅将层开启.
`layer_state = (1<<layer) [layer: 0-31]`

    ACTION_LAYER_SET(layer, on)

仅将这层开启,当放开键时清理其它层状态

    ACTION_LAYER_SET_CLEAR(layer)


#### 2.2.10 Bitwise operation 位运算

**part** 用 32bit 来代表层状态(0-7). **bits** 是 5-bit 值. **on** 指示何时执行操作.

    ACTION_LAYER_BIT_AND(part, bits, on)
    ACTION_LAYER_BIT_OR(part, bits, on)
    ACTION_LAYER_BIT_XOR(part, bits, on)
    ACTION_LAYER_BIT_SET(part, bits, on)

这些操作与下列代码的参数一起工作.

    uint8_t shift = part*4;
    uint32_t mask = (bits&0x10) ? ~(0xf<<shift) : 0;
    uint32_t layer_state = layer_state <bitop> ((bits<<shift)|mask);


默认层在它们执行释放时也按位运算.

    ACTION_DEFAULT_LAYER_BIT_AND(part, bits)
    ACTION_DEFAULT_LAYER_BIT_OR(part, bits)
    ACTION_DEFAULT_LAYER_BIT_XOR(part, bits)
    ACTION_DEFAULT_LAYER_BIT_SET(part, bits)



### 2.3 Macro action 宏操作
***TBD***

`Macro` 指示复杂的按键操作.
 
    MACRO( D(LSHIFT), D(D), END )
    MACRO( U(D), U(LSHIFT), END )
    MACRO( I(255), T(H), T(E), T(L), T(L), W(255), T(O), END )

#### 2.3.1 Macro Commands 宏命令
- **I()**   按键间的间隔时间.
- **D()**   按下键
- **U()**   释放键
- **T()**   击键(按下然后快速释放键)
- **W()**   等待
- **END**   宏结束符号

#### 2.3.2 例子

***TODO: sample implementation***
详见文件 `keyboard/hhkb/keymap.c` 中的例子.



### 2.4 Function action 函数操作
***TBD***

有两种函数, 普通函数 `Function` 和 tappable(点击?)函数 `Function`.
这些动作调用用户定义的函数与`id`, `opt`，和 事件 信息作为参数。

#### 2.4.1 Function 普通函数
定义普通函数.

    ACTION_FUNCTION(id, opt)

#### 2.4.2 Function with tap
定义按键功能函数.

    ACTION_FUNCTION_TAP(id, opt)

#### 2.4.3 实现用户自定义功能函数
`Function` 用户何以在回调函数中用C自定义操作:

    void keymap_call_function(keyrecord_t *event, uint8_t id, uint8_t opt)

This C function is called every time key is operated, argument `id` selects action to be performed and `opt` can be used for option. Function `id` can be 0-255 and `opt` can be 0-15.

 `keyrecord_t` is comprised of key event and tap count. `keyevent_t` indicates which and when key is pressed or released. From `tap_count` you can know tap state, 0 means no tap. These information will be used in user function to decide how action of key is performed.

    typedef struct {
        keyevent_t  event;
        uint8_t     tap_count;
    } keyrecord_t;

    typedef struct {
        key_t    key;
        bool     pressed;
        uint16_t time;
    } keyevent_t;

    typedef struct {
        uint8_t col;
        uint8_t row;
    } key_t;

***TODO: sample implementation***
See `keyboard/hhkb/keymap.c` for sample.



### 2.5 Backlight Action 背光功能(我的hhkb没有背光,所以先不管了)
These actions control the backlight.

#### 2.5.1 Change backlight level
Increase backlight level.

    ACTION_BACKLIGHT_INCREASE()

Decrease backlight level.

    ACTION_BACKLIGHT_DECREASE()

Step through backlight levels.

    ACTION_BACKLIGHT_STEP()

Turn a specific backlight level on or off.

    ACTION_BACKLIGHT_LEVEL(1)

#### 2.5.2 Turn on / off backlight
Turn the backlight on and off without changing level.

    ACTION_BACKLIGHT_TOGGLE()



## 3. 层选择例子
以下是一些切换层的例子

### 3.1 快速切换
使用Fn键瞬间开关变化层

This action makes 'Layer 1' active(valid) on key press event and inactive on release event. Namely you can overlay a layer on lower layers or default layer temporarily with this action.

    ACTION_LAYER_MOMENTARY(1)


Note that after switching on press the actions on destination layer(Layer 1) are performed.
***Thus you shall need to place an action to go back on destination layer***, or you will be stuck in destination layer without way to get back. Usually you need to place same action or 'KC_TRNS` on destination layer to get back.


### 3.2 Toggle switching
Toggle switching performed after releasing a key. With this action you can keep staying on the destination layer until you type the key again to return.

This performs toggle switching action of 'Layer 2'.

    ACTION_LAYER_TOGGLE(2)



### 3.3 Momentary switching with Tap key
These actions switch a layer only while holding a key but register the key on tap. **Tap** means to press and release a key quickly.

    ACTION_LAYER_TAP_KEY(2, KC_SCLN)

With this you can place a layer switching action on normal key like ';' without losing its original key register function. This action allows you to have layer switching action without necessity of a dedicated key. It means you can have it even on home row of keyboard.



### 3.4 Momentary switching with Tap Toggle
This switches layer only while holding a key but toggle layer with several taps. **Tap** means to press and release key quickly.

    ACTION_LAYER_TAP_TOGGLE(1)

Number of taps can be configured with `TAPPING_TOGGLE` in `config.h`, `5` by default.



### 3.5 Momentary switching with Modifiers
This registers modifier key(s) simultaneously with layer switching.

    ACTION_LAYER_MODS(2, MOD_LSFT | MOD_LALT)



## 4. Tapping
Tapping is to press and release a key quickly. Tapping speed is determined with setting of `TAPPING_TERM`, which can be defined in `config.h`, 200ms by default.

### 4.1 Tap Key
This is a feature to assign normal key action and modifier including layer switching to just same one physical key. This is a kind of [Dual role key][dual_role]. It works as modifier when holding the key but registers normal key when tapping.

Modifier with tap key:

    ACTION_MODS_TAP_KEY(MOD_RSFT, KC_GRV)

Layer switching with tap key:

    ACTION_LAYER_TAP_KEY(2, KC_SCLN)

[dual_role]: http://en.wikipedia.org/wiki/Modifier_key#Dual-role_keys


### 4.2 Tap Toggle
This is a feature to assign both toggle layer and momentary switch layer action to just same one physical key. It works as momentary layer switch when holding a key but toggle switch with several taps.

    ACTION_LAYER_TAP_TOGGLE(1)


### 4.3 Oneshot Modifier
This runs onetime effects which modify only on just one following key. It works as normal modifier key when holding down while oneshot modifier when tapping.

    ACTION_MODS_ONESHOT(MOD_LSFT)

Say you want to type 'The', you have to push and hold Shift key before type 't' then release it before type 'h' and 'e', otherwise you'll get 'THe' or 'the' unintentionally. With Oneshot Modifier you can tap Shift then type 't', 'h' and 'e' normally, you don't need to holding Shift key properly here. This mean you can release Shift before 't' is pressed down.

Oneshot effect is cancel unless following key is pressed down within `ONESHOT_TIMEOUT` of `config.h`. No timeout when it is `0` or not defined.


### 4.4 Tap Toggle Mods
Similar to layer tap toggle, this works as a momentary modifier when holding, but toggles on with several taps. A single tap will 'unstick' the modifier again.

    ACTION_MODS_TAP_TOGGLE(MOD_LSFT)




## 5. Legacy Keymap
This was used in prior version and still works due to legacy support code in `common/keymap.c`. Legacy keymap doesn't support many of features that new keymap offers. ***It is not recommended to use Legacy Keymap for new project.***

To enable Legacy Keymap support define this macro in `config.h`.

    #define USE_LEGACY_KEYMAP

Legacy Keymap uses two arrays `fn_layer[]` and `fn_keycode[]` to define Fn key. The index of arrays corresponds with postfix number of `Fn` key. Array `fn_layer[]` indicates destination layer to switch and `fn_keycode[]` has keycodes to send when tapping `Fn` key.

In following setting example, `Fn0`, `Fn1` and `Fn2` switch layer to 1, 2 and 2 respectively. `Fn2` registers `Space` key when tapping while `Fn0` and `Fn1` doesn't send any key.

    static const uint8_t PROGMEM fn_layer[] = {
        1,              // Fn0
        2,              // Fn1
        2,              // Fn2
    };

    static const uint8_t PROGMEM fn_keycode[] = {
        KC_NO,          // Fn0
        KC_NO,          // Fn1
        KC_SPC,         // Fn2
    };


## 6. 术语表
***TBD(待定)***
### keymap
由多个层(layer)组成
### layer
矩阵的按键码
### key
键盘上的物理按钮或软件上的逻辑开关
### keycode
固件上使用的代码
### action
在键上分配的函数
### layer transparency(层透明)
在一层定义透明,则可穿透这层漏出下一层的键设定
### layer precedence(层优先)
顶层比下层具有更高的优先级.
### tapping(按键)
快速的按下并放开一个键
### Fn key(Fn 键)
执行一些特殊动作,如层切换，鼠标键，宏等
### dual role key(双重角色按键)
<http://en.wikipedia.org/wiki/Modifier_key#Dual-role_keys>



