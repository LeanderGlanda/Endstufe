#include "SigmaStudioOutput/systemfiles_IC_1.h"
#include "SigmaStudioOutput/systemfiles_IC_2.h"
#include "ADAU1467.h"

extern "C" void load_sigmastudio_program_adau1467() {
    default_download_IC_1();
}

extern "C" void load_sigmastudio_program_adau1962a() {
    default_download_IC_2();
}