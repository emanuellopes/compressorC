/**
* @file dic.h
* @brief Function to parse a DIC file
* @date 2015-11-25
* @author vitor.carreira@ipleiria.pt
*/
#ifndef _DIC_H
#define _DIC_H

/**
 * Structure that represents a dictionary.
 */
typedef struct {
    /**
     * Number of blocks.
     */
    unsigned short size;
    /**
     * Individual block width.
     */
    unsigned int block_width;
    /**
     * Individual block height.
     */
    unsigned int block_height;

    /**
     * All pixels. Each row represents the flat matrix containing
     * the block pixels.
     */
    unsigned short **pixels;
} dic_t;

/**
 * Initializes a DIC structure
 * @return a DIC structure initialized
 */
dic_t dic_init(void);

/**
 * Parses a dic file.
 * @param  filename filename's path to be parsed.
 * @param  dic      pointer to the structure where the content of the file is stored
 * @return          0 on success; a positive error code otherwise
 */
int dic_parse_file(const char *filename, dic_t *dic);

/**
 * Release the memory held by the provided dic structure
 * @param dic a pointer to the structure to release
 */
void dic_free(dic_t *dic);

/**
 * Returns the value of the pixel (x,y) within the block with the given index.
 * @param  dic   the dictionary structure
 * @param  index the block index
 * @param  x     the x position of the pixel (column)
 * @param  y     the y position of the pixel (row)
 * @return       the value of the pixel (x,y) within the block with the given index.
 */
unsigned short dic_get_pixel(dic_t *dic, unsigned short index, unsigned int x, unsigned int y);

#endif
