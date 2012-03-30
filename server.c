#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>  // sleep, pause

// Maximum number of children:
#define MAX_CHILDREN 5

// The number of children handling requests:
int child_count = 0;

// This function does the work of reaping the zombies:
void reap_children() {
	// TODO: Handle zombie children.
	// Add code here to deal with zombie children. You should
	// use the printf below to provide the proper output.
	printf("SERVER: Handler reaped child %d\n", (int) pid);
}

// The handler that is invoked when a child process exits:
void handler(int sig) {
	printf("SERVER: Handler invoked.\n");
	reap_children();
}

// Simulates random work. We just sleep for 0 < r < 5 seconds:
void do_random_work() {
	int r = rand() % 5;
	sleep(r);
}

// Simulates an accept of an HTTP request:
void accept() {
	printf("SERVER: Waiting for request.\n");
	int r = rand() % 1;
	sleep(r);
}

// This function simulates a child process handling a request:
void service_req(int rn) {
	printf("CHILD: Child %d handling request %d.\n", getpid(), rn);
	do_random_work();
	printf("CHILD: Child %d completed request %d.\n", getpid(), rn);
	exit(0);
}

// The server function that simulates a long running process that
// accepts incoming requests and passes the work along to a child
// process to deal with it:
void server(int numreq) {
	// TODO: Implement server routine.
	// Here you need to add code for simulating the server. The
	// important parts are as follows:
	//
	// 1. Keep track of the number of requests received.
	// 2. Create a main loop that will loop `numreq` times.
	// 3. If the `child_count` is equal to MAX_CHILDREN the server
	//    should call `pause()`. The SIGCHLD signal will cause the
	//    `pause()` function to return.
	// 4. You must call the `accept()` function.
	// 5. Make sure to increment the `child_count` variable.
	// 6. You must call `fork()` to create the child processes and
	//    then call `service_req(rn)` to service the request in the
	//    child.
	// 7. Make sure you call `reap_children()` as the final clean up
	//    of the server function after you break out of the main
	//    server loop.

	// Here are the `printf()` calls you should use to generate your
	// final output:

	//	printf("SERVER: Max child reached. Waiting...\n");
	//	printf("SERVER: Received request %d.\n", rn);
	//	printf("SERVER: completed %d requests.\n", rn);
}

// The main entry point:
int main(int argc, char *argv[]) {
	srand(time(0));  	// Initializes the random number generator.
	server(50);			// Invokes the server to run on 50 requests.
	return 0;
}
