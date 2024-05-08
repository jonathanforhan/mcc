#include "mcc.h"

#include <locale.h>

void InitializeMCC(void) {
    setlocale(LC_ALL | ~LC_NUMERIC, "");
}
