#include <kernel/interp/interp.h>

#include <kernel/shell.h>
#include <kernel/kernel.h>
#include <kernel/driver/fat32.h>
#include <kernel/system/window_manage.h>
#include <kernel/cpu/timer.h>

#include <string.h>
#include <stdlib.h>

variable_t** array_args = 0;
int array_args_count = 0;
int pops = 0;

variable_t** variables = 0;
int variable_count = 0;

label_t** labels = 0;
int label_count = 0;

int* stack = 0;
int stack_count = 0;

int should_execute = 1;
int program_executing = 0;

char input_buf[1024];

int comparison = EQUAL_TO;

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
	} else if (buf[0] == '&') {
		// Process a pointer variable
		var->type = "PTR";
		int* dat = malloc(sizeof(int));
		*dat = atoi(buf+1);
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

void interp_clear_args_hard() {
	if (array_args_count != 0) {
		for (int i = 0; i < array_args_count; i++) {
			if (array_args[i]->name == 0) {
				free(array_args[i]->name);
				free(array_args[i]->type);
				free(array_args[i]->data);
				free(array_args[i]);
			}
		}
		array_args_count = 0;
		free(array_args);
	}
	pops = 0;
}

void interp_clear_args() {
	interp_clear_args_hard();
}

void interp_clear_vars() {
	if (variable_count != 0) {
		for (int i = 0; i < variable_count; i++) {
			free(variables[i]->name);
			free(variables[i]->type);
			free(variables[i]->data);
			free(variables[i]);
		}
		variable_count = 0;
		free(variables);
	}
}

void interp_clear_labels() {
	if (label_count != 0) {
		for (int i = 0; i < label_count; i++) {
			free(labels[i]);
		}
		label_count = 0;
		free(labels);
	}
}

void interp_clear_stack() {
	free(stack);
	stack_count = 0;
}

int interp_process(unsigned char* cmd, int x) {
	if (strlen(cmd) == 0) {
		return x + 1;
	}

	char* cmd1 = strtok(cmd, " ");
	char* arg1 = strtok(0, "");

	while (*cmd1 == '\t') cmd1++;
	while (*cmd1 == 0xA0) cmd1++;

	if (*cmd == '#') {
		return x+1;
	}

	if (strcmp(cmd1, "print") == 0 && should_execute) {
		variable_t* var_arg = interp_process_variable(arg1);

		if (strcmp(var_arg->type, "STRING") == 0) {
			printf(var_arg->data);
			printf("\n");
		} else if (strcmp(var_arg->type, "FLOAT") == 0) {
			printf("%f\n", *(float*)var_arg->data);
		} else if (strcmp(var_arg->type, "PTR") == 0) {
			printf("%d\n", *(int*)var_arg->data);
		}

		free(var_arg);

		interp_clear_args();
	} else if (strcmp(cmd1, "push") == 0 && should_execute) {
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
	} else if (strcmp(cmd1, "pop") == 0 && should_execute) {
		if (array_args_count != 0) {
			variable_t* var_arg = interp_process_variable(arg1);
			var_arg->data = array_args[array_args_count - 1]->data;
			array_args_count--;
			if (array_args_count == 0) {
				array_args_count = pops+1;
				interp_clear_args();
			}
			pops ++;
		}
	} else if (strcmp(cmd1, "string") == 0 && should_execute) {
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
	} else if (strcmp(cmd1, "float") == 0 && should_execute) {
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
	} else if (strcmp(cmd1, "ptr") == 0 && should_execute) {
		if (variable_count == 0) {
			variables = malloc(sizeof(variable_t*));
			variables[0] = malloc(sizeof(variable_t));
			variables[0]->name = arg1;
			variables[0]->type = "PTR";
			variable_count ++;
		} else {
			variable_t** temp = malloc(sizeof(variable_t*) * (variable_count+1));
			memcpy(temp, variables, variable_count*sizeof(variable_t*));
			free(variables);
			temp[variable_count] = malloc(sizeof(variable_t));
			temp[variable_count]->name = arg1;
			temp[variable_count]->type = "PTR";
			variables = temp;
			variable_count++;
		}
	} else if (strcmp(cmd1, "set") == 0 && should_execute) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0) {
			if (strcmp(var->type, "STRING") == 0) {
				var->data = array_args[0]->data;
			} else if (strcmp(var->type, "FLOAT") == 0) {		// Copy the value
				float* dat = malloc(sizeof(float));
				
				*dat = *(float*)array_args[0]->data;
				var->data = dat;
			} else if (strcmp(var->type, "PTR") == 0) {			// Copy the value
				int* dat = malloc(sizeof(int));
				
				*dat = *(int*)array_args[0]->data;
				var->data = dat;
			}
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "add") == 0 && should_execute) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0 && strcmp(var->type, "FLOAT") == 0) {
			float* dat = (float*)var->data;
			*dat += *(float*)array_args[0]->data;
			var->data = dat;
		} else if (var != 0 && strcmp(var->type, "PTR") == 0) {
			int* dat = (int*)var->data;
			*dat += *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "sub") == 0 && should_execute) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0 && strcmp(var->type, "FLOAT") == 0) {
			float* dat = (float*)var->data;
			*dat -= *(float*)array_args[0]->data;
			var->data = dat;
		} else if (var != 0 && strcmp(var->type, "PTR") == 0) {
			int* dat = (int*)var->data;
			*dat -= *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "mul") == 0 && should_execute) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0 && strcmp(var->type, "FLOAT") == 0) {
			float* dat = (float*)var->data;
			*dat *= *(float*)array_args[0]->data;
			var->data = dat;
		} else if (var != 0 && strcmp(var->type, "PTR") == 0) {
			int* dat = (int*)var->data;
			*dat *= *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "div") == 0 && should_execute) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}
		if (var != 0 && strcmp(var->type, "FLOAT") == 0) {
			float* dat = (float*)var->data;
			*dat /= *(float*)array_args[0]->data;
			var->data = dat;
		} else if (var != 0 && strcmp(var->type, "PTR") == 0) {
			int* dat = (int*)var->data;
			*dat /= *(float*)array_args[0]->data;
			var->data = dat;
		}

		interp_clear_args();
	} else if (strcmp(cmd1, "include") == 0 && should_execute) {
		unsigned char* buf = read_file_from_name(0, arg1);
		if (buf) {
				interp_clear_args();
				interp(buf);
				free(buf);
		} else {
			printf("WARNING: could not find file: %s\n", arg1);
		}
	} else if (strcmp(cmd1, "read") == 0 && should_execute) {
		if (array_args_count != 0) {
			unsigned char* buf = read_file_from_name(0, arg1);
			if (buf) {
				variable_t* var = array_args[0];
				
				if (strcmp(var->type, "STRING") == 0)
					var->data = (void*)buf;
			} else {
				printf("WARNING: could not find file: %s\n", arg1);
			}
		}
	} else if (strcmp(cmd1, "write") == 0 && should_execute) {
		if (array_args_count != 0) {
			int c = 0;
			directory_entry_t* entry = read_directory_from_name(0, arg1, &c);

			if (c == 1 && entry != 0) {
				int length = strlen((unsigned char*)array_args[0]->data);
				write_cluster(0, entry->first_cluster_low | (entry->first_cluster_high >> 16), (unsigned char*)array_args[0]->data, length);
			} else if (c == 0 && entry != 0) {
				// Add directory to parent and write to that.
				printf("WARNING: could not find file: %s\n", arg1);
			}
		}
	} else if (strcmp(cmd1, "move") == 0 && should_execute) {
		variable_t* var = 0;
		for (int i = 0; i < variable_count; i++) {
			if (strcmp(variables[i]->name, strtok(arg1, " \n")) == 0) {
				var = variables[i];
				break;
			}
		}

		if (var != 0 && strcmp(var->type, "PTR") == 0) {
			if (strcmp(array_args[0]->type, "FLOAT") == 0) 
				*(char*)*(int*)var->data = *(float*)array_args[0]->data;
			else if (strcmp(array_args[0]->type, "STRING") == 0) 
				*(char*)*(int*)var->data = *(unsigned char*)array_args[0]->data;
		}

		interp_clear_args();
	} else if (cmd1[strlen(cmd1)-1] == ':') {		// Label
		label_t* l = malloc(sizeof(label_t));
		unsigned char* name = malloc(strlen(cmd1));
		memcpy(name, cmd1, strlen(cmd1)-1);
		name[strlen(cmd1)-1] = '\0';

		l->name = name;
		l->line = x;

		if (label_count == 0) {
			labels = malloc(sizeof(label_t*));
			labels[0] = l;
			label_count++;
		} else {
			label_t** temp = malloc(sizeof(label_t*) * (label_count+1));
			memcpy(temp, labels, label_count*sizeof(label_t*));
			free(labels);
			temp[label_count] = l;
			labels = temp;
			label_count++;
		}

		should_execute = 0;
	} else if (strcmp(cmd1, "end") == 0) {
		if (stack_count != 0) {
			int next_line = stack[stack_count - 1] + 1;
			if (stack_count > 1) {
				int* test = malloc(sizeof(int) * stack_count - 1);
				memcpy(test, stack, sizeof(int) * stack_count - 1);
				free(stack);
				stack = test;
			} else free(stack);

			stack_count--;
			return next_line;
		} else {
			should_execute = 1;
		}
	} else if (strcmp(cmd1, "call") == 0 && should_execute) {
		for (int i = 0; i < label_count; i++) {
			if (strcmp(labels[i]->name, arg1) == 0) {
				if (stack_count == 0) {
					stack = malloc(sizeof(int));
					stack[0] = x;
					stack_count++;
				} else {
					int* temp = malloc(sizeof(int) * (stack_count+1));
					memcpy(temp, stack, stack_count*sizeof(int));
					free(stack);
					temp[stack_count] = x;
					stack = temp;
					stack_count++;
				}

				return labels[i]->line+1;
			}
		}
		printf("Could not find label!\n");
	} else if (strcmp(cmd1, "goto") == 0 && should_execute) {
		label_t* l = 0;
		for (int i = 0; i < label_count; i++) {
			if (strcmp(labels[i]->name, arg1) == 0) {
				return labels[i]->line+1;
			}
		}
		printf("Could not find label!\n");
	} else if (strcmp(cmd1, "comp") == 0 && should_execute) {
		float x = *(float*)array_args[0]->data;
		float y = *(float*)array_args[1]->data;

		if (x == y) {
			comparison = EQUAL_TO;
		} else if (x > y) {
			comparison = GREATER_THEN;
		} else {
			comparison = LESS_THEN;
		}
		interp_clear_args();
	} else if (strcmp(cmd1, "ce") == 0 && should_execute) {
		if (comparison == EQUAL_TO) {
			for (int i = 0; i < label_count; i++) {
				if (strcmp(labels[i]->name, arg1) == 0) {
					if (stack_count == 0) {
						stack = malloc(sizeof(int));
						stack[0] = x;
						stack_count++;
					} else {
						int* temp = malloc(sizeof(int) * (stack_count+1));
						memcpy(temp, stack, stack_count*sizeof(int));
						free(stack);
						temp[stack_count] = x;
						stack = temp;
						stack_count++;
					}

					return labels[i]->line+1;
				}
			}
			printf("Could not find label!\n");
		}
	} else if (strcmp(cmd1, "cgt") == 0 && should_execute) {
		if (comparison == GREATER_THEN) {
			for (int i = 0; i < label_count; i++) {
				if (strcmp(labels[i]->name, arg1) == 0) {
					if (stack_count == 0) {
						stack = malloc(sizeof(int));
						stack[0] = x;
						stack_count++;
					} else {
						int* temp = malloc(sizeof(int) * (stack_count+1));
						memcpy(temp, stack, stack_count*sizeof(int));
						free(stack);
						temp[stack_count] = x;
						stack = temp;
						stack_count++;
					}

					return labels[i]->line+1;
				}
			}
			printf("Could not find label!\n");
		}
	} else if (strcmp(cmd1, "clt") == 0 && should_execute) {
		if (comparison == LESS_THEN) {
			for (int i = 0; i < label_count; i++) {
				if (strcmp(labels[i]->name, arg1) == 0) {
					if (stack_count == 0) {
						stack = malloc(sizeof(int));
						stack[0] = x;
						stack_count++;
					} else {
						int* temp = malloc(sizeof(int) * (stack_count+1));
						memcpy(temp, stack, stack_count*sizeof(int));
						free(stack);
						temp[stack_count] = x;
						stack = temp;
						stack_count++;
					}

					return labels[i]->line+1;
				}
			}
			printf("Could not find label!\n");
		}
	} else if (strcmp(cmd1, "ge") == 0 && should_execute) {
		if (comparison == EQUAL_TO) {
			for (int i = 0; i < label_count; i++) {
				if (strcmp(labels[i]->name, arg1) == 0) {
					return labels[i]->line+1;
				}
			}
			printf("Could not find label!\n");
		}
	} else if (strcmp(cmd1, "ggt") == 0 && should_execute) {
		if (comparison == GREATER_THEN) {
			for (int i = 0; i < label_count; i++) {
				if (strcmp(labels[i]->name, arg1) == 0) {
					return labels[i]->line+1;
				}
			}
			printf("Could not find label!\n");
		}
	} else if (strcmp(cmd1, "glt") == 0 && should_execute) {
		if (comparison == LESS_THEN) {
			for (int i = 0; i < label_count; i++) {
				if (strcmp(labels[i]->name, arg1) == 0) {
					return labels[i]->line+1;
				}
			}
			printf("Could not find label!\n");
		}
	}

	free(cmd1);
	free(arg1);

	return x + 1;
}

void interp(unsigned char* buf) {
	program_executing = 1;

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
	while(commands[i] != 0 && i < 1024 && program_executing == 1) {
		i = interp_process(commands[i], i);

		if (program_executing == 0) break;

		sleep(1);
	}

	interp_clear_args_hard();
	interp_clear_vars();
	interp_clear_labels();
	interp_clear_stack();

	program_executing = 0;

	for (i = 0; i < 1024; i++) {
		free(commands[i]);
	}

	comparison = EQUAL_TO;
}
