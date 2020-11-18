/*
	controlflow.c
	- 'if' processing is done with two state varibales
	- if_state and if_result
*/
#include<stdio.h>
#include<string.h>
#include"smsh.h"
enum states {NEUTRAL, WANT_THEN, THEN_BLOCK}; // encoding string with int
enum results {SUCCESS, FAIL};
static int if_state = NEUTRAL;
static int if_result = SUCCESS;
static int last_stat = 0;
int syn_err(char*);
//	determine the shell should execute a command
//	returns : 1 for yesm 0 for no
//	details : if in THEN_BLOCK and if_result was SUCCESS then yes
//	 if in THEN_BLOCK and if_result was FAIL then no
//	 if in WANT_THEN and syntax error (sh is different)
int ok_to_execute() {
	int rv = 1;				// default is positive
	if (if_state == WANT_THEN) {
		syn_err("then expected");
		rv = 0;
	}
	else if (if_state == THEN_BLOCK && if_result == SUCCESS)
		rv = 1;
	else if (if_state == THEN_BLOCK && if_result == FAIL)
		rv = 0;
	return rv;
}

// boolean to report if the command is a shell control command
// return 0 or 1
int is_control_command(char* s) {
	return (strcmp(s, "if") == 0 || strcmp(s, "then") == 0 || strcmp(s, "fi") == 0);
}

// process "if", "then", "fi", - change state or detect error
// return 0 if ok -1 for syntax error
int do_control_command(char** args) {
	char* cmd = args[0];
	int rv = -1;
	if (strcmp(cmd, "if") == 0) {
		if (if_state != NEUTRAL) {	//expect neutral (when we see if)
			rv = syn_err("if unexpected");
		}else {
			last_stat = process(args + 1); // evaluate if
			if_result = (last_stat == 0 ? SUCCESS : FAIL); // nothing
			if_state = WANT_THEN; // update the (next) happend?
			rv = 0;
		}
	}else if (strcmp(cmd, "then") == 0) {
		if (if_state != WANT_THEN) {
			rv = syn_err("then unexpected");
		}else {
			if_state = THEN_BLOCK; // update the state
			rv = 0;
		}
	}
	// else if (strcmp(cmd,"else"==0)){
	//			how to handle the else part?
	//}
	else if (strcmp(cmd, "fi") == 0) {	// expect then_block
		if (if_result != THEN_BLOCK) {
			printf("%s",cmd);
			rv = syn_err("fi unexpected");
		}else {
			if_state = NEUTRAL; // update the next state
			rv = 0;
		}
	}
	else
		fatal("internal error processing", cmd, 2);
	return rv;
}

//  handles syntax errors in control structures
//  resets state to NEUTRAL
//  returns : -1 in interactive mode. Should call fatal in scripts
int syn_err(char* msg) {
	if_state = NEUTRAL;	//back to neutral
	fprintf(stderr, "syntax error : %s\n", msg);
	return -1;
}