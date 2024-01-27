Once you have downloaded of cloned this repo to your machine using git, run
this command in your CLI
```
g++ -pthread -o run prodCons.cpp
```
What we did here is compiled the cpp file called prodCons. We compiled the 
executable file under the name 'run' and used the pthread flag to tell
the compiler that this program will require the pthread.h flag to function.

Then, run the following command into your CLI to run the file
```
./run
```

To close the program, press ctr + c
