#ifndef __COD_H__
#define __COD_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "common.h"

//tipo de ficheiros cod 
typedef enum {Z2 = 2, Z5 = 5} file_cod_t;

//header da estrutura
typedef struct {
	file_cod_t type;
	char filename[MAX_FILENAME + 1]; // +1 => \0
	unsigned int width;
	unsigned int height;
	unsigned int max_value;
	unsigned int block_width;
	unsigned int block_height;
} header_cod_t;

//estrutura do cod
typedef struct {
	header_cod_t header;
	unsigned short **indexes;
} cod_t;

/**
 * @brief prototipos
 */
int open_file_cod(char *path, cod_t *codFile);
int read_header_cod(FILE *stream, char *path, header_cod_t *header);
int read_body_cod(FILE *stream, cod_t *codFile);
int is_cod_file(char *typeFile, file_cod_t *version);
int get_cod_blocks(char *line, unsigned int *block1, unsigned int *block2);
int read_blocks_cod(FILE *stream, unsigned short *blocks, int sizeArray, unsigned int MaxValue);
int check_blocks(int rows, int columns, int blockRows, int blockColumns);
#endif