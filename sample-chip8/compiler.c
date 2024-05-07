#include "compiler.h"

#include <string.h>
#include <stdlib.h>

CompilationResult compile(const char* source_file)
{
    CompilationResult cr;
    memset(&cr, 0, sizeof(cr));
    cr.success = false;

    const char* text = "There was a compilation error.";
    cr.error_info = calloc(1, strlen(text));
    strncpy(cr.error_info, text, strlen(text));

    return cr;
}