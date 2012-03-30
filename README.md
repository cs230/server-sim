# Server Simulation
Server processes are long running programs that provide a service to clients and fulfill incoming requests by performing the task requested. In order to satisfy client requests as quickly as possible a server will use concurrency. One way of accomplishing this is to use the `fork()` system call to create a new process for each incoming request. It is the responsibility of the *child process* to service the request and complete the task. The responsibility then of the server process is to manage the *creation* and *reaping* of these child processes. In this assignment you will create a program that simulates an HTTP server program. The HTTP server waits for incoming requests and when one is received forks a new child process to handle the request and then returns to waiting for the next incoming request. When a child completes the server must *reap* the process using the `waitpid()` system call. Because the server is a long running program (likely non-terminating) and because it can't know before hand how many child processes it will be creating we can't rely on calling `waitpid()` after a child is created - this would make the use of processes and concurrency pointless. Instead, the server must use provide a *signal handler* to catch the `SIGCHLD` signal to perform the necessary clean up after a child process exits.

# Starting Details
## Source Code Overview
1. `MAX_CHILDREN` specifies the maximum number of child processes allowed.
2. `child_count` is used to record the number of child processes currently executing.
3. `reap_children()` is used to reap child zombie processes. You need to implement this one.
4. `handler()` is the signal handler that is registered to handle the `SIGCHLD` signal.
5. `do_random_work()` is a utility function that simulates random work. It does "work" for a random amount of time.
6. `accept()` is used to simulate incoming network requests. It "receives" requests after `r` seconds.
7. `service_req()` simulates the servicing of a request to be performed by the child processes. This should be called only once from a child process.
8. `server()` is the main server routine. You must implement this. It consists of a main while loop over the number of requests required (specified by `numreq`). It does the job of calling `accept()`, forking child processes to handle the simulated request (using `service_req()`) and keeping track of the number of child processes currently executing and the number of requests that were serviced.

## Compiling
You can compile this program like this:

	$ gcc -o server server.c
	
## Assessment
You will be assessed on your understanding in the following knowledge areas:

1. fork
2. child process
3. signal
4. signal handler
5. program organization
6. reaping

Each knowledge area will be evaluated as follows:

* Student did not use this knowledge. (0 point)
* Student demonstrated little understanding. (1 point)
* Student demonstrated some understanding. (2 points)
* Student demonstrated satisfactory understanding. (3 points)
* Student demonstrated good understanding. (4 points)
* Student demonstrated mastery understanding. (5 points)

Thus, for 6 knowledge areas the maximum total points that can be scored is 30.
There is also an opportunity for a bonus of 5 points in **Part 2** below.

# Instructions

## Part 1 - Basic Implementation
1. Read chapter 8 again.
2. Open the `server.c` file and implement the TODOs in the `reap_children()` and `server()` functions.
3. The `reap_children()` function is *easy*. You should study the example code in **Figure 8.31** on page 747 and **Figure 8.32** on page 749 in the book. You should read the corresponding text that describes the difference between these examples and choose the correct implementation for reaping children in a long running server process. Use the provided `printf` to generate the identical output as my solution. Don't forget to decrement the `child_count` variable after reaping each child.
4. The `server()` function is more difficult. First, you must install the signal handler function `handler()` to be invoked when the parent process receives the `SIGCHLD` signal. This signal is delivered to the parent process each time a child process exits. More details about this are discussed in Chapter 8 so you should review it there. You will want to loop over the number of requests specified by the `numreq` argument. For each iteration you need to make a call to `accept()` which simulates receiving a request from the network. When accept returns you need to *fork* a new child to handle that request which involves a single call to `service_req()` passing to it the current request number. You should keep track of the current request number using an `int` and increment it after each call to `accept()`. Make sure you increment the `child_count` variable right before your call to `fork()`. Lastly, the parent process within the `server()` function must make a check to see if `child_count` is equal to `MAX_CHILDREN`. If it is you must call the `pause()` function to put the parent process to sleep until it receives a `SIGCHLD` signal. When it receives the signal the registered `handler()` function will be invoked and then the `pause()` function will return. It is important that you implement this check as a loop because you can't make any assumption about the interleaved execution of processes. In particular, you should loop while `child_count` is equal to `MAX_CHILDREN` and call `pause()` each time. There are three different `printf`s that we have provided you - use these calls to generate output that is identical (except for order) to our solution. 

## Part 2 - Concurrency Bugs (BONUS: 5 points)
You will notice that when you run your implementation from **Part 1** your server may get stuck in the `pause()` function. All activity will stop and the terminal will display `SERVER: Max child reached. Waiting...`. It turns out that this is related to a concurrency bug associated with subtleties in the way processes are interleaved and how signals are delivered. To solve this problem you need to manipulate the *blocked bit vector* in the process context to block the `SIGCHLD` signal for a short period of time. To solve this you should read **Section 8.5.7 Synchronizing Flows to Avoid Nasty Concurrency Bugs** and look at the example code in **Figure 8.37** on page 757 in the book. It shows you how to solve this problem for a very similiar program. Use this as an example to fix the basic implementation from part 1. If you do it right your server program will execute to completion each and every time it is invoked.  
