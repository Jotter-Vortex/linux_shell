/*
	builtin.c
	 - contains the switch and the
	 - functions for builtin cmds
*/
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include"smsh.h"
#include"varlib.h"

int assign(char*); // for assignment cmd
int okname(char*); // check legal name

//		purpose: run s builtin command
//		returns : 1 if args[0] is built - in, 0 if not
//		details : test args[0] against all known built - ins.
//			call functions
int builtin_command(char** args, int* resultp) {
	int rv = 0;
	if (strcmp(args[0], "set") == 0) { // is this 'set'?
		VLlist(); // we call 'set'
		*resultp = 0;
		rv = 1;
	}
	else if (strchr(args[0], '=') != NULL) { // assignment cmd
		*resultp = assign(args[0]); // assignment
		if (*resultp != -1) rv = 1; // x-y = 123 not ok!
	}
	else if (strcmp(args[0], "export") == 0) {
		if (args[1] != NULL && okname(args[1]))
			*resultp = VLexport(args[1]);
		else *resultp = 1;
		rv = 1;
	}
	return rv;
}

//	purpose : excute name = val AND ensure that name is legal
//	returns : -1 for illegal lval, or result of VLstore
//	warning : modifies the string, but restores it to normal
int assign(char* str) {
	char* cp;
	int rv;
	// strchr : returns a pointer to the first occurrence of '='
	cp = strchr(str, '=');
	*cp = '\0'; // parse var name
	rv = (okname(str) ? VLstore(str, cp + 1) : -1);
	*cp = '=';
	return rv;
}

//		purpose : determines if a string is a legal var name
//		returns : 0 for no, 1 for yes
int okname(char* str) {
	char* cp;
	for (cp = str; *cp; cp++) {
		// check var name below
		if ((isdigit(*cp) && cp == str) || !( isalnum(*cp) || *cp == '_')) return 0;
	}
	return (cp != str);		// no empty strings, either
}