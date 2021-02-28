#include "../include/process_lib.h"

enum ARGS {PROGNAME, INFILE, OUTFILE};

int parseArgs(int argc, char* argv[], char** infile, char** outfile) {
    if (argc != NUM_ARGS) {
        printf("Incorrect number of args %d. %d required\n", argc, NUM_ARGS);
        printf("usage: `process_audio infile outfile`\n");
        return ERROR;
    }
    *infile = argv[INFILE];
    *outfile = argv[OUTFILE];
    return SUCCESS;
}

// EACH APP PASSES A LOCALLY DEFINED (OR FROM A LIB, CAN COMPOSE ETC.) processFrameBuf CALLBACK
// processFrameBuf contract, modifies a frame buffer, retuns 0 if success or some other error code
//  value if failure
int processFrames(const char* inFile, const char* outFile, int (*processFrameBuf)(float**)) {
    init();

    // Allocate frame buffer
    float* frameBuf;
    int ret = allocFrameBuf(NUM_CHANS, &frameBuf);
    if (ret == ERROR) {
        goto exit_error;
    }

    // Open input file
    printf("Open sound file for reading\n");
    PSF_PROPS rdProps;
    int rdHandle = psf_sndOpen(inFile, &rdProps, READ_SOUNDFILE_RESCALE);
    if (rdHandle < 0) {
        printf("FAILURE psf_sndOpen error code: %d\n", rdHandle);
        goto exit_error;
    }

    // Open output file
    printf("Open sound file for writing\n");
    PSF_PROPS wrProps;
    wrProps.srate = SAMPLING_RATE;
    wrProps.chans = NUM_CHANS;
    wrProps.format = psf_getFormatExt(outFile);
    wrProps.samptype = SAMPLE_TYPE;
    wrProps.chformat = CHANNEL_FORMAT;
    int wrHandle = psf_sndCreate(outFile, &wrProps, CLIP_FLOATS, MIN_HEADER, MODE);
    logReturnCode(wrHandle, "psf_sndCreate()");

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

int main(int argc, char* argv[]) {
    // CUSTOM ARG PARSING PER APP
    char* inFile;
    char* outFile;
    int ret = parseArgs(argc, argv, &inFile, &outFile); 
    if (ret == ERROR) {
        exit(ERROR);
    }

    processFrames(inFile, outFile, dummyProcessFrameBuf);
}
