# Zephyr

> LiME-TM: MNIST Training Example

## Step 1: Install Zephyr

```
$ pip install west
$ west init ~/zephyrproject

$ cd ~/zephyrproject
$ west update

$ west zephyr-export
$ west packages pip --install
```

## Step 2: Install Zephyr-SDK

```
$ cd ~/zephyrproject/zephyr
$ west sdk install
```

## Step 3: Build

```
$ cd platforms/zephyr

$ west build -b pandora_stm32l475
$ west build -b black_f407zg_pro
$ west build -b mini_stm32h743
```

## Step 4: Upload the firmware:

Using [`probe-rs`](https://probe.rs/) to downlaod `elf` via a Debugger:

```
$ probe-rs download build/zephyr/zephyr.elf
```

Using [`stm32flash`](https://github.com/stm32duino/stm32flash) to download `hex` via UART:

```
$ stm32flash -w build/zephyr/zephyr.hex /dev/ttyUSB0
```

Using `dfu-utils` to download `bin` via USB:

```
$ dfu-util -a 0 -s 0x08000000:leave -D build/zephyr/zephyr.bin
```