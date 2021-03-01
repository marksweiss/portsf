#include <stdarg.h>
#include <stdlib.h>
#include "../include/processing_lib.h"

// Arg processing is done per app because each app may need different arguments
int NUM_ARGS = 5;
enum ARGS {PROGNAME, ARG_INFILE, ARG_OUTFILE, ARG_NUM_CHANNELS, ARG_SAMPLING_RATE};

int NUM_VAR_ARGS = 2;
int reportPeak(int wrHandle, ...) {
    va_list varg;
    va_start(varg, wrHandle);
    int numChans = va_arg(varg, int);
    int samplingRate = va_arg(varg, int);

    printf("Allocating peaks struct\n");
    PSF_CHPEAK* peaks = (PSF_CHPEAK*)malloc(numChans * sizeof(PSF_CHPEAK));
    if (checkAlloc((void**)&peaks, "Allocation of PSF_CHPEAK failed") == ERROR) {
        return ERROR;
    }

    if (psf_sndReadPeaks(wrHandle, peaks, NULL) > 0) {
        long i;
        double peaktime;
        printf("\n*****************\nPEAK information:\n");
        for(i = 0; i < numChans; i++) {
            peaktime = (double) peaks[i].pos / (double)samplingRate;
            printf("CH %ld:\t%.4f at %.4f secs\n",
                   i + 1, peaks[i].val, peaktime);
        }
        printf("*****************\n\n");
    }
    
    va_end(varg);
    return SUCCESS;
}

int parseArgs(int argc, char* argv[], char** infile, char** outfile,
        int* numChannels, int* samplingRate) {
    if (argc != NUM_ARGS) {
        printf("Incorrect number of args %d. %d required\n", argc, NUM_ARGS);
        printf("usage: `process_audio infile outfile`\n");
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
    
    postProcessOutputFile(inFile, outFile, reportPeak, NUM_VAR_ARGS, numChannels, samplingRate);
}
