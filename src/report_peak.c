#include <stdlib.h>
#include "../include/processing_lib.h"

// Arg processing is done per app because each app may need different arguments
int NUM_ARGS = 5;
enum ARGS {PROGNAME, ARG_INFILE, ARG_OUTFILE, ARG_NUM_CHANNELS, ARG_SAMPLING_RATE};

typedef struct _CustomArgs {
    int numChannels;
    double samplingRate;
} CustomArgs;

int reportPeak(int wrHandle, void* customArgs) {
    CustomArgs* args = (CustomArgs*)customArgs;

    printf("Allocating peaks struct\n");
    PSF_CHPEAK* peaks = (PSF_CHPEAK*)malloc(args->numChannels * sizeof(PSF_CHPEAK));
    if (checkAlloc((void**)&peaks, "Allocation of PSF_CHPEAK failed") == ERROR) {
        return ERROR;
    }

    if (psf_sndReadPeaks(wrHandle, peaks, NULL) > 0) {
        long i;
        double peaktime;
        printf("\n*****************\nPEAK information:\n");
        for(i = 0; i < args->numChannels; i++) {
            peaktime = (double) peaks[i].pos / args->samplingRate;
            printf("CH %ld:\t%.4f at %.4f secs\n",
                   i + 1, peaks[i].val, peaktime);
        }
        printf("*****************\n\n");
    }
    
    return SUCCESS;
}

int parseArgs(int argc, char* argv[], char** infile, char** outfile,
        int* numChannels, int* samplingRate) {
    if (argc != NUM_ARGS) {
        printf("Incorrect number of args %d. %d required\n", argc, NUM_ARGS);
        printf("usage: `process_audio infile outfile numChannels samplingRate`\n");
        return ERROR;
    }
    *infile = argv[ARG_INFILE];
    *outfile = argv[ARG_OUTFILE];
    *numChannels = atoi(argv[ARG_NUM_CHANNELS]);
    *samplingRate = atoi(argv[ARG_SAMPLING_RATE]);
    return SUCCESS;
}

int main(int argc, char* argv[]) {
    char* inFile;
    char* outFile;
    int numChannels;
    int samplingRate;
    int ret = parseArgs(argc, argv, &inFile, &outFile, &numChannels, &samplingRate); 
    if (ret == ERROR) {
        exit(ERROR);
    }

    CustomArgs* customArgs = malloc(sizeof(CustomArgs));
    if (checkAlloc((void**)&customArgs, "Allocation of customArgs failed") == ERROR) {
        return ERROR;
    }
    customArgs->numChannels = numChannels;
    customArgs->samplingRate = samplingRate;

    postProcessOutputFile(inFile, outFile, reportPeak, customArgs);

    free(customArgs);
    return SUCCESS;
}
