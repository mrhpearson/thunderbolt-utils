#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "utils.h"

#define PAGE_SIZE		sysconf(_SC_PAGE_SIZE)
#define GET_ALIGNED_PAGE(x, a)	_GET_ALIGNED_PAGE(x, (typeof(x))(a) - 1)
#define _GET_ALIGNED_PAGE(x, a)	(((x) + (a)) & ~(a))

struct list_item* list_add(struct list_item *tail, const void *val)
{
	struct list_item *temp = malloc(sizeof(struct list_item));

	temp->val = val;
	temp->next = NULL;

	if (tail == NULL)
		return temp;

	tail->next = temp;

	return temp;
}

int strpos(char *str, char *substr, const u32 offset)
{
	char strnew[strlen(str)];
	char *pos_str;
	int pos;

	strncpy(strnew, str + offset, strlen(str) - offset);
	strnew[strlen(str) - offset] = '\0';

	pos_str = strstr(strnew, substr);

	if (pos_str)
		pos = pos_str - (strnew + offset);
	else
		pos = -1;

	return pos;
}

char* do_bash_cmd(const char *cmd)
{
	char *output = malloc(MAX_LEN * sizeof(char));
	FILE *file = popen(cmd, "r");

	fgets(output, MAX_LEN, file);

	pclose(file);

	return trim_white_space(output);
}

struct list_item* do_bash_cmd_list(const char *cmd)
{
	char *output = malloc(MAX_LEN * sizeof(char));
	struct list_item *head = NULL;
	FILE *file = popen(cmd, "r");

	struct list_item *tail = head;

	while (fgets(output, MAX_LEN, file) != NULL) {
		char *temp_output = malloc(MAX_LEN * sizeof(char));

		output = trim_white_space(output);

		if (tail == NULL) {
			head = list_add(tail, (void*)output);;
			tail = head;

			output = temp_output;
			continue;
		}

		tail = list_add(tail, (void*)output);

		output = temp_output;
	}

	pclose(file);

	return head;
}

char* trim_white_space(char *str)
{
	char *end;

	while (isspace((unsigned char)*str))
		str++;

	if (*str == 0)
		return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end))
		end--;

	*++end = '\0';

	return str;
}

char* switch_cmd_to_root(const char *cmd)
{
	char *cmd_to_return = malloc(MAX_LEN * sizeof(char));
	char cmd_as_root[MAX_LEN];

	snprintf(cmd_as_root, sizeof(cmd_as_root), "sudo bash -c \"%s\"", cmd);

	strncpy(cmd_to_return, cmd_as_root, sizeof(cmd_as_root));
	cmd_to_return[sizeof(cmd_as_root)] = '\0';
	return cmd_to_return;
}

static bool is_page_aligned(const u64 off)
{
	return !off || ((PAGE_SIZE % off) == 0);
}

u64 get_page_aligned_addr(const u64 off)
{
	if (is_page_aligned(off))
		return off;

	return GET_ALIGNED_PAGE(off, PAGE_SIZE);
}

void* get_user_mapped_read_va(int fd, u64 off, u64 size)
{
	return mmap(NULL, size, PROT_READ, MAP_SHARED, fd, off);
}

void* get_user_mapped_write_va(int fd, u64 off, u64 size)
{
	return mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, off);
}

void* get_user_mapped_rw_va(int fd, u64 off, u64 size)
{
	return mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, off);
}

void unmap_user_mapped_va(void *addr, u64 size)
{
	munmap(addr, size);
}
