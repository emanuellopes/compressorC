/**
* @file encode.c
* @brief Function to handle COD files
* @date 2015-12-15
* @author 2140825@ipleiria.pt
*/

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include "encode.h"
#include "errors.h"
#include "debug.h"
#include "memory.h"
#include "signal_handler.h"

#define MAX_FILENAME 512

/**
 * @brief main function for initiate structure encode
 *
 * @param structencode struct of encode file
 * @param pathDic filename path of dictionary
 * @param pathPgm filename path of pgm
 * @param parallel check if for parallel encode
 * @param threads number of threads
 * @return int status code
 */
int encode_init(struct_encode_t *structencode, char *pathDic, char *pathPgm) {
	//DEBUG("encode init");
	//init pgm and dic vars
	structencode->pgm = pgm_init();
	structencode->dic = dic_init();
	structencode->max_value = 0;
	//parse dic and pgm files to respective structures
	int infoimage = pgm_parse_file(pathPgm, &(structencode->pgm));
	int infodict = dic_parse_file(pathDic, &(structencode->dic));
	if (infoimage == 0 && infodict == 0) {
		//verification of blocks pgm and dict
		if (check_blocks(structencode->pgm.header.width, structencode->pgm.header.height, structencode->dic.block_width, structencode->dic.block_height))
			return COD_ERR_INVALID_BLOCKS_NUMBER;

		//total of blocks
		structencode->total_blocks = (structencode->pgm.header.width / structencode->dic.block_width) * (structencode->pgm.header.height / structencode->dic.block_height);
		//DEBUG("total_blocks: %d\n", structencode->total_blocks);
		//allocate memory for pixels
		structencode->blocks = MALLOC(sizeof(unsigned short) * structencode->total_blocks);

		if (structencode->blocks == NULL) {
			return C_ERR_ALLOC_MEMORY;
		}
		//allocate memory for positions index
		structencode->position = MALLOC(sizeof(block_position) * structencode->total_blocks);
		if (structencode->position == NULL) {
			return C_ERR_ALLOC_MEMORY;
		}
		//set index position to 0
		structencode->index = 0;
	} else {
		return COD_ERR_INVALID_FILE;
	}
	return 0;
}

/**
 * @brief free dic and pgm structure
 *
 * @param encode structure_encode_t
 */
void encode_free(struct_encode_t *encode) {
	pgm_free(&(encode->pgm));
	dic_free(&(encode->dic));
}

/**
 * @brief linear encode pgm
 *
 * @param structencode struct_encode_t
 * @param pathDic path for dic
 * @param pathPgm path for pgm
 * @return status
 */
int encode_single(struct_encode_t *structencode, char *pathDic, char *pathPgm) {
	int i = 0;
	//call encode init
	int status = encode_init(structencode, pathDic, pathPgm);
	if (status != 0)
		return status;

	//get block pixel position
	getBlock_Pixel(structencode);

	//get best pixel and write in blocks array
	for (i = 0; i < structencode->total_blocks; i++) {
		structencode->blocks[i] = best_index(&(structencode->pgm), &(structencode->dic), structencode->position[i].i, structencode->position[i].j);
		if (structencode->max_value < structencode->blocks[i]) {
			structencode->max_value = structencode->blocks[i];
		}
	}
	//write to file cod
	writeFile(pathPgm, pathDic, structencode);
	//free arrays
	FREE(structencode->blocks);
	FREE(structencode->position);
	return 0;
}

/**
 * @brief parallel encode for pgm files
 *
 * @param structencode struct_encode_t
 * @param threads number of threads
 * @param pathDic path of Dic File
 * @param pathPgm path of Pgm File
 * @return status
 */
int parallel_encode(struct_encode_t *structencode, int threads, char *pathDic, char *pathPgm) {
	pthread_t tids[threads];
	int i = 0;
	//encode init
	int status = encode_init(structencode, pathDic, pathPgm);
	if (status != 0)
		return status;

	//get blocks to position array in structure
	getBlock_Pixel(structencode);

	//init mutex
	if ((errno = pthread_mutex_init(&structencode->mutex, NULL)) != 0)
		ERROR(C_ERRO_MUTEX_INIT, "pthread_mutex_init() failed!");

	//create threads
	for (i = 0; i < threads; i++) {
		if ((errno = pthread_create(&tids[i], NULL, calculate, structencode)) != 0)
			ERROR(C_ERRO_PTHREAD_CREATE, "pthread_create() failed!");
	}
	//join threads
	for (i = 0; i < threads; i++) {
		if ((errno = pthread_join(tids[i], NULL)) != 0)
			ERROR(C_ERRO_PTHREAD_JOIN, "pthread_join() failed!");
	}

	//destroy mutex
	if ((errno = pthread_mutex_destroy(&structencode->mutex)) != 0)
		ERROR(C_ERRO_MUTEX_DESTROY, "pthread_mutex_destroy() failed!");
	//create a cod file
	writeFile(pathPgm, pathDic, structencode);

	//free arrays
	FREE(structencode->blocks);
	FREE(structencode->position);
	return 0;
}

