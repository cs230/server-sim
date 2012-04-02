# Server Simulation (30 Points)
Server processes are long running programs that provide a service to clients
and fulfill incoming requests by performing the task requested. In order to 
satisfy client requests as quickly as possible a server will use concurrency. 
One way of accomplishing this is to use the `fork()` system call to create a 
new process for each incoming request. It is the responsibility of the 
*child process* to service the request and complete the task. The responsibility 
then of the server process is to manage the *creation* and *reaping* of these 
child processes. In this assignment you will create a program that simulates 
an HTTP server program. The HTTP server waits for incoming requests and when 
one is received forks a new child process to handle the request and then returns 
to waiting for the next incoming request. When a child completes the server must 
*reap* the process using the `waitpid()` system call. Because the server is a 
long running program (likely non-terminating) and because it can't know before 
hand how many child processes it will be creating we can't rely on calling 
`waitpid()` after a child is created - this would make the use of processes 
and concurrency pointless. Instead, the server must use provide a *signal handler* 
to catch the `SIGCHLD` signal to perform the necessary clean up after a child 
process exits.

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
2. Open the `server.c` file and implement the TODOs in the `reap_children()` 
and `server()` functions.
3. The `reap_children()` function is *easy*. You should study the example 
code in **Figure 8.31** on page 747 and **Figure 8.32** on page 749 in the 
book. You should read the corresponding text that describes the difference 
between these examples and choose the correct implementation for reaping 
children in a long running server process. Use the provided `printf` to 
generate the identical output as my solution. Don't forget to decrement 
the `child_count` variable after reaping each child.
4. The `server()` function is more difficult. First, you must install 
the signal handler function `handler()` to be invoked when the parent 
process receives the `SIGCHLD` signal. This signal is delivered to the 
parent process each time a child process exits. More details about this are 
discussed in Chapter 8 so you should review it there. You will want to loop 
over the number of requests specified by the `numreq` argument. For each 
iteration you need to make a call to `accept()` which simulates receiving 
a request from the network. When accept returns you need to *fork* a new 
child to handle that request which involves a single call to `service_req()` 
passing to it the current request number. You should keep track of the 
current request number using an `int` and increment it after each call to 
`accept()`. Make sure you increment the `child_count` variable right before 
your call to `fork()`. Lastly, the parent process within the `server()` 
function must make a check to see if `child_count` is equal to `MAX_CHILDREN`. 
If it is you must call the `pause()` function to put the parent process to 
sleep until it receives a `SIGCHLD` signal. When it receives the signal 
the registered `handler()` function will be invoked and then the `pause()` 
function will return. It is important that you implement this check as a 
loop because you can't make any assumption about the interleaved execution of 
processes. In particular, you should loop while `child_count` is equal to 
`MAX_CHILDREN` and call `pause()` each time. There are three different 
`printf`s that we have provided you - use these calls to generate output 
that is identical (except for order) to our solution. 

## Part 2 - Concurrency Bugs (BONUS: 5 points)
You will notice that when you run your implementation from **Part 1** your
server may get stuck in the `pause()` function. All activity will stop and 
the terminal will display `SERVER: Max child reached. Waiting...`. It turns 
out that this is related to a concurrency bug associated with subtleties in 
the way processes are interleaved and how signals are delivered. To solve 
this problem you need to manipulate the *blocked bit vector* in the process 
context to block the `SIGCHLD` signal for a short period of time. To solve 
this you should read **Section 8.5.7 Synchronizing Flows to Avoid Nasty Concurrency Bugs** 
and look at the example code in **Figure 8.37** on page 757 in the book. 
It shows you how to solve this problem for a very similiar program. Use this 
as an example to fix the basic implementation from part 1. If you do it 
right your server program will execute to completion each and every time 
it is invoked.  

