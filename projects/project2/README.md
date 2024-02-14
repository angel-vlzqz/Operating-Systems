This project is a `simple` UNIX command line interpretter written in C. To 
compile this program in your system, run this command in your CLI
```
gcc -o run bash.c 
```

To run the bash interpretter, run the following command into your CLI
```
./run
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
