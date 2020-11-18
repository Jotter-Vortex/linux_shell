/*
process.c : command processing layer
The process (char **arglist) function is called by the main loop
It sits in front the execute() function. This layer handles
two main classes of processing.
	a) built-in functions (e.g. exit(), set, =, read, ..)
	b) control sturctures (e.g. if, while, for)

Writer : St. Chang-jin Lee
	
*/
#include<stdio.h>
#include "smsh.h"
// is a boolean-valued funvtion
// that tells process if the command is
// part of the shell programming lang. or
// if the command is something to execute
int is_control_command(char*);
// handles the keywords, if, then, and fi.
// (Each word is the boundary btw. two states.)
// updates the state variables and
// performs any appropriate operations.
int do_control_command(char**);
// checks the current state and
// the result of the confition command
// and returns a boolean value to indicate
// if the current command should be executed
int ok_to_execute();
/*
	purpose : process user command
	returns : result of processing command
	details : if a built-in then call appropriate function,
	if not execute()
	errors : arise from subroutines, handled there
*/
int process(char** args) {
	int rv = 0;
	if (args[0] == NULL) rv = 0;
	else if (is_control_command(args[0]))
			rv = do_control_command(args);
	else if (ok_to_execute())	//execute commands?
			rv = execute(args);
	return rv;
}