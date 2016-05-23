/**
* @file decode.c
* @brief Funções de descodificação
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/
#include "decode.h"
#include "debug.h"
/**
 * @brief descodificar ficheiro Z2 ou Z5
 * @details descodifica ficheiros do tipo Z2 e Z5
 * 
 * @param dic ficheiro dicionario
 * @param cod ficheiro codificado
 * 
 * @return 0 OK 1 - falha
 */
int decodeFile(dic_t *dic, cod_t *cod) {
	FILE *stream;
	fpos_t pos;
	pgm_t pgmFile;
	//errors
	if ((dic->header.rows != cod->header.block_width) || (dic->header.columns != cod->header.block_height)) return ERR_DECODE_FILE;
	pgmFile.pixels = MALLOC(sizeof(unsigned short *) * cod->header.width);
	unsigned short *data = MALLOC(sizeof(unsigned short) * cod->header.width * cod->header.height);
	unsigned int i;
	for (i = 0; i < cod->header.width; ++i) {
		pgmFile.pixels[i] = &data[i * cod->header.height];
	}
	if (pgmFile.pixels[0] == NULL) {
		return ERR_ALLOC_MEMORY;
	}
	unsigned int lines = cod->header.width / cod->header.block_width;
	unsigned int columns = cod->header.height / cod->header.block_height;
	char version[4];
	char name[MAX_FILENAME + 1];
	removeExtension(cod->header.filename);
	sprintf(name, "%s.pgm", cod->header.filename);
	stream = fopen(name, "w");

	//escreve a versão no ficheiro
	sprintf(version, "P%d\n", cod->header.type);
	fprintf(stream, version);

	//escreve largura e altura
	fprintf(stream, "%u %u\n", cod->header.width, cod->header.height);
	fgetpos (stream, &pos);//grava a posicao da stream, para mais tarde adicionar o valor correcto do max_value
	fprintf(stream, "%u\n", cod->header.max_value);

	unsigned int j, a;
	unsigned int max_value = 0;
	//read files to array[][]
	for (i = 0; i < lines; i++) {
		for (j = 0; j < columns; j++) {
			for (a = 0; a < dic->header.rows; a++) {
				unsigned short value = dic->blocks[cod->indexes[i][j]][a];
				if (value > max_value) {
					max_value = value;
				}
				if (cod->header.type == Z2) {
					fprintf(stream, "%d\n", value);
				} else if (cod->header.type == Z5) {
					pgmFile.pixels[i][j] = value;
				}
			}
		}
	}
	fsetpos(stream, &pos);
	fprintf(stream, "%u\n", max_value);
	//grava o array pgmFile para o ficheiro
	if (cod->header.type == Z5) {
		for (i = 0; i < lines; i++) {
			for (j = 0; j < columns; j++) {
				for (a = 0; a < dic->header.rows; a++) {
					unsigned short value = pgmFile.pixels[i][j];
					//printf("value: %d", pgmFile.pixels[i][j]);
					unsigned int data_size = (max_value <= 255) ? 1 : 2;
					fwrite(&value, data_size, 1, stream);
				}
			}

		}
	}
	//free para o array pgmFile
	FREE(pgmFile.pixels[0]);
	FREE(pgmFile.pixels);
	fclose(stream);
	return 0;
}


/**
 * @brief descodifica ficheiros de forma recursiva
 * @details descodifica ficheiros cod em qualquer pasta
 * 
 * @param folder_name pasta
 * @param dicFile ficheiro dicionario
 * 
 * @return 0- ok 1- falha
 */
int decode_dir(char *folder_name, dic_t *dicFile) {
	DIR *dir = opendir(folder_name);
	if (dir == NULL) {
		WARNING("opendir() failed: %s", folder_name);
		return ERR_OPEN_FAILED;
	}
	struct dirent entry, *result = NULL;
	while (readdir_r(dir, &entry, &result) == 0) {
		if (result == NULL) {
			break;
		}

		if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0) {
			continue;
		}

		struct stat info;
		char path[strlen(folder_name) + strlen(entry.d_name) + 2];
		sprintf(path, "%s/%s", folder_name, entry.d_name);
		if (lstat(path, &info) != 0) {
			ERROR(3, "lstat() failed for %s", path);
		}

		if (S_ISDIR(info.st_mode)) {
			decode_dir(path, dicFile);
		} else if (S_ISREG(info.st_mode)) {
			if (getExtension(entry.d_name, COD)) {
				printf("%s\n ", path);
				cod_t codFile;
				int infoCod = open_file_cod(path, &codFile);
				if (infoCod == 0) {
					WARNING("teste %s", dicFile->header.filename);
					decodeFile(dicFile, &codFile);
				}
			}
		}
	}
	closedir(dir);
	return 0;
}
