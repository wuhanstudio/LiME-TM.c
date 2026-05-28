# RT-Thread

> LiME-TM: MNIST Training Example

```
$ sudo apt install scons
```

## Step 1: Clone RT-Thread

```
$ git clone http://github.com/rt-thread/rt-thread
```

## Step 2: Build

```
$ cd rt-thread/bsp/stm32/stm32l475-atk-pandora

$ scons --menuconfig

$ source ~/.env/env.sh
$ pkgs --update

$ scons
```

## Step 3: Upload the firmware

Using [`probe-rs`](https://probe.rs/) to downlaod `elf` via a Debugger:

```
$ probe-rs download rtthread.elf
```

Using [`stm32flash`](https://github.com/stm32duino/stm32flash) to download `hex` via UART:

```
$ stm32flash -w rtthread.hex /dev/ttyUSB0
```

Using `dfu-utils` to download `bin` via USB:

```
$ dfu-util -a 0 -s 0x08000000:leave -D rtthread.bin
```
