#pragma once

typedef void (*xcommand_t)(void);

typedef struct cmd_function_s
{
	struct cmd_function_s* next;
	char* name;
	xcommand_t function;
} cmd_function_t;