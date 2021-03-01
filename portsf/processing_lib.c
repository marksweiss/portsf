#include "../include/processing_lib.h"

int ERROR = 1;
int SUCCESS = 0;

enum ARGS {PROGNAME, INFILE, OUTFILE};

const int FREE_STACK_CAPACITY = 20;
int g_freeStackLength = 0;
void* g_freeStack[FREE_STACK_CAPACITY];

int READ_SOUNDFILE_RESCALE = 0;
psf_channelformat CHANNEL_FORMAT = SPEAKER_FRONT_CENTER;

int CLIP_FLOATS = 1;
// NOTE: Set to 1 for platforms/applications that can't rdHandle custom header fields
int MIN_HEADER = 0;
int MODE = PSF_CREATE_RDWR;

static char errMsg[128];

int dummyProcessFrameBuf(float** frameBufPtr) {
    return SUCCESS;
}

#pragma clang diagnostic ignored "-Wformat-security"
void logReturnCode(int ret, const char* msg) {
    if (msg) {
        printf("%s ", msg);
    }
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

int allocFrameBuf(int numChans, float** frameBuf, int numFrames) {
    *frameBuf = (float*) malloc(numChans * sizeof(float) * numFrames);
    if (*frameBuf == NULL) {
        printf("Frame allocation failed");
        return ERROR;
    }
    addToFreeStack((void**)&frameBuf);
    return SUCCESS;
}

int processFrames(const char* inFile, const char* outFile, int (*processFrameBuf)(float**)) {
    init();

    // Open input file
    // Set properties used for frame buffer and writing output
    printf("Open sound file for reading\n");
    PSF_PROPS rdProps;
    int rdHandle = psf_sndOpen(inFile, &rdProps, READ_SOUNDFILE_RESCALE);
    if (rdHandle < 0) {
        sprintf(errMsg, "psf_sndOpen() error code: %d\n", rdHandle);
        logReturnCode(rdHandle, errMsg);
        goto exit_error;
    }

    // Allocate frame buffer
    printf("Allocate frame buffer\n");
    float* frameBuf;
    // TODO Support frame buffers of size > 1
    int numFrames = 1;
    int ret = allocFrameBuf(rdProps.chans, &frameBuf, numFrames);
    if (ret == ERROR) {
        logReturnCode(ret, "allocFrameBuf()");
        goto exit_error;
    }

    // TODO This logic doesn't support modifying any file-leve property other than audio file format
    // Open output file
    printf("Open sound file for writing\n");
    PSF_PROPS wrProps;
    wrProps.srate = rdProps.srate;
    wrProps.chans = rdProps.chans;
    wrProps.format = psf_getFormatExt(outFile);
    wrProps.samptype = rdProps.samptype;
    wrProps.chformat = rdProps.chformat;
    int wrHandle = psf_sndCreate(outFile, &wrProps, CLIP_FLOATS, MIN_HEADER, MODE);
    if (wrHandle < 0) {
        sprintf(errMsg, "psf_sndCreate() error code: %d\n", wrHandle);
        logReturnCode(wrHandle, errMsg);
        goto exit_error;
    }

    // Process each frame in input file with callback, write to output file
    long framesRead, totalRead;
    totalRead = 0;
    framesRead = psf_sndReadFloatFrames(rdHandle, frameBuf, 1);
    while (framesRead == 1) {
        // Process the last frame read
        if ((processFrameBuf(&frameBuf)) != SUCCESS) {
            goto exit_error;
        }

        // Write the last processed frame to output
        if (psf_sndWriteFloatFrames(wrHandle, frameBuf, 1) != 1) {
            printf("Error writing to outfile\n");
            goto exit_error;
        } 

        // Read the next frame, error handled on while() check
        framesRead = psf_sndReadFloatFrames(rdHandle, frameBuf, 1);
        totalRead++;
    }
    // Check last read (that exited the processing loop) for failure
    if (framesRead < 0) {
        printf("Error reading infile. Outfile is incomplete.\n");
        goto exit_error;
    }
    printf("%ld sample frames processed to %s\n", totalRead, outFile);

    // Cleanup
    printf("Close sound file for reading\n");
    ret = psf_sndClose(rdHandle);
    logReturnCode(ret, "psf_sndClose()"); 

    printf("Close sound file for writing\n");
    ret = psf_sndClose(wrHandle);
    logReturnCode(ret, "psf_sndClose()"); 

    finish();
    goto exit_success;

    // Handle failure
    exit_error:
        for (int i = 0; i < g_freeStackLength; i++) {
            free(g_freeStack[i]);
        }
        finish();
        return ERROR; 

    exit_success:
        return SUCCESS;
}
