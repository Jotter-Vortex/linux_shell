/*
	smsh1.c -small-shell version 1
		first really useful version after prompting shell
		this one parses the command line into strings
		uses fork, exec, wait, and ignores signals

	Writer : ST, Chang-Jin Lee
*/
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include"smsh.h"	// user - defined header file

#define DFL_PROMPT ">"

int main() {
	int pid;
	int child_info = -1;
	int i=0;
	char* cmdline, * prompt, ** arglist;
	int result, process(char**);
	void setup();	// ignore signals

	prompt = DFL_PROMPT;
	setup();

	while ((cmdline = next_cmd(prompt, stdin)) != NULL) {
		if (strchr(cmdline, '&') != NULL) {
			while (cmdline[i]) i++;
			cmdline[i - 1] = '\0';
			if ((pid = fork()) == -1)
				perror("fork");
			else if (pid == 0) {
				if ((arglist = splitline(cmdline)) != NULL) {
					result = process(arglist);
					freelist(arglist);
				}
				else {
					if ((arglist = splitline(cmdline)) != NULL) {
						result = process(arglist);
						freelist(arglist);
					}
				}
				free(cmdline);
			}

		}
		else {
			if ((arglist = splitline(cmdline)) != NULL) {
				result = process(arglist);
				freelist(arglist);
			}
		}
		
	}
	return 0;
}
#include"varlib.h"	// user - defined header file

void setup() {
	//		purpose: initialize shell
	//		returns : nothing. colls fatal() if possible
	extern char** environ;
	VLenviron2table(environ);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char* s1, char* s2, int n) {
	//purpose : print out error and terminate
	fprintf(stderr, "Error: %s, %s\n", s1, s2);
	exit(n);
}