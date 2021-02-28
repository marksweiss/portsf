#ifndef __PROCESS_LIB_H_INCLUDED
#define __PROCESS_LIB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// Make this the top-level include, pull in dependencies here
#include <stdio.h>
#include <stdlib.h>
#include "portsf.h"

// Declare consts used in this library and in process_*.c application files
extern int ERROR;
extern int SUCCESS;

extern int NUM_ARGS;

extern const int FREE_STACK_CAPACITY;
// Hack to parameterize g_freeStack. Can't use FREE_STACK_CAPACITY because not declared here.
// NOTE: Have to keep in sync with process_lib.c
extern int g_freeStackLength;
extern void* g_freeStack[20];

extern psf_format FORMAT;
extern int READ_SOUNDFILE_RESCALE;
extern int NUM_CHANS;
extern int SAMPLING_RATE;
extern psf_stype SAMPLE_TYPE;
extern psf_channelformat CHANNEL_FORMAT;

extern int CLIP_FLOATS;
// NOTE: Set to 1 for platforms/applications that can't rdHandle custom header fields
extern int MIN_HEADER;
extern int MODE;

// process_lib API declarations. Helpers and wrappers around portsf, mostly to add error handling
// Notably, the *FreeStack functions add a system for registering allocated pointers and cleaning
//  them up on failure

// Dummy no-op example callback to pass to any app processFrames() function
int dummyProcessFrameBuf(float** frameBufPtr);
// Helper that printfs() return code message and exits on failure
void logReturnCode(int ret, const char* msg);
// Wrappers around portsf init and finish calls, with error handling and print logging
void init();
void finish();
// API for registering allocated pointers so they are freed on failure
void addToFreeStack(void** ptr);
void removeFromFreeStack(void** ptr);
// Helper to allocate frame buffer
int allocFrameBuf(int numChans, float** frameBuf);

#ifdef __cplusplus
}
#endif

#endif
