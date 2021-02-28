#include "../include/process_lib.h"

int ERROR = 1;
int SUCCESS = 0;

int NUM_ARGS = 2;
enum ARGS {PROGNAME, INFILE, OUTFILE};

const int FREE_STACK_CAPACITY = 20;
int g_freeStackLength = 0;
void* g_freeStack[FREE_STACK_CAPACITY];

psf_format FORMAT = PSF_STDWAVE;
int READ_SOUNDFILE_RESCALE = 0;
int NUM_CHANS = 1;
int SAMPLING_RATE = 44100;
psf_stype SAMPLE_TYPE = PSF_SAMP_16;
psf_channelformat CHANNEL_FORMAT = SPEAKER_FRONT_CENTER;

int CLIP_FLOATS = 1;
// NOTE: Set to 1 for platforms/applications that can't rdHandle custom header fields
int MIN_HEADER = 0;
int MODE = PSF_CREATE_RDWR;

int dummyProcessFrameBuf(float** frameBufPtr);
void logReturnCode(int ret, const char* msg);
void init();
void finish();
void addToFreeStack(void** ptr);
void removeFromFreeStack(void** ptr);
int allocFrameBuf(int numChans, float** frameBuf);
int parseArgs(int argc, char* argv[], char** infile, char** outfile);

int dummyProcessFrameBuf(float** frameBufPtr) {
    return SUCCESS;
}

#pragma clang diagnostic ignored "-Wformat-security"
void logReturnCode(int ret, const char* msg) {
    printf(msg);
    if (ret == 0) {
        printf("SUCCESS return code: %d\n", ret);
    } else {
        printf("FAILURE return code: %d\n", ret);
    }
}

void init() {
    printf("Init\n");
    int ret = psf_init();
    logReturnCode(ret, "init()");
}

void finish() {
    printf("Finish\n");
    int ret = psf_finish();
    logReturnCode(ret, "finish()");
}

void addToFreeStack(void** ptr) {
    if (g_freeStackLength == FREE_STACK_CAPACITY) {
        printf("ERROR: free list full");
        exit(1);
    } else {
        g_freeStack[g_freeStackLength] = *ptr;
        g_freeStackLength++;
    }
}

void removeFromFreeStack(void** ptr) {
    if (g_freeStackLength == 0) {
        printf("ERROR: invalid free list access negative index");
        exit(1);
    } else {
        free(g_freeStack[g_freeStackLength]);    
        g_freeStackLength--;
    }
    
}

int allocFrameBuf(int numChans, float** frameBuf) {
    *frameBuf = (float*) malloc(numChans * sizeof(float));
    if (*frameBuf == NULL) {
        printf("Frame allocation failed");
        return ERROR;
    }
    addToFreeStack((void**)&frameBuf);
    return SUCCESS;
}
