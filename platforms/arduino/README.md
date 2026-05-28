# Arduino

> LiME-TM: Arduino package

The Arduino package can be automatically generated:

```
# Windows:
./arduino_library.bat

# Linux
chmod u+x ./arduino_library.sh
./arduino_library.sh
```

Then copy the entire `arduino` folder to arduino libraries folder and rename it as `LiME-TM`:

```
- Documents
  -- Arduino
     -- libraries
        -- LiME-TM
          -- examples/
          -- src/
          -- keywords.txt
          -- library.json
          -- library.properties
```

Restart Arduino IDE, and you wil find the following examples:

- [mnist_inference_1_bit](examples/mnist_inference_1_bit)
- [mnist_inference_4_bit](examples/mnist_inference_4_bit)
- [mnist_inference_8_bit](examples/mnist_inference_8_bit)
- [mnist_train_sd_card_4_bit](examples/mnist_train_sd_card_4_bit)
- [mnist_train_sd_card_8_bit](examples/mnist_train_sd_card_8_bit)
