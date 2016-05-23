/**
* @file cod.c
* @brief ficheiro com funções para ler os ficheiros .COD
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/
#include "cod.h"
#include "errors.h"
#include "common.h"
#include "debug.h"
#include "memory.h"

/**
 * @brief Abrir ficheiro cod
 *
 * @param path - caminho do ficheiro
 * @param codFile estrutura do ficheiro
 *
 * @return !=0 in case os error
 */
int open_file_cod(char *path, cod_t *codFile) {
	FILE *stream = fopen(path, "r");
	if (stream == NULL) {
		WARNING("fopen() failed");
		return ERR_OPEN_FAILED;
	}
	//read header
	int status = read_header_cod(stream, path, &(codFile->header));
	if (status != 0) {
		fclose(stream);
		return status;
	}
	//read body
	status = read_body_cod(stream, codFile);
	if (status != 0) {
		fclose(stream);
		return status;
	}
	fclose(stream);
	return 0;
}
/**
 * @brief lê o cabeçalho do ficheiro
 *
 * @param stream - ficheiro
 * @param path  - name of file
 * @param header - estrutura do ficheiro
 * @return 0=Succefull ou !=0 em caso de erro
 */
int read_header_cod(FILE *stream, char *path, header_cod_t *header) {
	char *line = NULL;
	strncpy(header->filename, path, MAX_FILENAME + 1);
	WARNING("teste");
	//versao
	line = read_line(stream);
	int versiontmp = is_cod_file(line, &(header->type));
	free(line);
	if (versiontmp) return versiontmp;

	//largura e altura
	line = read_line(stream);
	if (!get_size(line, &(header->width), &(header->height))) {
		return ERR_INVALID_SIZE;
	}
	free(line);

	//valor maximo
	line = read_line(stream);
	if (!max_value(line, &(header->max_value))) return ERR_INVALID_MAX_VALUE;
	free(line);
	if ((header->max_value < 1) || (header->max_value >= 65536)) return ERR_INVALID_MAX_VALUE;

	//tamanho dos blocos
	line = read_line(stream);
	if (!get_cod_blocks(line, &(header->block_width), &(header->block_height))) {
		return ERR_INVALID_BLOCK_W_H;
	}
	FREE(line);
	//verifica se os blocos estão correctos
	int checkBls = check_blocks(header->width, header->height, header->block_width, header->block_height);
	if(checkBls) return checkBls;
	return 0;
}
/**
 * @brief lê o corpo do ficheiro
 * 
 * @param stream 
 * @param codFile 
 * 
 * @return !=0 erro 0=ok
 */
int read_body_cod(FILE *stream, cod_t *codFile) {
	int lines = codFile->header.width / codFile->header.block_width;
	int columns = codFile->header.height / codFile->header.block_height;
	//alloca memoria
	codFile->indexes = MALLOC(sizeof(unsigned short *) * codFile->header.width);
	unsigned short *data = MALLOC(sizeof(unsigned short) * codFile->header.width * codFile->header.height);
	unsigned int i;

	for (i = 0; i < codFile->header.width; ++i) {
		codFile->indexes[i] = &data[i * codFile->header.height];
	}
	if (codFile->indexes == NULL) {
		WARNING("Filed to alloc memory");
		return ERR_ALLOC_MEMORY;
	}
	int counterBlocks = 0;
	//lê os ficheiros do tipo Z2
	if (codFile->header.type == Z2) {
		int columnsNumber = 0;
		int lineNumber = 0;
		char *line=NULL;
		//lê as linhas do ficheiro
		while (strcmp((line = read_line(stream)), "EOF") != 0) {
			char *token;
			char *endptr = NULL;
			errno = 0;
			token = strtok(line, SPACE);
			while (token != NULL) {
				long value = strtol(token, &endptr, 10);
				if (*endptr != '\0' && *endptr != '\n') {
					return ERR_INVALID_BLOCKS_NUMBER;
				}
				if (errno != 0) {
					return ERR_INVALID_BLOCKS_NUMBER;
				}
				if (value < 0 || value > (codFile->header.max_value)) return ERR_INVALID_BLOCKS_NUMBER;

				codFile->indexes[lineNumber][columnsNumber] = (unsigned int)value;
				columnsNumber++;
				counterBlocks++;
				if (columnsNumber >= columns) {
					columnsNumber = 0;
					lineNumber++;
				}
				token = strtok(NULL, SPACE);
			}
			FREE(line);
			FREE(token);
		}
		if ((lines * columns) == counterBlocks)return 0;
		//lê os ficheiros do tipo Z5
	} else if (codFile->header.type == Z5) {
		int i, j;
		unsigned int index = 0;
		for (i = 0; i < lines; i++) {
			for (j = 0; j < columns; j++) {
				counterBlocks++;
				unsigned int data_size = (codFile->header.max_value <= 255) ? 1 : 2;
				if (fread(&index, data_size, 1, stream) != 1) {
					WARNING("saiu aqui cod 2");
					return ERR_INVALID_BLOCKS_NUMBER;
				}
				if ((short)(index) < 0 || (short)(index) > (int)codFile->header.max_value) {
					return ERR_INVALID_BLOCKS_NUMBER;
				}
				codFile->indexes[i][j] = index;
			}
		}
		//verifica se tem valores a mais no ficheiro
		char ch;
		fread(&ch, 1, 1, stream);
		if (!feof(stream)) {
			return ERR_INVALID_BLOCKS_NUMBER;
		}
		if ((lines * columns) == counterBlocks) return 0;
	}
	return ERR_INVALID_BLOCKS_NUMBER;
}

/**
 * @brief verifica e devolve a versao do ficheiro Z2 ou Z5
 *
 * @param line - linha do ficheiro de texto
 * @param version - versao do ficheiro
 *
 * @return 0-succefull, !=0 em caso de erro
 */
int is_cod_file(char *line, file_cod_t *version) {
	char *token;
	int counter = -1;
	token = strtok(line, SPACE);
	while (token != NULL) {
		counter++;
		token = strtok(NULL, SPACE);
	}
	FREE(token);
	if (counter > 0)return ERR_MISSING_HEADER;
	if (line[0] == 'Z') {
		if (strcmp(line, "Z2") == 0) {
			*version = Z2;
			return 0;
		} else if (strcmp(line, "Z5") == 0) {
			*version = Z5;
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
 * @brief grava os bloco largura e altura
 *
 * @param line linha do ficheiro de texto
 * @param width - largura
 * @param height - altura
 * @return 1=succefull 0=error
 */
int get_cod_blocks(char *line, unsigned int *width, unsigned int *height) {
	return get_size(line, width, height);
}
/**
 * @brief verifica se os blocos são correctos
 *
 * @param width	 largura da imagem
 * @param height altura da imagem
 * @param blockRows largura dos blocos
 * @param blockColumns altura dos blocos
 * @return 0=succefull and !=0 is error
 */
int check_blocks(int width, int height, int blockRows, int blockColumns) {
	if (width % blockRows != 0 || height % blockColumns != 0)
		return ERR_INVALID_BLOCK_MISMATCH;
	if ((width * height) % (blockRows * blockColumns))
		return ERR_INVALID_BLOCKS_NUMBER;
	return 0;
}