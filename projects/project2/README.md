This project is a `simple` UNIX command line interpretter written in C++. Some
parts of the code utilize features from version 11 of C++. Thus, you may need
to tell your compiler to compile the file in version 11 of C++. To do so, run
this command in your CLI
```
g++ -o bash bash.cpp -std=c++11
```
We use `-std=c++11` to tell the compiler to compile the cpp file in version 11
of C++.

To run the bash interpretter, run the following command into your CLI
```
./bash
```

Now, you may interact with the shell as if it were a part of your system.
Here are some commands you may run.

1. `whoami` will print the current user of the system
2. `ls` will print all the files in the current directory. You may also utilize
flags such as `ls -l` to print meta data related to every file in the system.
3. `cd` will return you to the root directory. `cd/path` will guide you to any
directory as long as the path entered is correct.
4. `touch file` will create a new file in current working directory
5. `rm file` will remove any file given into the input. Using `rm -rf file`
will force remove the file given. And `rm -rf /` will delete all directories
on your system. What ever you do, DO NOT RUN `rm -rf /` because you will have
a very bad day

To run the .sh script, run:
```
bash file.sh
```

To quit the program, simply enter `quit`
