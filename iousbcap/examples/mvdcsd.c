/*
	Simple library using IOUSBDeviceFamily and IOKit to talk to USB devices.
	Written by Sem Voigtländer on November 13, 2019
	Licensed under the MIT License
 */
#include <stdio.h>
#include <stdlib.h>
#include <CoreFoundation/CoreFoundation.h>
#include "../mvusblib.h"
#include "../mvftdi.h"

mvftdi_ctx_t ftdi = NULL;

int blinkenlights() {
	kern_return_t result = KERN_SUCCESS;
	long int tab[8] = {0xF2, 0xFA, 0xFB, 0xF9, 0xF1, 0xF3, 0xFB, 0xF9};
    int i = 0;
    while (i<=7) {
        result = mvftdi_set_bitmode(ftdi, tab[i], BITMODE_CBUS); // 0x20
        if (i == 7) i = 0;
        i++;
    }
	return KERN_SUCCESS;
}

int callbackDCSDDevice(IOUSBDeviceInterface** iface) {
    kern_return_t result =  mvusblib_opendevice(iface);

    if( result != KERN_SUCCESS) {
        fprintf(stderr, "%s: Failed to open USB device: %s.\n", __func__, mach_error_string(result));
        return result;
    }

    fprintf(stderr, "%s: Got a connection to the device.\n", __func__);

    ftdi = mvftdi_open(iface);
    return result;
}



int main(int argc, char *argv[]) {
    
    uint32_t vid = 0x0403;
    uint32_t pid = 0x8a88;
    uint32_t locationid = 0;

    fprintf(stderr, "%s: Starting mvdcsd device lookup for %#x:%#x @ %#x\n", __func__, vid, pid, locationid);
    kern_return_t err = KERN_FAILURE;
    
    while(KERN_SUCCESS != err){
        err = mvusblib_awaitdevice(vid, pid, locationid, (mvusblib_awaitcallback_t)callbackDCSDDevice);
        usleep(100);
    }
    
    return 0;
}
