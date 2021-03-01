#include <stdarg.h>
#include "../include/processing_lib.h"

// Arg processing is done per app because each app may need different arguments
int NUM_ARGS = 3;
enum ARGS {PROGNAME, INFILE, OUTFILE};

int reportPeak(int wrHandle, ...) {
    va_list varg;
    va_start(varg, wrHandle);
    int numChans = va_arg(varg, int);
    int samplingRate = va_arg(varg, int);

    printf("Allocating peaks struct");
    PSF_CHPEAK* peaks = (PSF_CHPEAK*)malloc(numChans * sizeof(PSF_CHPEAK));
    if (checkAlloc((void**)&peaks, "Allocation of PSF_CHPEAK failed") == ERROR) {
        return ERROR;
    }

    if (psf_sndReadPeaks(wrHandle, peaks, NULL) > 0) {
        long i;
        double peaktime;
        printf("PEAK information:\n");
        for(i = 0; i < numChans; i++) {
            peaktime = (double) peaks[i].pos / (double)samplingRate;
            printf("CH %ld:\t%.4f at %.4f secs\n",
                   i + 1, peaks[i].val, peaktime);
        }
    }
    
    va_end(varg);
    return SUCCESS;
}

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

int main(int argc, char* argv[]) {
    // CUSTOM ARG PARSING PER APP
    char* inFile;
    char* outFile;
    int ret = parseArgs(argc, argv, &inFile, &outFile); 
    if (ret == ERROR) {
        exit(ERROR);
    }
    
    int numVarArgs = 1;
    int numChannels = 2;
    postProcessOutputFile(inFile, outFile, reportPeak, numVarArgs, numChannels);
}
