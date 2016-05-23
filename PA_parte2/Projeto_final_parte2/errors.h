/**
* @file errors.h
* @brief erros do programa
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/

#ifndef _ERRORS_H
#define _ERRORS_H

#define C_ERRO_PTHREAD_CREATE 1
#define C_ERRO_PTHREAD_JOIN 2
#define C_ERRO_MUTEX_DESTROY 3
#define C_ERRO_MUTEX_INIT 4

#define C_ERR_ALLOC_MEMORY 5

#define C_ERR_SYS_CALL 6

#define COD_ERR_INVALID_FILE 7
#define COD_ERR_INVALID_BLOCKS_NUMBER 8
#define COD_ERR_INVALID_BLOCK_MISMATCH 9

/**
* Define arguments error
*/
#define ERR_ARGS 10
/**
* Define call system error
*/
#define ERR_SYS_CALL 11
#define ERR_SYS_TIME 12
#define ERR_SYS_NO_ARGS 13

#endif