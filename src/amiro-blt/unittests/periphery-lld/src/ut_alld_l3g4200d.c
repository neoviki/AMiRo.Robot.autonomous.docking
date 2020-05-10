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

#include <ut_alld_l3g4200d.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_L3G4200D)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_l3g4200d.h>

/**
 * @brief   L3G4200D unit test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldL3g4200dFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_l3g4200ddata_t*)(ut->data)) != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  uint8_t data = 0;
  uint8_t write_data[5];
  uint8_t read_data[5];
  int16_t sdata[3];
  uint8_t status_reg;
  eventmask_t event_mask;
  bool success = false;
  uint8_t fifo = 0x5F;
  event_listener_t el;

  for (uint8_t dataIdx = 0; dataIdx < 4; dataIdx++) {
    write_data[dataIdx] = (dataIdx+1)*11;
  }
  write_data[4] = 0;

  chprintf(stream, "check identity...\n");
  status = l3g4200d_lld_read_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd, L3G4200D_LLD_REGISTER_WHO_AM_I, &data, 1);
  if(status == APAL_STATUS_SUCCESS && data == L3G4200D_LLD_WHO_AM_I){
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X, data: %d\n", status, data);
  }

  chprintf(stream, "write register...\n");
  status = l3g4200d_lld_write_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd, L3G4200D_LLD_REGISTER_CTRL_REG1, write_data, 1);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read register...\n");
  status = l3g4200d_lld_read_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd, L3G4200D_LLD_REGISTER_CTRL_REG1, &data, 1);
  if (status == APAL_STATUS_SUCCESS && data == write_data[0]) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X, data: %d\n", status, data);
  }

  chprintf(stream, "write multiple registers...\n");
  status = l3g4200d_lld_write_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd, L3G4200D_LLD_REGISTER_CTRL_REG1, write_data, 5);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read multiple registers...\n");
  status = l3g4200d_lld_read_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd, L3G4200D_LLD_REGISTER_CTRL_REG1, read_data, 5);
  uint8_t errors = 0;
  for (uint8_t dataIdx = 0; dataIdx < 5; dataIdx++) {
    if (read_data[dataIdx] != write_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_SUCCESS && errors == 0) {
    aosUtPassed(stream, &result);
  } else {
    for (uint8_t dataIdx = 0; dataIdx < 5; dataIdx++) {
      chprintf(stream, "\t\tStatus: %d, CTRL_REG%d: %d, write_data: %d\n", status, dataIdx+1, read_data[dataIdx], write_data[dataIdx]);
    }
    aosUtFailedMsg(stream, &result, "0x%08X, errors: %d\n", status, errors);
  }

  chprintf(stream, "read config...\n");
  l3g4200d_lld_cfg_t cfg;
  status = l3g4200d_lld_read_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &cfg);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "write config...\n");
  cfg.registers.ctrl_reg1 = L3G4200D_LLD_PD | L3G4200D_LLD_DR_100_HZ | L3G4200D_LLD_BW_12_5 | L3G4200D_LLD_ZEN | L3G4200D_LLD_YEN | L3G4200D_LLD_XEN;
  //cfg.registers.ctrl_reg1 = L3G4200D_LLD_PD | L3G4200D_LLD_DR_800_HZ | L3G4200D_LLD_BW_20 | L3G4200D_LLD_ZEN | L3G4200D_LLD_YEN | L3G4200D_LLD_XEN;
  cfg.registers.ctrl_reg3 = 0x07;
  cfg.registers.ctrl_reg5 |= L3G4200D_LLD_FIFO_EN;
  status = l3g4200d_lld_write_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, cfg);
  uint8_t reg1 = cfg.data[0];
  status |= l3g4200d_lld_read_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &cfg);
  if (status == APAL_STATUS_SUCCESS && cfg.data[0] == reg1) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read gyro data for five seconds...\n");
  status = APAL_STATUS_OK;
  for (uint8_t i = 0; i < 5; ++i) {
    status |= l3g4200d_lld_read_all_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, sdata, &cfg);
    chprintf(stream, "\t\tX = %6d\tY = %6d\tZ = %6d\n", sdata[0], sdata[1], sdata[2]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read X axis for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint32_t i = 0; i <= 5; i++) {
    status |= l3g4200d_lld_read_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &(sdata[0]), L3G4200D_LLD_X_AXIS, &cfg);
    chprintf(stream, "\t\tX = %6d\n", sdata[0]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read Y axis for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint32_t i = 0; i <= 5; i++) {
    status |= l3g4200d_lld_read_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &(sdata[0]), L3G4200D_LLD_Y_AXIS, &cfg);
    chprintf(stream, "\t\tY = %6d\n", sdata[0]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read Z axis for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint32_t i = 0; i <= 5; i++) {
    status |= l3g4200d_lld_read_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &(sdata[0]), L3G4200D_LLD_Z_AXIS, &cfg);
    chprintf(stream, "\t\tZ = %6d\n", sdata[0]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }
  aosThdMSleep(10);

  chprintf(stream, "read status register...\n");
  status = l3g4200d_lld_read_status_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &status_reg);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read interrupt config...\n");
  l3g4200d_lld_int_cfg_t int_cfg;
  status = l3g4200d_lld_read_int_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &int_cfg);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "write interrupt config...\n");
  int_cfg.registers.int1_tsh_xh = 10;
  status = l3g4200d_lld_write_int_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, int_cfg);
  l3g4200d_lld_int_cfg_t int_cfg2;
  status |= l3g4200d_lld_read_int_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, &int_cfg2);
  if (status == APAL_STATUS_SUCCESS && int_cfg.registers.int1_tsh_xh == 10) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "interrupt test: read fifo until empty...\n");
  chEvtRegister(((ut_l3g4200ddata_t*)(ut->data))->src, &el, 0);
  status = l3g4200d_lld_write_fifo_ctrl_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd,fifo);
  fifo = 0;
  status |= l3g4200d_lld_read_fifo_ctrl_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd,&fifo);
  status |= l3g4200d_lld_read_all_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, sdata, &cfg);
  chEvtGetAndClearFlags(&el);
  aosThdSSleep(1);
  chEvtGetAndClearFlags(&el);
  success = false;
  for (uint8_t i = 0; i < 200; i++) {
    status |= l3g4200d_lld_read_all_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, sdata, &cfg);
    event_mask = chEvtWaitAnyTimeout(~0, TIME_IMMEDIATE);
    status |= l3g4200d_lld_read_fifo_src_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd,&fifo);
    if (event_mask != 0 && ((fifo & L3G4200D_LLD_EMPTY) || fifo == 0)) {
      success = true;
      break;
    }
    aosThdMSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS && success) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  fifo = 0x4A;
  status |= l3g4200d_lld_write_fifo_ctrl_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd,fifo);
  cfg.registers.ctrl_reg1 = L3G4200D_LLD_PD | L3G4200D_LLD_DR_800_HZ | L3G4200D_LLD_BW_20 | L3G4200D_LLD_ZEN | L3G4200D_LLD_YEN | L3G4200D_LLD_XEN;
  cfg.registers.ctrl_reg3 = 0x04;
  status |= l3g4200d_lld_write_config(((ut_l3g4200ddata_t*)(ut->data))->l3gd, cfg);
  chprintf(stream, "interrupt test: wait until wtm reached...\n");
  for (uint8_t i = 0; i < 200; i++) {
    status |= l3g4200d_lld_read_all_data(((ut_l3g4200ddata_t*)(ut->data))->l3gd, sdata, &cfg);
    event_mask = chEvtWaitAnyTimeout(~0, TIME_IMMEDIATE);
    status |= l3g4200d_lld_read_fifo_src_register(((ut_l3g4200ddata_t*)(ut->data))->l3gd,&fifo);
    if (event_mask != 0 && (fifo & L3G4200D_LLD_WTM)) {
      success = true;
      break;
    }
    aosThdMSleep(10);
  }
  if (status == APAL_STATUS_SUCCESS && success) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chEvtUnregister(((ut_l3g4200ddata_t*)(ut->data))->src, &el);
  aosThdMSleep(10);

  aosUtInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(L3G4200DDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_L3G4200D) */


