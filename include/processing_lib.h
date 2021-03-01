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

extern const int FREE_STACK_CAPACITY;
extern int g_freeStackLength;
// NOTE: Have to keep in sync with process_lib.c because FREE_STACK_CAPACITY not declared here
extern void* g_freeStack[20];

extern psf_format FORMAT;
extern int READ_SOUNDFILE_RESCALE;
extern int NUM_CHANS;
extern int SAMPLING_RATE;
extern psf_stype SAMPLE_TYPE;
extern psf_channelformat CHANNEL_FORMAT;

extern int CLIP_FLOATS;
// NOTE: Set to 1 for platforms/applications that can't handle custom header fields, which are
// placed in the 0th header
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
int allocFrameBuf(int numChans, float** frameBuf, int numFrames);


// Main Processing Loop
// Each app uses this library and defines a processFrameBuf callback for procssing an input file
//  of audio into an output file of audio.
// processFrameBuf contract: takes a frame buffer by reference and modifies the frame buffer,
//  returns 0 if SUCESS or some other error code if ERROR
int processFrames(const char* inFile, const char* outFile, int (*processFrameBuf)(float**));

#ifdef __cplusplus
}
#endif

#endif
