#include <stdio.h>
#include "../include/portsf.h"

void logReturnCode(int ret) {
    if (ret == 0) {
        printf("SUCCESS return code: %d\n", ret);
    } else {
        printf("FAILURE return code: %d\n", ret);
    }
}

void init() {
    printf("Init\n");
    int ret = psf_init();
    logReturnCode(ret);
}

void finish () {
    printf("Finish\n");
    int ret = psf_finish();
    logReturnCode(ret);
}

int main(int argc, char* argv[]) {
    init();

    printf("Open sound file for reading\n");
    PSF_PROPS rdProps;
    int rescale = 0;
    int rdHandle = psf_sndOpen("SIG_126_A_Retro_Synth.wav", &rdProps, rescale);
    if (rdHandle >= 0) {
        printf("SUCCESS opened file rdHandle: %d\n", rdHandle);
        printf("sampling rate: %d number of channels: %d \n", rdProps.srate, rdProps.chans);
        // TODO switch/case to map sampletype enum to string, code on pg. 216 TAPB
        printf("sample type: %d\n", rdProps.samptype);
    } else {
        printf("FAILURE return code: %d\n", rdHandle);
    }

    printf("Close sound file for reading\n");
    int ret = psf_sndClose(rdHandle);
    logReturnCode(ret); 

    printf("Open sound file for writing\n");
    const char* writeFileName = "test_write.wav";
    int clipFloats = 1;
    // NOTE: Set to 1 for platforms/applications that can't rdHandle custom header fields
    int minHeader = 0;
    int mode = PSF_CREATE_RDWR;
    PSF_PROPS wrProps;
    wrProps.srate = 44100;
    wrProps.chans = 1;
    wrProps.samptype = PSF_SAMP_16;
    wrProps.format = psf_getFormatExt(writeFileName);
    wrProps.chformat = SPEAKER_FRONT_CENTER;
    int wrHandle = psf_sndCreate(writeFileName, &wrProps, clipFloats, minHeader, mode);
    logReturnCode(wrHandle);

    printf("Close sound file for writing\n");
    ret = psf_sndClose(wrHandle);
    logReturnCode(ret); 

    finish();
}
