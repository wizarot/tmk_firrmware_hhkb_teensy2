建立固件和程序控制器
=====================================


下载和安装
--------------------
### 1. 安装相关工具

1. **Toolchain** Windows系统 [MHV AVR Tools][mhv] for AVR GCC compiler and [Cygwin][cygwin](or [MinGW][mingw]) for shell terminal. Mac系统 [CrossPack][crosspack]. Linux系统安装 AVR GCC .

2. **Programmer** Windows: [Atmel FLIP][flip]. Mac 和 Linux [dfu-programmer][dfu-prog].

3. **Driver** On Windows you start DFU bootloader on the chip first time you will see 'Found New Hardware Wizard' to install driver. If you install device driver properly you can find chip name like 'ATmega32U4' under 'LibUSB-Win32 Devices' tree on 'Device Manager'. If not you shall need to update its driver on 'Device Manager'. You will find the driver in `FLIP` install directory like: C:\Program Files (x86)\Atmel\Flip 3.4.5\usb\. In case of `dfu-programmer` use its driver.

我们这里使用Teensy开发板,所以直接使用这个就行了,前面2,3步都可以省略 [Teensy loader][teensy-loader].


### 2. 下载源代码
github上的项目:

- <https://github.com/wizarot/tmk_firrmware_hhkb_teensy2>

直接用git工具clone下来,或者点击下载下面这个zip压缩包:

- <https://github.com/wizarot/tmk_firrmware_hhkb_teensy2/archive/master.zip>


编译项目
--------------
### 1. 打开命令行工具
这步懒得翻译了,反正windows你就用cmd,mac,linux都用对应的,这个你都不会就问问比人吧.

### 2. 进入项目录
cd 到项目的目录中去,注意下选择你对应的键盘或者转换器,我这里用hhkb键盘,所以就是keyboard/hhkb/ 目录.

    cd tmk_keyboard/{'keyboard' or 'converter'}/<project>

### 3. Make(这个不翻译,make就是编译)
参照下面这个例子,用make命令对你的项目进行编译,最终完成一个 `<project>_<variant>.hex` 这样的hex文件,然后teensy可以用官网下载的对应软件把这hex文件上传到固件就可以使用了.


    make -f Makefile.<variant> clean
    make -f Makefile.<variant>




程序控制器
------------------
最终编译出来的 **hex** 这个文件是你需要的,其它都是源代码你可以根据实际需要来进行修改和重新编译之类的, 这个hex上传到硬件就可以跑起来了.下面详细讲一遍方法,如果还是不明白可以考虑看看teensy的官网,配图那个我觉得更好理解.

### 1. 启动bootloader
How to program controller depends on controller chip and its board design. To program AVR USB chips you'll need to start it up in bootloader mode. Most of boards with the chip have a push button to let bootloader come up. Consult with your controller board manual.

### 2. Program with DFU bootloader(用不上先跳过)
Stock AVR USB chip including ATmega32U4 has DFU bootloader by factory default. `FLIP` is a DFU programmer on Windows offered by Atmel. Open source command line tool `dfu-programmer` also supports AVR chips, it runs on Linux, Mac OSX and even Windows.

To program AVR chip with DFU bootloader use `FLIP` or `dfu-programmer`.
If you have a proper program command in `Makefile` just type this.

`FLIP` has two version of tool, GUI app and command line program. If you want GUI see tutorial below.
To use command line tool run this command. Note that you need to set PATH variable properly.

    $ make -f Makefile.<variant> flip

Or to program with `dfu-programmer` run:

    $ make -f Makefile.<variant> dfu

#### FLIP GUI tutorial(用不上先跳过)
1. On menu bar click Device -> Select, then. `ATmega32u4`.
2. On menu bar click Settings -> Communication -> USB, then click 'Open' button on 'USB Port Connection' dialog.
At this point you'll see grey-outed widgets on the app get colored and ready.

3. On menu bar click File -> Load HEX File, then select your firmware hex file on File Selector dialog.
4. On 'Operations Flow' panel click 'Run' button to load the firmware binary to the chip. Note that you should keep 'Erase', 'Blank Check', 'Program' and 'Verify' check boxes selected.
5. Re-plug USB cord or click 'Start Application' button to restart your controller.
Done.

See also these instructions if you need.

- <http://code.google.com/p/micropendous/wiki/LoadingFirmwareWithFLIP>
- <http://www.atmel.com/Images/doc7769.pdf>


