# Windows / Linux

> LiME-TM: MNIST Training Example

```
sudo apt install build-essential cmake
```

First, initialize the `mnist` dataset:

```
cd platforms/win_linux
git submodule init
git submodule update
```

Compile on Linux:

```
cmake -S . -B build
cmake --build build
```

Compile on Windows:

```
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build
```

Run the compiled example:

```
cd build
./lime-tm
```

```
[I][main] Model loaded (163502 Bytes)
[I][main]
[I][main] Model loaded successfully
[I][main] n_class   = 10
[I][main] n_feature = 6272
[I][main] n_clause  = 200
[I][main] n_state   = 100
[I][main] model_type = 2
[I][main]
[I][main]
[I][main] MNIST training set: 60000 images of size 28x28
[I][main] MNIST testing set: 10000 images of size 28x28
[I][main]
[I][main] Loading and printing training image 48802
[I][main]
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@                    @@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@                        @@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@          @@@@@@          @@@@@@@@@@
@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@        @@@@@@@@@@@@
@@@@@@@@@@@@@@@@        @@@@@@@@@@@@      @@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@      @@@@@@@@@@@@      @@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@    @@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@        @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@      @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
[I][main] Evaluating model on test image 9788 (label 7)
[I][main] Class 0: -33 votes
[I][main] Class 1: -28 votes
[I][main] Class 2: -31 votes
[I][main] Class 3: -32 votes
[I][main] Class 4: -10 votes
[I][main] Class 5: -16 votes
[I][main] Class 6: -37 votes
[I][main] Class 7: 22 votes
[I][main] Class 8: -12 votes
[I][main] Class 9: -9 votes
[I][main] Predicted class: 7 with 22 votes
[I][main]
Testing 10000/10000 [========================================] 100%
[FS] Achieved 0.00 ms/image
[TM] Achieved 0.19 ms/image
Accuracy on test set (10000): 95.07%
```
