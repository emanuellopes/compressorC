#ifndef __COMMON_H__
/**
 * defines
 */
#define __COMMON_H__
#define SPACE " "
#define MAX_FILENAME 511

#define COMMA ","
#define COD "cod"
#define DIC "dic"
#define PGM "pgm"
/**
 * includes de outros ficheiros
 */
#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/**
 * @brief variavel global que verifica se o programa ainda se encontra a correr
 */
extern int running;

/**
 * prototypes of common.c file
 */

int get_size(char *line, unsigned int *rows, unsigned int *columns);
int max_value(char *line, unsigned int *maxValue);
int remove_comments(char *line, ssize_t size);
char  *read_line(FILE *stream);
char *trimwhitespace(char *str);
int getExtension(char *filename, char *extension);
void install_signal_handler(void);
void removeExtension(char *filename);
void install_signal_handler(void);
void getDate(void);
#endif