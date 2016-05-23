/**
* @file pgm.c
* @brief Funções relativas à leitura dos ficheiros pgm
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/
#include "pgm.h"
#include "debug.h"
#include "errors.h"
#include "common.h"
#include "memory.h"

/**
 * @brief abreo o ficheiro pgm
 * @details [long description]
 * 
 * @param path 
 * @param pgmFile 
 * 
 * @return !=0 falha
 */
int open_file_pgm(char *path, pgm_t *pgmFile) {
	FILE *stream = fopen(path, "r");
	if (stream == NULL) {
		WARNING("fopen() failed %s", path);
		return ERR_OPEN_FAILED;
	}
	//read header
	int status = read_header_pgm(stream, path, &(pgmFile->header));
	if (status != 0) {
		fclose(stream);
		return status;
	}
	//read body
	status = read_body_pgm(stream, pgmFile);
	if (status != 0) {
		fclose(stream);
		return status;
	}
	fclose(stream);
	return 0;
}
/**
 * @brief ĺê o cabeçalho do ficheiro
 * 
 * @param stream FILE stream
 * @param path nome do ficheiro
 * @param header estrutura do header
 * @return 0-ok 1-falha
 */
int read_header_pgm(FILE *stream, const char *path, header_pgm_t *header) {
	char *line = NULL;
	strncpy(header->filename, path, MAX_FILENAME + 1);
	//get version
	line = read_line(stream);
	int versiontmp = is_pgm_file(line, &(header->type));
	//FREE(line);
	if (versiontmp) return versiontmp;
	//get height and width of cod file
	line = read_line(stream);
	if (!get_size(line, &(header->width), &(header->height))) {
		//FREE(line);
		return ERR_INVALID_SIZE;
	}
	//get max Value
	line = read_line(stream);
	if (!max_value(line, &(header->max_value))) {return ERR_INVALID_MAX_VALUE;}
	//FREE(line);
	if (header->type == P2) {
		if ((header->max_value < 1) || (header->max_value >= 256)) {
			return ERR_INVALID_MAX_VALUE;
		}
	}
	else if (header->type == P5) {
		if ((header->max_value < 1) || (header->max_value >= 65536)) {
			return ERR_INVALID_MAX_VALUE;
		}
	}
	return 0;
}

/**
 * @brief verifica e devolve a versao do ficheiro P2 ou P5
 *
 * @param line - linha do ficheiro de texto
 * @param version - versao do ficheiro
 *
 * @return 0-succefull, !=0 em caso de erro
 */
int is_pgm_file(char *line, file_pgm_t *version) {
	char *token;
	int counter = -1;
	token = strtok(line, SPACE);
	while (token != NULL) {
		counter++;
		token = strtok(NULL, SPACE);
	}
	FREE(token);
	if (counter > 0)return ERR_MISSING_HEADER;
	if (line[0] == 'P') {
		if (strcmp(line, "P2") == 0) {
			*version = P2;
			return 0;
		} else if (strcmp(line, "P5") == 0) {
			*version = P5;
			return 0;
		} else {
			return ERR_INVALID_FORMAT;
		}
	} else {
		return ERR_MISSING_HEADER;
	}
	return ERR_MISSING_HEADER;
}
/**
 * @brief lê o corpo do ficheiro
 * 
 * @param stream 
 * @param codFile 
 * 
 * @return !=0 erro 0=ok
 */
int read_body_pgm(FILE *stream, pgm_t *pgmFile) {
	int counterPixels = 0;
	//allocate memory for pixels
	pgmFile->pixels = MALLOC(sizeof(unsigned short *) * pgmFile->header.width);
	unsigned short *data = MALLOC(sizeof(unsigned short) * pgmFile->header.width * pgmFile->header.height);
	unsigned int i;
	for (i = 0; i < pgmFile->header.width; ++i) {
		pgmFile->pixels[i] = &data[i * pgmFile->header.height];
	}
	if (pgmFile->pixels[0] == NULL) {
		return ERR_ALLOC_MEMORY;
	}
	//ficheiro p2
	if (pgmFile->header.type == P2) {
		int indexCounter = 0;
		int lineNumber = 0;
		char *line = NULL;
		while (strcmp((line = read_line(stream)), "EOF") != 0) {
			char *token;
			char *endptr = NULL;
			errno = 0;
			token = strtok(line, SPACE);
			while (token != NULL) {
				long value = strtol(token, &endptr, 10);
				if (*endptr != '\0' && *endptr != '\n') {
					return ERR_INVALID_PGM_FILE;
				}
				if (errno != 0) {
					return ERR_INVALID_PGM_FILE;
				}
				if (value < 0 || (unsigned int)value > pgmFile->header.max_value) {
					return ERR_INVALID_PGM_FILE;
				}
				if (counterPixels >= (int)(pgmFile->header.height * pgmFile->header.width)) return ERR_INVALID_PGM_FILE;
				if ((unsigned int)indexCounter >= pgmFile->header.height) {
					indexCounter = 0;
					lineNumber++;
				}
				pgmFile->pixels[lineNumber][indexCounter] = (unsigned short) value;
				indexCounter++;
				counterPixels++;
				token = strtok(NULL, SPACE);
			}
			FREE(token);
			//free(line);
		}
		if ((unsigned int)counterPixels == (pgmFile->header.width * pgmFile->header.height)) return 0;
		//lê ficheiros p5
	} else if (pgmFile->header.type == P5) {
		int i, j;
		unsigned int pixel = 0;
		for (i = 0; i < (int)pgmFile->header.width; i++) {
			for (j = 0; j < (int)pgmFile->header.height; j++) {
				counterPixels++;
				if (fread(&pixel, 1, 1, stream) != 1) {
					return ERR_INVALID_PGM_FILE;
				}
				if ((short)(pixel) < 0 || (short)(pixel) > (int)pgmFile->header.max_value) {
					return ERR_INVALID_PGM_FILE;
				}
				pgmFile->pixels[i][j] = pixel;
			}
		}
		char ch;
		fread(&ch, 1, 1, stream);
		if (!feof(stream)) {
			return ERR_INVALID_PGM_FILE;
		}
		if ((unsigned int)counterPixels == (pgmFile->header.width * pgmFile->header.height)) return 0;
	}
	return ERR_INVALID_PGM_FILE;
}