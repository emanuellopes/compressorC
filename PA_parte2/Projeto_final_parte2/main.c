/**
* @file encode.c
* @brief Main
* @date 2015-12-15
* @author 2140825@ipleiria.pt
*/

#include <stdio.h>
#include <time.h>
#include <unistd.h>


#include "debug.h"
#include "memory.h"
#include "options.h"
#include "errors.h"
#include "signal_handler.h"
#include "encode.h"


#define AUTHOR_NAME "Emanuel João Conceição Lopes"
#define AUTHOR_STUDENT_NUMBER "2140825"
#define BIG_NUMBER 1000000000.0
#define UNDEFINED 100

int main(int argc, char *argv[]) {
    int status = UNDEFINED;

    /**
     * get processor cores
     */
    unsigned int threadsMax = sysconf(_SC_NPROCESSORS_ONLN);

    //Struct to exectute time
    struct timespec timeStart, timeEnd;

    //initialize timer
    if (clock_gettime(CLOCK_REALTIME, &timeStart) != 0) {
        ERROR(ERR_SYS_TIME, "clock_gettime () failed\n");
    }

    //args structure
    struct gengetopt_args_info args_info;

    if (argc <= 1) {
        ERROR(ERR_SYS_NO_ARGS, "Specify at least one operation\n");
    }


    if (cmdline_parser (argc, argv, &args_info) != 0) {
        cmdline_parser_print_help();
        exit(1);
    }

    //install signal ctrl+c
    install_signal_handler();

    if (args_info.about_given) {
        printf("\n");
        cmdline_parser_print_version();
        printf("\nAuthor\t\t\t\tStudent Nr.\n\n"
               AUTHOR_NAME"\t"AUTHOR_STUDENT_NUMBER"\n\n");
    }
    /**
     * parallel encode
     */
    if (args_info.parallel_encode_given) {
        if (args_info.dict_given) {
            if (args_info.threads_given) {
                //check if threads options is given
                threadsMax = args_info.threads_arg;
                //DEBUG("threads %d", threadsMax);
            }
            //DEBUG("parallel_encode");
            struct_encode_t structencode;
            //call function for parallel encode
            status = parallel_encode(&structencode, threadsMax, args_info.dict_arg, args_info.parallel_encode_arg);
            //free parallel encode
            encode_free(&structencode);
        }
    }

    /**
     * encode single
     */
    if (args_info.encode_given) {
        if (args_info.dict_given) {
            //DEBUG("encode");
            struct_encode_t structencode;
            //call encode linear
            status = encode_single(&structencode, args_info.dict_arg, args_info.encode_arg);
            encode_free(&structencode);
        }
    }
    if (running) {
        //error messages
        if (clock_gettime(CLOCK_REALTIME, &timeEnd) != 0) {
            ERROR(ERR_SYS_TIME, "clock_gettime () failed\n");
        }
        switch (status) {
        case COD_ERR_INVALID_FILE:
        case COD_ERR_INVALID_BLOCKS_NUMBER:
        case COD_ERR_INVALID_BLOCK_MISMATCH:
            fprintf(stderr, "operation:FAILURE:%s ENCODE %s\n", (args_info.encode_given ? args_info.encode_arg : args_info.parallel_encode_given ? args_info.parallel_encode_arg : ""), args_info.dict_arg);
            fprintf(stderr, "execution time: %.2lf s\n", timeEnd.tv_sec - timeStart.tv_sec + (timeEnd.tv_nsec - timeStart.tv_nsec) / BIG_NUMBER);
            fprintf(stderr, "FAILURE:incompatible dimensions of file '%s' with dict '%s'\n", (args_info.encode_given ? args_info.encode_arg : args_info.parallel_encode_given ? args_info.parallel_encode_arg : ""), args_info.dict_arg);
            break;
        case 0:

            fprintf(stderr, "operation:OK:%s ENCODE %s\n", (args_info.encode_given ? args_info.encode_arg : args_info.parallel_encode_given ? args_info.parallel_encode_arg : ""), args_info.dict_arg);
            fprintf(stderr, "execution time: %.2lf s\n", timeEnd.tv_sec - timeStart.tv_sec + (timeEnd.tv_nsec - timeStart.tv_nsec) / BIG_NUMBER);
            break;
        default:
            break;
        }
    }

//sigint message
    if (!running) {
        getDate();
    }
    cmdline_parser_free(&args_info);
    return 0;
}
