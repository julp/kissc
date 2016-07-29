#pragma once

typedef void (*DtorFunc)(void *);
typedef void *(*DupFunc)(const void *);
typedef int (*CmpFunc)(const void *, const void *);
