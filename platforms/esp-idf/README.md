# ESP32-IDF

> LiME-TM: MNIST Training Example

## Step 1: Insall ESP-IDF:

Install `ESP-IDF` extension for VSCode:

https://marketplace.visualstudio.com/items?itemName=espressif.esp-idf-extension

## Step 2: Build

```
$ cd platforms/esp-idf

$ idf.py menuconfig
$ idf.py build
```

## Step 3: Upload the firmware

```
$ idf.py -p /dev/ttyUSB0 flash
$ idf.py -p /dev/ttyUSB0 monitor
```
