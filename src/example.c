#include "../include/processing_lib.h"

// Arg processing is done per app because each app may need different arguments
int NUM_ARGS = 3;
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
