# Server Simulation
Server processes are long running programs that provide a service to
clients and fulfill incoming requests by performing the task
requested. In order to satisfy client requests as quickly as possible
a server will use concurrency. One way of accomplishing this is to use
the `fork()` system call to create a new process for each incoming
request. It is the responsibility of the *child process* to service
the request and complete the task. The responsibility then of the
server process is to manage the *creation* and *reaping* of these
child processes. In this assignment you will create a program that
simulates an HTTP server program. The HTTP server waits for incoming
requests and when one is received forks a new child process to handle
the request and then returns to waiting for the next incoming
request. When a child completes the server must *reap* the process
using the `waitpid()` system call. Because the server is a long
running program (likely non-terminating) and because it can't know
before hand how many child processes it will be creating we can't rely
on calling `waitpid()` after a child is created - this would make the
use of processes and concurrency pointless. Instead, the server must
use provide a *signal handler* to catch the `SIGCHLD` signal to
perform the necessary clean up after a child process exits.

# Instructions

