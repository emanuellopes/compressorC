#ifndef __PGM_H__
#define __PGM_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "common.h"

// pgm struct
#define MAX_FILENAME 511
typedef enum {P2 = 2, P5 = 5} file_pgm_t;

typedef struct {
	file_pgm_t type;
	char filename[MAX_FILENAME + 1]; // +1 => \0
	unsigned int width;
	unsigned int height;
	unsigned int max_value;
} header_pgm_t;

typedef struct {
	header_pgm_t header;
	unsigned short **pixels;
} pgm_t;

int open_file_pgm(char *path, pgm_t *pgmFile);
int read_header_pgm(FILE *stream, const char *path, header_pgm_t *header);
int is_pgm_file(char *line, unsigned int *version);
int read_body_pgm(FILE *stream, pgm_t *pgmFile);
#endif



