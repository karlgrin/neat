#ifndef HEADER_PM
#define HEADER_PM


#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <uv.h>
#include <jansson.h>

// TODO
// should provide lookup functions
// maybe also initalization of main pib/cib objects
#include "pib.h"
#include "cib.h"
#include "pmhelper.h"


void* pm_start(void *vargp);
void pm_close(int sig);

#endif