/**
 * @brief calculate best index, used in parallel encode
 *
 * @param arg struct_encode_t
 * @return NULL
 */
void *calculate(void *arg) {
	struct_encode_t *param = (struct_encode_t *)arg;

	while (1 && running) {
		int index = 0;
		pthread_mutex_lock(&(param->mutex));
		index = param->index;
		param->index++;

		pthread_mutex_unlock(&(param->mutex));

		if (index >= param->total_blocks) {
			break;
		}

		param->blocks[index] = best_index(&(param->pgm), &(param->dic), param->position[index].i, param->position[index].j);
		//check max_value
		if (param->max_value < param->blocks[index]) {
			param->max_value = param->blocks[index];
		}
	}
	return NULL;
}

/**
* @brief check if blocks is corrects between pgm and dict file
*
* @param width	 width of image
* @param height  height of image
* @param blockRows width block dictionary
* @param blockColumns height block dictionary
* @return 0=succefull and !=0 is error
*/
int check_blocks(int width, int height, int blockRows, int blockColumns) {
	if (width % blockRows != 0 || height % blockColumns != 0)
		return COD_ERR_INVALID_BLOCK_MISMATCH;
	if ((width * height) % (blockRows * blockColumns))
		return COD_ERR_INVALID_BLOCKS_NUMBER;
	return 0;
}

/**
 * @brief get block of pixel
 *
 * @param structencode struct struct_encode_t
 * @return void
 */
void getBlock_Pixel(struct_encode_t *structencode) {
	unsigned int i, j, index = 0;
	for (i = 0; i < (structencode->pgm.header.height - structencode->dic.block_height + 1); i += structencode->dic.block_height) {
		for (j = 0; j < (structencode->pgm.header.width - structencode->dic.block_width + 1); j += structencode->dic.block_width) {
			structencode->position[index].i = i;
			structencode->position[index].j = j;
			++index;
		}
	}
}

/**
 * @brief get best pixel of line, and return line of dictionary
 * @description squared error caulculation
 *
 * @param pgm struct of pgm
 * @param dic struct of dictionary
 * @param i x coord more left block
 * @param j y coord more left block
 * @return line of dictionary(best pixel)
 */
unsigned short best_index(pgm_t * pgm, dic_t * dic, int i, int j) { //x,y cordenadas do canto superior esquerdo
	unsigned long long best = ULLONG_MAX, best_index = 0;
	unsigned int a, b, line;
	for (line = 0; line < dic->size; ++line) {
		unsigned long long sum = 0;
		for (a = 0; a < dic->block_height; ++a) {
			for (b = 0; b < dic->block_width; ++b) {
				sum += pow(pgm->pixels[a + i][b + j] - dic_get_pixel(dic, line, b, a), 2);
				if (!running)return 0;
			}
		}
		if (best > sum) {
			best = sum;
			best_index = line;
		}
	}
	return best_index;
}

/**
 * @brief write content to cod file
 *
 * @param fileNamePgm  pgm filename
 * @param fileNameDic dictionary filename
 * @param structencode struct encode
 * @return status
 */
int writeFile(char *fileNamePgm, char *fileNameDic, struct_encode_t *structencode) {
	FILE *stream;
	int i = 0;

	char name[MAX_FILENAME + 1];
	sprintf(name, "%s.cod", fileNamePgm);
	stream = fopen(name, "w");

	//write version
	fprintf(stream, "Z%d\n", structencode->pgm.header.format);
	//write more info
	fprintf(stream, "# cod created with pacodec by Emanuel Lopes\n");
	//write comment about coding
	fprintf(stream, "# coded file for '%s' (dict:'%s')\n", fileNamePgm, fileNameDic);
	//write original dimensions of file
	fprintf(stream, "%u %u\n", structencode->pgm.header.width, structencode->pgm.header.height);
	//write max temp block
	fprintf(stream, "%u\n", structencode->max_value);

	fprintf(stream, "# width x height of individual block of dict\n");
	//write dimensions of dictionary
	fprintf(stream, "%u %u\n", structencode->dic.block_width, structencode->dic.block_height);
	//write blocks
	//check if file is a P2 or P5
	int lines = structencode->pgm.header.width / structencode->dic.block_width;
	//write Z2 file
	if (structencode->pgm.header.format == P2) {
		DEBUG("p2");
		int line = 0;
		for (line = 0, i = 0; i < structencode->total_blocks; i++, line++) {
			//printf("i=%d cod=%u\n",i, structencode->blocks[i]);
			fprintf(stream, "%u ", structencode->blocks[i]);
			if (line >= lines) {
				line = 0;
				fprintf(stream, "\n");
			}
		}
		//write Z5 file
	} else if (structencode->pgm.header.format == P5) {
		DEBUG("p5");
		unsigned int data_size = (structencode->max_value <= 255) ? 1 : 2;
		for (i = 0; i < structencode->total_blocks; i++) {
			unsigned short value = structencode->blocks[i];
			fwrite(&value, data_size, 1, stream);
		}
	}
	fclose(stream);
	return 0;
}