### 3. Teensy主板上传
看下面这个地址的说明来使用 `Teensy Loader`,第一步安装那个软件,第二步编译好的hex文件选择上,第三步插上usb线到teensy板,第四步按板子上那个bootloader按钮上传完事儿就行了,就这么简单.

- <http://www.pjrc.com/teensy/loader.html>

如果你安装了Teensy Loader 的命令行上传工具,就用下面这个命令来上传安装(我没试过,不知道是否可以.).

    $ make -f Makefile.<variant> teensy


### 4. Program with Other programmer(没用到,还是跳过)
You may want to use other programmer like `avrdude` with AVRISPmkII, Arduino or USBasp. In that case you can still use make target `program` for build with configuring `PROGRAM_CMD` in Makefile.

    $ make -f Makefile.<variant> program


[cygwin]:       https://www.cygwin.com/
[mingw]:        http://www.mingw.org/
[mhv]:          https://infernoembedded.com/products/avr-tools
[winavr]:       http://winavr.sourceforge.net/
[crosspack]:    http://www.obdev.at/products/crosspack/index.html
[flip]:         http://www.atmel.com/tools/FLIP.aspx
[dfu-prog]:     http://dfu-programmer.sourceforge.net/
[teensy-loader]:http://www.pjrc.com/teensy/loader.html



Makefile Options选项
----------------
### 1. MCU and Frequency.(如果用teensy2.0,这些是不用改的,反正我直接编译就可以用了,你如果不确定自己知道在干什么就别乱动了)

    MCU = atmega32u4       # Teensy 2.0 (我用的这个)
    #MCU = at90usb1286      # Teensy++ 2.0(这个也很容易买到但如果不是熟手,最好还是用上面那款比较安全)
    F_CPU = 16000000

Set your MCU and its clock in Hz.

    # Boot Section Size in *bytes*
    #   Teensy halfKay   512
    #   Atmel DFU loader 4096
    #   LUFA bootloader  4096
    OPT_DEFS += -DBOOTLOADER_SIZE=4096

If you are using PJRC Teensy use `512` for `BOOTLOADER_SIZE`, otherwise use `4096` unless you are sure.

### 2. 特性
如果不启用,则使用 `#`来注释掉对应的行.

    BOOTMAGIC_ENABLE = yes      # Virtual DIP switch configuration(+1000)
    MOUSEKEY_ENABLE = yes       # Mouse keys(+4700)
    EXTRAKEY_ENABLE = yes       # Audio control and System control(+450)
    CONSOLE_ENABLE = yes        # Console for debug(+400)
    COMMAND_ENABLE = yes        # Commands for debug and configuration
    SLEEP_LED_ENABLE = yes      # Breathing sleep LED during USB suspend
    #NKRO_ENABLE = yes          # USB Nkey Rollover - not yet supported in LUFA
    #BACKLIGHT_ENABLE = yes     # Enable keyboard backlight functionality

### 3. Programmer
Optional. Set proper command for your controller, bootloader and programmer. This command can be used with `make program`. Not needed if you use `FLIP`, `dfu-programmer` or `Teensy Loader`.

    # avrdude with AVRISPmkII
    PROGRAM_CMD = avrdude -p $(MCU) -c avrispmkII -P USB -U flash:w:$(TARGET).hex

    # avrdude with USBaspLoader
    PROGRAM_CMD = avrdude -p $(MCU) -c usbasp -U flash:w:$(TARGET).hex

    # avrdude with arduino
    PROGRAM_CMD = avrdude -p $(MCU) -c arduino -P COM1 -b 57600 -U flash:w:$(TARGET).hex



Config.h 文件中的配置项
----------------
### 1. Magic command key combination

    #define IS_COMMAND() (keyboard_report->mods == (MOD_BIT(KB_LSHIFT) | MOD_BIT(KB_RSHIFT)))

### 2. Mechanical Locking Support for CapsLock

    /* Mechanical locking CapsLock support. Use KC_LCAP instead of KC_CAPS in keymap */
    #define CAPSLOCK_LOCKING_ENABLE
    /* Locking CapsLock re-synchronize hack */
    #define CAPSLOCK_LOCKING_RESYNC_ENABLE

### 3. 关闭debug模式

    /* disable debug print */
    #define NO_DEBUG

    /* disable print */
    #define NO_PRINT

### 4. 关闭一些特性

    #define NO_ACTION_LAYER
    #define NO_ACTION_TAPPING
    #define NO_ACTION_ONESHOT
    #define NO_ACTION_MACRO
    #define NO_ACTION_FUNCTION

***TBD***


