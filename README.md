# Thread_fork

The C++ code creates a hierarchy of child processes using fork, shared memory, and
atomic variables. The number of processes and child processes are user-inputted. The
program uses recursion to achieve the desired structure of the process tree. To enforce
the constraint of each process creating no more than two child processes, the program
carefully manages the recursive calls and process creation logic using atomic variable.
Each child process waits for 3 seconds before creating its own child processes. The
program uses shared memory to synchronize process creation and ensures proper
cleanup.

Input
Number of processes to be created : <Integer>
Children per process : <Integer>