## Sample Output
	[Sat Mar 31:server-sim] ./server 
	SERVER: Waiting for request.
	SERVER: Received request 1.
	SERVER: Waiting for request.
	SERVER: Received request 2.
	SERVER: Waiting for request.
	SERVER: Received request 3.
	SERVER: Waiting for request.
	SERVER: Received request 4.
	CHILD: Child 27945 handling request 2.
	CHILD: Child 27944 handling request 1.
	SERVER: Waiting for request.
	SERVER: Received request 5.
	CHILD: Child 27946 handling request 3.
	SERVER: Max child reached. Waiting...
	CHILD: Child 27947 handling request 4.
	CHILD: Child 27948 handling request 5.
	CHILD: Child 27948 completed request 5.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27948
	CHILD: Child 27947 completed request 4.
	CHILD: Child 27946 completed request 3.
	SERVER: Handler reaped child 27947
	SERVER: Handler reaped child 27946
	CHILD: Child 27945 completed request 2.
	CHILD: Child 27944 completed request 1.
	SERVER: Handler reaped child 27945
	SERVER: Handler reaped child 27944
	SERVER: Waiting for request.
	SERVER: Received request 6.
	SERVER: Waiting for request.
	SERVER: Received request 7.
	SERVER: Waiting for request.
	SERVER: Received request 8.
	SERVER: Waiting for request.
	SERVER: Received request 9.
	CHILD: Child 27949 handling request 6.
	CHILD: Child 27950 handling request 7.
	SERVER: Waiting for request.
	SERVER: Received request 10.
	CHILD: Child 27951 handling request 8.
	SERVER: Max child reached. Waiting...
	CHILD: Child 27952 handling request 9.
	CHILD: Child 27953 handling request 10.
	CHILD: Child 27953 completed request 10.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27953
	CHILD: Child 27949 completed request 6.
	CHILD: Child 27952 completed request 9.
	SERVER: Handler reaped child 27952
	SERVER: Handler reaped child 27949
	CHILD: Child 27951 completed request 8.
	SERVER: Handler reaped child 27951
	CHILD: Child 27950 completed request 7.
	SERVER: Handler reaped child 27950
	SERVER: Waiting for request.
	SERVER: Received request 11.
	SERVER: Waiting for request.
	SERVER: Received request 12.
	SERVER: Waiting for request.
	SERVER: Received request 13.
	CHILD: Child 27954 handling request 11.
	SERVER: Waiting for request.
	SERVER: Received request 14.
	CHILD: Child 27955 handling request 12.
	CHILD: Child 27955 completed request 12.
	SERVER: Waiting for request.
	SERVER: Received request 15.
	CHILD: Child 27956 handling request 13.
	CHILD: Child 27956 completed request 13.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27955
	CHILD: Child 27957 handling request 14.
	SERVER: Handler reaped child 27956
	CHILD: Child 27958 handling request 15.
	CHILD: Child 27958 completed request 15.
	SERVER: Handler reaped child 27958
	CHILD: Child 27954 completed request 11.
	SERVER: Handler reaped child 27954
	CHILD: Child 27957 completed request 14.
	SERVER: Handler reaped child 27957
	SERVER: Waiting for request.
	SERVER: Received request 16.
	SERVER: Waiting for request.
	SERVER: Received request 17.
	SERVER: Waiting for request.
	SERVER: Received request 18.
	CHILD: Child 27959 handling request 16.
	SERVER: Waiting for request.
	SERVER: Received request 19.
	CHILD: Child 27960 handling request 17.
	SERVER: Waiting for request.
	SERVER: Received request 20.
	CHILD: Child 27961 handling request 18.
	SERVER: Max child reached. Waiting...
	CHILD: Child 27962 handling request 19.
	CHILD: Child 27963 handling request 20.
	CHILD: Child 27963 completed request 20.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27963
	CHILD: Child 27962 completed request 19.
	SERVER: Handler reaped child 27962
	CHILD: Child 27960 completed request 17.
	CHILD: Child 27961 completed request 18.
	SERVER: Handler reaped child 27961
	SERVER: Handler reaped child 27960
	CHILD: Child 27959 completed request 16.
	SERVER: Handler reaped child 27959
	SERVER: Waiting for request.
	SERVER: Received request 21.
	SERVER: Waiting for request.
	SERVER: Received request 22.
	SERVER: Waiting for request.
	SERVER: Received request 23.
	SERVER: Waiting for request.
	SERVER: Received request 24.
	CHILD: Child 27964 handling request 21.
	CHILD: Child 27964 completed request 21.
	CHILD: Child 27965 handling request 22.
	CHILD: Child 27965 completed request 22.
	SERVER: Waiting for request.
	SERVER: Received request 25.
	CHILD: Child 27966 handling request 23.
	CHILD: Child 27966 completed request 23.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27966
	SERVER: Handler reaped child 27965
	SERVER: Handler reaped child 27964
	CHILD: Child 27967 handling request 24.
	CHILD: Child 27967 completed request 24.
	CHILD: Child 27968 handling request 25.
	SERVER: Handler reaped child 27967
	CHILD: Child 27968 completed request 25.
	SERVER: Handler reaped child 27968
	SERVER: Waiting for request.
	SERVER: Received request 26.
	SERVER: Waiting for request.
	SERVER: Received request 27.
	SERVER: Waiting for request.
	SERVER: Received request 28.
	CHILD: Child 27969 handling request 26.
	SERVER: Waiting for request.
	SERVER: Received request 29.
	CHILD: Child 27970 handling request 27.
	SERVER: Waiting for request.
	SERVER: Received request 30.
	CHILD: Child 27971 handling request 28.
	SERVER: Max child reached. Waiting...
	CHILD: Child 27972 handling request 29.
	CHILD: Child 27973 handling request 30.
	CHILD: Child 27971 completed request 28.
	CHILD: Child 27973 completed request 30.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27973
	SERVER: Handler reaped child 27971
	CHILD: Child 27969 completed request 26.
	CHILD: Child 27972 completed request 29.
	CHILD: Child 27970 completed request 27.
	SERVER: Handler reaped child 27972
	SERVER: Handler reaped child 27970
	SERVER: Handler reaped child 27969
	SERVER: Waiting for request.
	SERVER: Received request 31.
	SERVER: Waiting for request.
	SERVER: Received request 32.
	SERVER: Waiting for request.
	SERVER: Received request 33.
	SERVER: Waiting for request.
	SERVER: Received request 34.
	SERVER: Waiting for request.
	CHILD: Child 27974 handling request 31.
	CHILD: Child 27975 handling request 32.
	SERVER: Received request 35.
	CHILD: Child 27976 handling request 33.
	SERVER: Max child reached. Waiting...
	CHILD: Child 27977 handling request 34.
	CHILD: Child 27978 handling request 35.
	CHILD: Child 27978 completed request 35.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27978
	CHILD: Child 27975 completed request 32.
	CHILD: Child 27976 completed request 33.
	SERVER: Handler reaped child 27976
	SERVER: Handler reaped child 27975
	CHILD: Child 27974 completed request 31.
	CHILD: Child 27977 completed request 34.
	SERVER: Handler reaped child 27977
	SERVER: Handler reaped child 27974
	SERVER: Waiting for request.
	SERVER: Received request 36.
	SERVER: Waiting for request.
	SERVER: Received request 37.
	SERVER: Waiting for request.
	SERVER: Received request 38.
	CHILD: Child 27979 handling request 36.
	SERVER: Waiting for request.
	CHILD: Child 27979 completed request 36.
	SERVER: Received request 39.
	CHILD: Child 27980 handling request 37.
	SERVER: Waiting for request.
	SERVER: Received request 40.
	CHILD: Child 27981 handling request 38.
	CHILD: Child 27981 completed request 38.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27979
	CHILD: Child 27982 handling request 39.
	SERVER: Handler reaped child 27981
	CHILD: Child 27983 handling request 40.
	CHILD: Child 27983 completed request 40.
	SERVER: Handler reaped child 27983
	CHILD: Child 27982 completed request 39.
	SERVER: Handler reaped child 27982
	CHILD: Child 27980 completed request 37.
	SERVER: Handler reaped child 27980
	SERVER: Waiting for request.
	SERVER: Received request 41.
	SERVER: Waiting for request.
	SERVER: Received request 42.
	SERVER: Waiting for request.
	SERVER: Received request 43.
	SERVER: Waiting for request.
	CHILD: Child 27984 handling request 41.
	SERVER: Received request 44.
	CHILD: Child 27985 handling request 42.
	SERVER: Waiting for request.
	SERVER: Received request 45.
	CHILD: Child 27986 handling request 43.
	CHILD: Child 27986 completed request 43.
	SERVER: Max child reached. Waiting...
	CHILD: Child 27987 handling request 44.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27986
	CHILD: Child 27988 handling request 45.
	CHILD: Child 27988 completed request 45.
	SERVER: Handler reaped child 27988
	CHILD: Child 27984 completed request 41.
	CHILD: Child 27985 completed request 42.
	SERVER: Handler reaped child 27985
	SERVER: Handler reaped child 27984
	CHILD: Child 27987 completed request 44.
	SERVER: Handler reaped child 27987
	SERVER: Waiting for request.
	SERVER: Received request 46.
	SERVER: Waiting for request.
	SERVER: Received request 47.
	SERVER: Waiting for request.
	SERVER: Received request 48.
	SERVER: Waiting for request.
	SERVER: Received request 49.
	CHILD: Child 27989 handling request 46.
	SERVER: Waiting for request.
	CHILD: Child 27990 handling request 47.
	SERVER: Received request 50.
	CHILD: Child 27991 handling request 48.
	CHILD: Child 27992 handling request 49.
	CHILD: Child 27993 handling request 50.
	CHILD: Child 27993 completed request 50.
	CHILD: Child 27991 completed request 48.
	SERVER: Handler invoked.
	SERVER: Handler reaped child 27993
	SERVER: Handler reaped child 27991
	CHILD: Child 27989 completed request 46.
	CHILD: Child 27990 completed request 47.
	SERVER: Handler reaped child 27990
	SERVER: Handler reaped child 27989
	CHILD: Child 27992 completed request 49.
	SERVER: Handler reaped child 27992
	SERVER: completed 50 requests.