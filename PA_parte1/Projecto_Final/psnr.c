/**
* @file psnr.c
* @brief teste PSNR
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/
#include "errors.h"
#include "pgm.h"
#include "psnr.h"
#include "debug.h"
/**
 * @brief teste PSNR
 * @details testa o PSNR de dois ficheiros PGM
 * 
 * @param image1 
 * @param image2 
 * 
 * @return valor do psnr
 */
double psnr_calc(pgm_t image1, pgm_t image2) {
	unsigned int i = 0, j = 0;
	double sdsum = 0.0; 	//sum of the differences squared

	//check if images are compatible (type, max intensity and size)

	if (image1.header.width != image2.header.width
	        || image1.header.height != image2.header.height)
		return ERR_PGM_INCOMPATIBLE_FILES;

	// mean square error calculation
	/* for loops are expensive. check run during diference squared calculation
	check if image1 and image 2 are the same if the pixels have the same value.
	If pixels are equal, sameimage++. If sameimage == nr. pixels in image, don't
	calculate MSNR (divide by 0).
	*/
	//calculate difference squared
	for (i = 0; i < image1.header.width; i++) {
		for (j = 0; j < image1.header.height; j++) {
			sdsum += pow((image1.pixels[i][j] - image2.pixels[i][j]), 2);
		}
	}
	//peak signal-to-noise ratio
	double psnr = 20 * log10(image1.header.max_value / sqrt(sdsum / (image1.header.width * image1.header.height)));
	return psnr;
}
