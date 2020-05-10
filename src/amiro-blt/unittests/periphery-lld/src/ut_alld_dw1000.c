/*
AMiRo-OS is an operating system designed for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2019  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ut_alld_dw1000.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_DW1000)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
// No unit testing! -> Contains the Deca Demo Application
#include <deca_instance.h>
#include <alld_dw1000.h>
#include <alld_dw1000_regs.h>
#include <math.h>


aos_utresult_t utAlldDw1000Func(BaseSequentialStream* stream, aos_unittest_t* ut) {

    aosDbgCheck(ut->data != NULL);

    aos_utresult_t result = {0, 0};

    chprintf(stream, "init DW1000...\n");
    dwt_initialise(DWT_LOADUCODE, (DW1000Driver*) ut->data);
    chprintf(stream, "device ID should be: 0xDECA0130\nget device ID...\n");
    uint32_t actual_deviceId = dwt_readdevid();
    chprintf(stream, "actual device ID is: 0x%x\n", actual_deviceId);
    aosThdMSleep(1);

    if (actual_deviceId == DWT_DEVICE_ID){
      aosUtPassed(stream, &result);
    } else {
      aosUtFailed(stream, &result);
    }

    /*chprintf(stream, "write-read test...\n");
    uint32_t testvalue = 0x0A;
    uint16_t panid = 0x00;

    dwt_readfromdevice(PANADR_ID, PANADR_PAN_ID_OFFSET, 2, (uint8_t*) &panid);
    chprintf(stream, "value PANADR register before write: %x\n", panid);
    chprintf(stream, "write 0x%x to PANADR register...\n", testvalue);
    dwt_setpanid(testvalue);
    dwt_readfromdevice(PANADR_ID, PANADR_PAN_ID_OFFSET, 2, (uint8_t*) &panid);

    chprintf(stream, "PANADR register is now: 0x%x\n", panid);

    if (panid == testvalue){
      aosUtPassed(stream, &result);
    } else {
      aosUtFailed(stream, &result);
    }
*/

    dwt_setleds(0x03);

    // RUN DECA-DEMO

    instanceConfig_t chConfig;
    chConfig.channelNumber = 2;            // channel
    chConfig.preambleCode = 4;             // preambleCode
    chConfig.pulseRepFreq = DWT_PRF_16M;   // prf
    chConfig.dataRate = DWT_BR_6M8;        // datarate
    chConfig.preambleLen = DWT_PLEN_128;   // preambleLength
    chConfig.pacSize = DWT_PAC8;           // pacSize
    chConfig.nsSFD = 0;                    // non-standard SFD
    chConfig.sfdTO = (129 + 8 - 8);        // SFD timeout

    sfConfig_t sfConfig;
    sfConfig.slotDuration_ms = (10);        //slot duration in milliseconds (NOTE: the ranging exchange must be able to complete in this time
                                            //e.g. tag sends a poll, 4 anchors send responses and tag sends the final + processing time
    sfConfig.numSlots = (10);               //number of slots in the superframe (8 tag slots and 2 used for anchor to anchor ranging),
    sfConfig.sfPeriod_ms = (10*10);         //in ms => 100 ms frame means 10 Hz location rate
    sfConfig.tagPeriod_ms = (10*10);        //tag period in ms (sleep time + ranging time)
    sfConfig.pollTxToFinalTxDly_us = (2500); //poll to final delay in microseconds (needs to be adjusted according to lengths of ranging frames)

    //TODO Disable EXTI IRQ
    //port_DisableEXT_IRQ();

    // inittestapplication
    // dwt_softreset(); // already done in instance_init()

    // Set this instance mode (tag/anchor)
    (void) instance_init(TAG, (DW1000Driver*) ut->data);
    //int err = instance_init(TAG, (DW1000Driver*) ut->data);
    (void) instance_readdeviceid();
    //uint32_t deca_dev_id = instance_readdeviceid();

    // TAG ID 0
    instance_set_16bit_address(0);
    // simulate DECA config Mode 2 (DIP 1100000)
    instance_config(&chConfig, &sfConfig) ;

    //TODO Enable EXTI IRQ
    //port_EnableEXT_IRQ();

    // Start Ranging

    chprintf(stream, "start ranging...\n");


    while(1) {
        //int n = 0;
        instance_data_t* inst = instance_get_local_structure_ptr(0);

        int monitor_local = inst->monitor ;
        int txdiff = (chVTGetSystemTimeX() - inst->timeofTx);

        tag_run();
        //if delayed TX scheduled but did not happen after expected time then it has failed... (has to be < slot period)
        //if anchor just go into RX and wait for next message from tags/anchors
        //if tag handle as a timeout
        if( (monitor_local == 1) && ( txdiff > inst->slotDuration_ms) )  {
            inst->wait4ack = 0;
            tag_process_rx_timeout(inst);
            inst->monitor = 0;
        }
    }

    (void) instance_newrange();
    //int rx = instance_newrange();

    return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_DW1000) */
