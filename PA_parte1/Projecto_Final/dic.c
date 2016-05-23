/**
* @file dic.c
* @brief Funções do dicionario
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/
#include "dic.h"
#include "debug.h"
#include "errors.h"
#include "common.h"
#include "memory.h"
/**
 * @brief abre ficheiro dicionario
 * 
 * @param path ficheiro
 * @param dicFile estrutura do dicionario
 * 
 * @return 0-ok 1-falha
 */
int open_file_dic(char *path, dic_t *dicFile) {
	FILE *stream = fopen(path, "r");
	if (stream == NULL) {
		WARNING("fopen() failed");
		return ERR_OPEN_FAILED;
	}
	//read header
	int status = read_header_dic(stream, path, &(dicFile->header));
	if (status != 0) {
		fclose(stream);
		return status;
	}
	status = read_body_dic(stream, dicFile);
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
int read_header_dic(FILE *stream, const char *path, dic_header_t *header) {
	char *line;
	strncpy(header->filename, path, MAX_FILENAME + 1);
	//get number of block number
	line = read_line(stream);
	int status = split_number_check(line, &(header->block_default));
	//free(line);
	if (status != 0) return status;
	if ((int)header->block_default<=0 || header->block_default >= 65536)return ERR_INVALID_DIC;
	//get rows
	line = read_line(stream);
	status = split_number_check(line, &(header->rows));
	//free(line);
	if (status != 0) return status;
	//get number of block number
	line = read_line(stream);
	status = split_number_check(line, &(header->columns));
	//free(line);
	if (status != 0) return status;
	return 0;
}
/**
 * @brief ler corpo do dicionario
 * 
 * @param stream 
 * @param dicFile 
 * 
 * @return 0-ok 1-falha
 */
int read_body_dic(FILE *stream, dic_t *dicFile) {
	dicFile->blocks = MALLOC(sizeof(unsigned short *) * dicFile->header.block_default);
	unsigned short *data = MALLOC(sizeof(unsigned short) * dicFile->header.block_default * (dicFile->header.rows * dicFile->header.columns));
	unsigned int i;
	for (i = 0; i < dicFile->header.block_default; ++i) {
		dicFile->blocks[i] = &data[i * (dicFile->header.rows * dicFile->header.columns)];
	}
	if (dicFile->blocks[0] == NULL) {
		return ERR_ALLOC_MEMORY;
	}
	//ler os blocos do ficheiro
	int counterPixels = 0;
	int indexCounter = 0;
	int lineNumber = 0;
	char *line = NULL;
	while (strcmp((line = read_line(stream)), "EOF") != 0) {
		char *token;
		char *endptr = NULL;
		errno = 0;
		token = strtok(line, "  \t");
		while (token != NULL) {
			long value = strtol(token, &endptr, 10);
			token = strtok(NULL, " \t");
			if (*endptr != '\0' && *endptr != '\n') {
				return ERR_INVALID_DIC;
			}
			if (errno != 0) {
				return ERR_INVALID_DIC;
			}
			if ((int)value < 0 || (int)value >= 65536) return ERR_INVALID_DIC;

			if ((unsigned int)indexCounter >= (dicFile->header.rows * dicFile->header.columns) && token != NULL) {
				indexCounter = 0;
				lineNumber++;
			}
			if ((unsigned int)counterPixels >= (dicFile->header.block_default) * (dicFile->header.columns * dicFile->header.rows)) return ERR_INVALID_DIC;
			dicFile->blocks[lineNumber][indexCounter] = (unsigned short) value;
			indexCounter++;
			counterPixels++;
		}
		FREE(token);
		if ((unsigned int)indexCounter < (dicFile->header.rows * dicFile->header.columns)) return ERR_MISSING_INDEX_DIC;
		//free(line);
	}
	if ((unsigned int)counterPixels == (dicFile->header.block_default) * (dicFile->header.rows * dicFile->header.columns)) return 0;
	if ((unsigned int)counterPixels < (dicFile->header.block_default) * (dicFile->header.columns * dicFile->header.rows) &&
	        ( unsigned int)lineNumber < dicFile->header.block_default) return ERR_MISSING_BLOCK_DIC;
	return ERR_INVALID_DIC;
}
/**
 * @brief dividir os numeros
 * 
 * @param line 
 * @param int 
 * 
 * @return 0-ok outro - falha
 */
int split_number_check(char *line, unsigned int *number) {
	char *token;
	char *endptr = NULL;
	errno = 0;
	int counter = 0;
	token = strtok(line, "  \t");
	while (token != NULL) {
		counter++;
		if (counter > 1) return ERR_INVALID_DIC;
		long value = strtol(token, &endptr, 10);
		if ((int)value < 0) return ERR_INVALID_DIC;
		*number = (unsigned int)value;
		if (*endptr != '\0' && *endptr != '\n') {
			FREE(token);
			return ERR_INVALID_DIC;
		}
		if (errno != 0) {
			FREE(token);
			return ERR_INVALID_DIC;
		}
		token = strtok(NULL, " \t");
	}
	FREE(token);
	return 0;
}