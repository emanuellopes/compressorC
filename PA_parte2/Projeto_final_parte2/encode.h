/**
* @file encode.h
* @brief Function to handle COD files
* @date 2015-12-15
* @author 2140825@ipleiria.pt
*/
#ifndef __ENCODE_H__
#define __ENCODE_H__

#include "pgm.h"
#include "dic.h"


/**
 * struct for save i,j position per index
 */
typedef struct{
	/**
	 * i position
	 */
	unsigned int i;
	/**
	 * j position
	 */
	unsigned int j;
} block_position;

/**
 * cod Structure
 */
typedef struct {
	/**
	 * pgm structure
	 */
	pgm_t pgm;
	/**
	 * dictionary structure
	 */
	dic_t dic;
	/**
	 * number of block
	 */
	int total_blocks;
	/**
	 * array of blocks
	 */
	unsigned short *blocks;
	/**
	 * max value of encoded file
	 */
	unsigned short max_value;
	/*
	 * block position
	 */
	 block_position *position;
	 /**
	  * mutex pthread
	  */
	 pthread_mutex_t mutex;
	 /**
	  * index position
	  */
	 int index;
} struct_encode_t;


/**
 * prototypes
 */
int encode_init(struct_encode_t *structencode, char *pathDic, char *pathPgm);
int check_blocks(int width, int height, int blockRows, int blockColumns);
void getBlock_Pixel(struct_encode_t *structencode);
unsigned short best_index(pgm_t * pgm, dic_t * dic, int i, int j);
int encode_single(struct_encode_t *structencode, char *pathDic, char *pathPgm);
int parallel_encode(struct_encode_t *structencode, int threads, char *pathDic, char *pathPgm);
int writeFile(char *fileNamePgm, char *fileNameDic, struct_encode_t *structencode);
void *calculate(void *arg);
void encode_free(struct_encode_t *encode);

#endif
