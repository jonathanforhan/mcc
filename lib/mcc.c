#include "mcc.h"

#include <locale.h>

void MCC_Initialize(void) {
    setlocale(LC_ALL | ~LC_NUMERIC, "");
}
