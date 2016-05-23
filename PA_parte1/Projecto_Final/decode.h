#include "common.h"
#include "dic.h"
#include "cod.h"
#include "pgm.h"
#include "errors.h"
#include "memory.h"

int decodeFile(dic_t *dic, cod_t *cod);
int decode_dir(char *folder_name, dic_t *dicFile);