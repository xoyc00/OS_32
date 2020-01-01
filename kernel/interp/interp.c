#include <kernel/interp/interp.h>

#include <kernel/shell.h>
#include <kernel/kernel.h>

#include <string.h>
#include <stdlib.h>

variable_t** array_args;
int array_args_count;

variable_t** variables;
int variable_count;

variable_t* interp_process_variable(unsigned char* buf) {
	variable_t* var = malloc(sizeof(variable_t));
	var->name = 0;
	if (buf[0] == '"') {
		// Process a string variable
		var->type = "STRING";
		var->data = strtok(buf+1, "\"");	
		return var;
	} else if (buf[0] == '$') {
		// Process a float variable
		var->type = "FLOAT";
		float* dat = malloc(sizeof(float));
		*dat = atof(buf+1);
		var->data = (void*)dat;
		return var;
	} else {
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, buf) == 0) {
				return variables[i];
			}
		}
	}

	return 0;
}

void interp_clear_args() {
	array_args_count = 0;
	free(array_args);
}

void interp_process(unsigned char* cmd) {
	char* cmd1 = strtok(cmd, " ");
	char* arg1 = strtok(0, "");

	if (cmd1[0] == '#') {
		return;
	}

	if (strcmp(cmd1, "call") == 0) {
		if (strcmp(arg1, "print") == 0) {
			if (array_args_count != 0 && strcmp(array_args[0]->type, "STRING") == 0) {
				printf(array_args[0]->data);
				printf("\n");
			} else {
				printf("%f\n", *(float*)array_args[0]->data);
			}
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "push") == 0) {
		variable_t* var_arg = interp_process_variable(arg1);

		if (array_args_count == 0) {
			array_args = malloc(sizeof(variable_t*));
			array_args[0] = var_arg;
			array_args_count ++;
		} else {
			variable_t** temp = malloc(sizeof(variable_t*) * (array_args_count+1));
			memcpy(temp, array_args, array_args_count*sizeof(variable_t*));
			free(array_args);
			temp[array_args_count] = var_arg;
			array_args = temp;
			array_args_count ++;
		}
	} else if (strcmp(cmd1, "string") == 0) {
		if (variable_count == 0) {
			variables = malloc(sizeof(variable_t*));
			variables[0] = malloc(sizeof(variable_t));
			variables[0]->name = arg1;
			variables[0]->type = "STRING";
			variable_count ++;
		} else {
			variable_t** temp = malloc(sizeof(variable_t*) * (variable_count+1));
			memcpy(temp, variables, variable_count*sizeof(variable_t*));
			free(variables);
			temp[variable_count] = malloc(sizeof(variable_t));
			temp[variable_count]->name = arg1;
			temp[variable_count]->type = "STRING";
			variables = temp;
			variable_count++;
		}
	} else if (strcmp(cmd1, "float") == 0) {
		if (variable_count == 0) {
			variables = malloc(sizeof(variable_t*));
			variables[0] = malloc(sizeof(variable_t));
			variables[0]->name = arg1;
			variables[0]->type = "FLOAT";
			variable_count ++;
		} else {
			variable_t** temp = malloc(sizeof(variable_t*) * (variable_count+1));
			memcpy(temp, variables, variable_count*sizeof(variable_t*));
			free(variables);
			temp[variable_count] = malloc(sizeof(variable_t));
			temp[variable_count]->name = arg1;
			temp[variable_count]->type = "FLOAT";
			variables = temp;
			variable_count++;
		}
	} else if (strcmp(cmd1, "set") == 0) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0) {
			var->data = array_args[0]->data;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "add") == 0) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0) {
			float* dat = (float*)var->data;
			*dat += *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "sub") == 0) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0) {
			float* dat = var->data;
			*dat -= *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "mul") == 0) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0) {
			float* dat = var->data;
			*dat *= *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "div") == 0) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0) {
			float* dat = var->data;
			*dat /= *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	}
}

void interp(unsigned char* buf) {
	int done = 0;
	char* commands[1024];
	int i = 0;
	commands[i] = strtok(buf, "\n");
	i++;
	while (done == 0) {
		char* cmd = strtok(0, "\n");
		if (cmd == 0) {
			done = 1;
			commands[i] = 0;
			i++;
			break;
		}
		commands[i] = cmd;
		i++;
	}
	i = 0;
	while (commands[i] != 0) {
		interp_process(commands[i]);
		i++;
	}
}
