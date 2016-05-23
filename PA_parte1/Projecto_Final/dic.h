#ifndef __DIC_H__
#define __DIC_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "common.h"
#define MAX_FILENAME 511

/**
 * estrutura do dicionario
 */
typedef struct {
	char filename[MAX_FILENAME + 1];
	unsigned int block_default;
	unsigned int rows;
	unsigned int columns;
} dic_header_t;
/**
 * estrutura do header do dicionario
 */
typedef struct{
	dic_header_t header;
	unsigned short **blocks;
}dic_t;

/*
 * prototipos
 */
int open_file_dic(char *path, dic_t *dicFile);
int read_header_dic(FILE *stream, const char *path, dic_header_t *header);
int read_body_dic(FILE *stream, dic_t *dicFile);
int split_number_check(char *line, unsigned int *number);
#endif