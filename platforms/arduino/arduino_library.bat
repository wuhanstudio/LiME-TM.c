@echo off

REM 1. Remove all .c and .h files in src folder
del /Q src\*.c 2>nul
del /Q src\*.h 2>nul

REM 2. Copy .c and .h files from tsetlin, utils, random into src
copy /Y ..\..\tsetlin\*.c src\
copy /Y ..\..\tsetlin\*.h src\

copy /Y ..\..\utils\*.c src\
copy /Y ..\..\utils\*.h src\

copy /Y ..\..\random\*.c src\
copy /Y ..\..\random\*.h src\

echo Done.