/**
* @file main.c
* @brief Corpo principal do programa
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/

#include <stdio.h>
#include <string.h>


#include "debug.h"
#include "memory.h"
#include "errors.h"
#include "options.h"
#include "cod.h"
#include "pgm.h"
#include "psnr.h"
#include "dic.h"
#include "decode.h"
#include "common.h"

#define AUTHOR_NAME "Emanuel João Conceição Lopes"
#define AUTHOR_STUDENT_NUMBER "2140825"

/**
 * @brief Main function
 *
 * @param argc
 * @param argv
 *
 * @return [description]
 */
int main(int argc, char *argv[]) {
	clock_t begin, end;
	double time_spent;
	begin = clock();

	install_signal_handler();
	// Commandline arguments (gengetopt)
	struct gengetopt_args_info args_info;

	//
	if (cmdline_parser (argc, argv, &args_info) != 0) {
		cmdline_parser_print_help();
		exit(1);
	}
	// Arguments

	//about
	if (args_info.about_given) {
		printf("\n");
		cmdline_parser_print_version();
		printf("\nAuthor\t\t\t\tStudent Nr.\n\n"
		       AUTHOR_NAME"\t"AUTHOR_STUDENT_NUMBER"\n\n");
	}

	if (args_info.dict_given) {
		dic_t dicFile;
		int infoDic = open_file_dic(args_info.dict_arg, &dicFile);
		if (!args_info.decode_given) {
			switch (infoDic) {
			case ERR_INVALID_DIC:
				fprintf(stderr, "FAILURE:file '%s' is not a valid DIC file\n", args_info.dict_arg);
				break;
			case ERR_MISSING_BLOCK_DIC:
				fprintf(stderr, "FAILURE:missing blocks in DIC file '%s'\n", args_info.dict_arg);
				break;
			case ERR_MISSING_INDEX_DIC:
				fprintf(stderr, "FAILURE:missing pixels in DIC file '%s'\n", args_info.dict_arg);
				break;
			case 0:
				end = clock();
				time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				fprintf(stderr, "operation:OK:%s\n", args_info.dict_arg);
				fprintf(stderr, "execution time: %.3fs\n", time_spent);
				break;
			}
		}

		//decode
		if (args_info.decode_given) {
			cod_t codFile;
			int infoCod = open_file_cod(args_info.decode_arg, &codFile);
			switch (infoCod) {
			case ERR_INVALID_FORMAT:
				fprintf(stderr, "FAILURE:file '%s' contains an invalid COD format\n", args_info.decode_arg);
				break;
			case ERR_INVALID_BLOCK_MISMATCH:
				fprintf(stderr, "FAILURE:file '%s' contains an invalid block size\n", args_info.decode_arg);
				break;
			case ERR_INVALID_SIZE:
			case ERR_MISSING_HEADER:
			case ERR_OPEN_FAILED:
			case ERR_INVALID_MAX_VALUE:
			case ERR_INVALID_BLOCKS_NUMBER:
			case ERR_INVALID_BLOCK_W_H:
				fprintf(stderr, "FAILURE:file '%s' is not a valid COD file\n", args_info.decode_arg);
				break;
			}
			if (infoCod == 0 && infoDic == 0) {
				//o ficheiro pode ser descodificado
				int status = decodeFile(&dicFile, &codFile);
				switch (status) {
				case ERR_DECODE_FILE:
					fprintf(stderr, "operation:FAILURE:%s ENCODE %s\n",
					        args_info.decode_arg, args_info.dict_arg);
					fprintf(stderr, "execution time: %.3fs\n", time_spent);
					fprintf(stderr, "FAILURE:incompatible dimensions of file '%s' with dict '%s'",
					        args_info.decode_arg, args_info.dict_arg);
					break;
				case ERR_INCOMPATIBLE_DIMENSION:
					fprintf(stderr, "FAILURE:index out of range of file '%s' with dict '%s'", args_info.decode_arg, args_info.dict_arg);
					break;
				case  0:
					end = clock();
					time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
					fprintf(stderr, "operation:OK:%s DECODE %s\n", args_info.decode_arg, args_info.dict_arg);
					fprintf(stderr, "execution time: %.3fs\n", time_spent);
					break;
				}
			}
			if (infoCod == 0) {
				FREE(codFile.indexes[0]);
				FREE(codFile.indexes);
			}
		}
		if (infoDic == 0) {
			FREE(dicFile.blocks[0]);
			FREE(dicFile.blocks);
		}
		if (args_info.decode_dir_given && args_info.dict_given) {
			if(infoDic==0){
				//o dicionario foi bem descodificado
				decode_dir(args_info.decode_dir_arg, &dicFile);
			}else{
				WARNING("problema ao ler o dicionario");
			}
		}
	}

	if (args_info.encode_given) {
		//printf("%s\n", args_info.encode_arg);
		printf("[TODO] option not implemented yet\n");
	}

	if (args_info.parallel_encode_given) {
		//printf("%s\n", args_info.parallel_encode_arg);
		printf("[TODO] option not implemented yet\n");
	}

	if (args_info.threads_given) {
		//printf("%d\n", args_info.threads_arg);
		printf("[TODO] option not implemented yet\n");
	}

	if (args_info.PSNR_given) {
		pgm_t pgm_file1;
		pgm_t pgm_file2;
		char *token;
		int argsPSNR = -1;
		char *PSNRFILE[2];
		token = strtok(args_info.PSNR_arg, COMMA);
		while (token != NULL) {
			argsPSNR++;
			PSNRFILE[argsPSNR] = token;
			token = strtok(NULL, COMMA);
		}
		FREE(token);
		if (argsPSNR < 1) {
			fprintf(stderr, "PSNR:OK:falha\n");
			exit(1);
		}
		int infoRead = 0;
		int infoimage1 = open_file_pgm(PSNRFILE[0], &pgm_file1);
		int infoimage2 = open_file_pgm(PSNRFILE[1], &pgm_file2);
		double psnr;
		if (infoimage1 == 0 && infoimage2 == 0) {
			psnr = psnr_calc(pgm_file1, pgm_file2);
			if (psnr == ERR_PGM_INCOMPATIBLE_FILES) {
				fprintf(stderr, "FAILURE:incompatible dimensions for PSNR calculation between files '%s' and '%s'\n", PSNRFILE[0], PSNRFILE[1]);
				exit(psnr);
			}
		}
		if (infoimage1 != 0)infoRead = infoimage1;
		if (infoimage2 != 0)infoRead = infoimage2;
		switch (infoRead) {
		case 0:
			end = clock();
			time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
			fprintf(stderr, "PSNR:OK:%s:%s:%.5f\n", PSNRFILE[0], PSNRFILE[1], psnr);
			fprintf(stderr, "execution time: %.3fs\n", time_spent);
			break;
		case ERR_INVALID_PGM_FILE:
		case ERR_INVALID_SIZE:
		case ERR_INVALID_MAX_VALUE:
		case ERR_MISSING_HEADER:
			fprintf(stderr, "FAILURE:file '%s' is not a valid PGM file\n", PSNRFILE[0]);
			break;
		case ERR_INVALID_FORMAT:
			fprintf(stderr, "FAILURE:file '%s' contains an invalid PGM format\n", PSNRFILE[0]);
			break;
		}
		if (infoimage1 == 0) {
			FREE(pgm_file1.pixels[0]);
			FREE(pgm_file1.pixels);
		}
		if (infoimage2 == 0) {
			FREE(pgm_file2.pixels[0]);
			FREE(pgm_file2.pixels);
		}
	}
	cmdline_parser_free(&args_info);
	if (!running) {
		getDate();
	}
	return 0;
}

