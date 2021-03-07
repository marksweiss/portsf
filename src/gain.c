#include <string.h>
#include "../include/processing_lib.h"

int NUM_ARGS = 5;
enum ARGS {PROGNAME, INFILE, OUTFILE, NUM_CHANNELS, GAIN_FACTOR};

typedef struct _CustomArgs {
    int numChannels;
    float gainFactor;
} CustomArgs;

int applyGain(float** frameBuf, void* customArgs) {
    CustomArgs* args = (CustomArgs*)customArgs;

    for (int i = 0; i < args->numChannels; i++) {
        *((*frameBuf) + i) *= args->gainFactor;     
    }

    return SUCCESS;
}

int parseArgs(int argc, char* argv[], char** infile, char** outfile, int* numChannels,
        float* gainFactor) {
    if (argc != NUM_ARGS) {
        printf("Incorrect number of args %d. %d required\n", argc, NUM_ARGS);
        printf("usage: `process_audio infile outfile numChannels gainFactor`\n");
        return ERROR;
    }
    *infile = argv[INFILE];
    *outfile = argv[OUTFILE];
    *numChannels = atoi(argv[NUM_CHANNELS]);
    *gainFactor = atof(argv[GAIN_FACTOR]);
    return SUCCESS;
}

int main(int argc, char* argv[]) {
    char* inFile;
    char* outFile;
    int numChannels;
    float gainFactor;
    int ret = parseArgs(argc, argv, &inFile, &outFile, &numChannels, &gainFactor); 
    if (ret == ERROR) {
        exit(ERROR);
    }

    CustomArgs* customArgs = malloc(sizeof(CustomArgs));
    if (checkAlloc((void**)&customArgs, "Allocation of customArgs failed") == ERROR) {
        return ERROR;
    }
    customArgs->numChannels = numChannels;
    customArgs->gainFactor = gainFactor;

    processFrames(inFile, outFile, applyGain, customArgs);

    free(customArgs);
    return SUCCESS;
}
