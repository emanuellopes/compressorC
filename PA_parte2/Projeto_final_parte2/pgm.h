/**
* @file pgm.h
* @brief Function to handle PGM files
* @date 2015-11-25
* @author vitor.carreira@ipleiria.pt
*/
#ifndef _PGM_H
#define _PGM_H

/**
 * Type of PGM file.
 */
typedef enum {P2 = 2, P5 = 5} pgm_format_t;

/**
 * Structure that represents a PGM header.
 */
typedef struct {
    /**
     * PGM format (P2 or P5).
     */
    pgm_format_t format;
    /**
     * Image height.
     */
    unsigned int height;
    /**
     * Image width.
     */
    unsigned int width;
    /**
     * Max intensity.
     */
    unsigned short max_value;
} pgm_header_t;

typedef struct {
    /**
     * PGM header.
     */
    pgm_header_t header;
    /**
     * PGM pixel array.
     */
    unsigned short **pixels;
} pgm_t;


/**
 * Initializes a PGM structure
 * @return a PGM structure initialized
 */
pgm_t pgm_init(void);

/**
 * Initializes a PGM structure with the given size
 * @param format the PGM file format
 * @param width width of the image
 * @param height height of the image
 * @return a PGM structure initialized with the specified size
 */
pgm_t pgm_create(pgm_format_t format, unsigned int width, unsigned int height);

/**
 * Parses a pgm file.
 * @param  filename filename's path to be parsed.
 * @param  pgm      pointer to the structure where the content of the file is stored
 * @return          0 on success; a positive error code otherwise
 */
int pgm_parse_file(const char *filename, pgm_t *pgm);

/**
 * Writes a pgm file.
 * @param  filename destination path.
 * @param  pgm      pointer to the structure containing the contents to be written
 * @return          0 on success; a positive error code otherwise
 */
int pgm_write_file(const char *filename, pgm_t *pgm);

/**
 * Release the memory held by the provided pgm structure
 * @param pgm a pointer to the structure to release
 */
void pgm_free(pgm_t *pgm);

#endif
