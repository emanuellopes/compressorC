/**
* @file common.c
* @brief Algumas funções mais globais
* @author Emanuel João Conceição Lopes <2140825@student.estg.ipleiria.pt>
*/

#include "common.h"
#include "debug.h"
#include "memory.h"
#include "errors.h"
#include "dic.h"


/**
* Starts variable running  = 1
*/
int running = 1;

static void handler(int);

/**
* @param signal
* @return nothing
*/
void handler(int signal) {
	(void) signal;
	running = 0;
}

/**
 * @brief signal para o ctrl+c
 */
void install_signal_handler(void) {
	struct  sigaction config;
	config.sa_handler = handler;
	sigemptyset(&config.sa_mask);
	config.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &config, NULL) != 0) {
		ERROR(ERR_SYS_CALL, "sigaction failed");
	}
}
/**
 * @brief devolve a data e a operacao interrompida
 */
void getDate(void) {
	time_t rawtime;
	struct tm *info;
	char buffer[80];
	time(&rawtime);
	info = localtime( &rawtime );
	strftime(buffer, 80, "%Y-%m-%d %Hh%M", info);
	printf("\nOperation interrupted by user %s\n", buffer);
}

/**
 * @brief tamanho do ficheiro
 * @details devolve o tamanho do ficheiro e verifica se é correcto
 *
 * @param line - linha do ficheiro
 * @param rows largura 
 * @param columns altura
 * @return 1=succefull 0=error
 */
int get_size(char *line, unsigned int *rows, unsigned int *columns) {
	char *token;
	char *endptr = NULL;
	errno = 0;
	int counter = -1;
	int size[2];
	token = strtok(line, SPACE);
	while (token != NULL) {
		counter++;
		long value = strtol(token, &endptr, 10);
		if (*endptr != '\0' && *endptr != '\n')
			return 0;
		if (errno != 0)
			return 0;
		size[counter] = (int)value;
		if (size[counter] <= 0) return 0;
		token = strtok(NULL, SPACE);
	}
	FREE(token);

	if (counter > 1) {
		return 1;
	} else {
		*rows = size[0];
		*columns = size[1];
	}
	return 1;
}


/**
 * @brief devolve o valor maximo do ficheiro
 *
 * @param line do ficheiro lido
 * @param maxValue devolve o valor maximo
 *
 * @return 1=succefull 0=error
 */
int max_value(char *line, unsigned int *maxValue) {
	char *endptr;
	errno = 0;
	long value = strtol(line, &endptr, 10);
	if (*endptr != '\0' && *endptr != '\n') {

		return 0;
	}
	if (errno != 0) {

		return 0;
	}
	*maxValue = (unsigned int)value;
	return 1;
}

/**
 * @brief remove commentarios
 * @details remove commentarios das linhas
 *
 * @param line
 * @param size
 *
 * @return 0 se a linha já não tem lixo e tem conteúdo
 * @return 1 se a linha só tem lixo ou está vazia
 */
int remove_comments(char *line, ssize_t size) {
	int i = 0;
	if (size <= 0)return 0;
	for (i = 0; i < size; i++) {
		if (isblank(line[0]))return 1;
		if (line[0] == '\n') return 1;
		if (line[i] == '#') {
			line[i] = '\0';
			if (i >= 1) {
				if (line[0] == ' ')return 1;
				if (isdigit(line[i - 1])) return 0;
				if (line[i - 1] == ' ') {
					line[i - 1] = '\n';
					return 0;
				}
			}
			return 1;
		}
	}
	return 0;
}

/**
 * @brief lê a linha do ficheiro
 * @details lê a linha do ficheiro e remove o lixo
 *
 * @param stream
 * @return line without trash
 */
char  *read_line(FILE *stream) {
	size_t size = 0;
	ssize_t read;
	char *line = NULL;
	read = getline(&line, &size, stream);
	if (read == -1) {
		FREE(line);
		return "EOF";
	}
	while (remove_comments(line, read) == 1) {
		read = getline(&line, &size, stream);
	}
	return trimwhitespace(line);
}
/**
 * @brief faz o trim a string
 * 
 * @param str string
 * @return a string sem espaços no principio e no fim
 */
char *trimwhitespace(char *str) {
	char *end;
	// Trim leading space
	while (isspace(*str)) str++;
	if (*str == 0) // All spaces?
		return str;
	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;
	// Write new null terminator
	*(end + 1) = 0;
	return str;
}
/**
 * @brief verifica se a extensão do ficheiro
 * @details verifica se a extensão do ficheiro é a correcta
 * 
 * @param filename nome do ficheiro
 * @param extension extensão do ficheiro a confirmar
 * 
 * @return 0 se a extensão não é a mesma ou 1 se for igual
 */
int getExtension(char *filename, char *extension) {
	char ext[4] = "";
	unsigned int i = strlen(filename) - 3;
	int pos = 0;

	//retirar a extenção dos ficheiros, ultimos 3 caracteres
	while (i < strlen(filename)) {
		ext[pos] = tolower(filename[i]);
		pos++;
		i++;
	}
	if (strcmp(ext, extension) == 0) {
		return 1;
	}
	return 0;
}

/**
 * @brief remover extensao 
 * @details remove a extensão do nome e devolve
 * 
 * @param filename nome do ficheiro
 */
void removeExtension(char *filename) {
	filename = strrchr (filename, '.');
	if (filename != NULL)
		*filename = '\0';
}