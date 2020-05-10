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

#include <ut_alld_lis331dlh.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_LIS331DLH)) || defined(__DOXYGEN__)

#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_lis331dlh.h>

/**
 * @brief   LIS331DLH unit test function.
 *
 * @param[in] stream  Stream for input/output.
 * @param[in] ut      Unit test object.
 *
 * @return            Unit test result value.
 */
aos_utresult_t utAlldLis331dlhFunc(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_lis331dlhdata_t*)(ut->data)) != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  uint8_t data = 0;
  uint8_t write_data[5];
  uint8_t read_data[5];
  uint8_t errors = 0;
  uint8_t reg1;
  int16_t sdata[3];
  uint8_t status_reg;
  lis331dlh_lld_int_cfg_t intcfg;
  event_listener_t el;
  eventmask_t event_mask;
  uint8_t success = 0;

  for (uint8_t dataIdx = 0; dataIdx < 4; dataIdx++) {
    write_data[dataIdx] = (dataIdx+1)*11;
  }
  write_data[4] = 0;

  chprintf(stream, "check identity...\n");
  status = lis331dlh_lld_read_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, LIS331DLH_LLD_REGISTER_WHO_AM_I, &data, 1);
  if (status == APAL_STATUS_SUCCESS &&
      data == LIS331DLH_LLD_WHO_AM_I) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed\n");
    aosUtFailedMsg(stream, &result, "0x%08X, data: %d\n", status, data);
    ++result.failed;
  }

  chprintf(stream, "write register...\n");
  status = lis331dlh_lld_write_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, LIS331DLH_LLD_REGISTER_CTRL_REG1, write_data, 1);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read register...\n");
  status = lis331dlh_lld_read_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, LIS331DLH_LLD_REGISTER_CTRL_REG1, &data, 1);
  if (status == APAL_STATUS_SUCCESS && data == write_data[0]) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed\n");
    aosUtFailedMsg(stream, &result, "0x%08X, data: %d\n", status, data);
    ++result.failed;
  }

  chprintf(stream, "write multiple registers...\n");
  status = lis331dlh_lld_write_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, LIS331DLH_LLD_REGISTER_CTRL_REG1, write_data, 5);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read multiple registers...\n");
  status = lis331dlh_lld_read_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, LIS331DLH_LLD_REGISTER_CTRL_REG1, read_data, 5);
  for (uint8_t dataIdx = 0; dataIdx < 5; dataIdx++) {
    if (read_data[dataIdx] != write_data[dataIdx]) {
      ++errors;
    }
  }
  if (status == APAL_STATUS_SUCCESS &&
      errors == 0) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed\n");
    for (uint8_t dataIdx = 0; dataIdx < 5; dataIdx++) {
      chprintf(stream, "\t\tStatus: %d, CTRL_REG%d: %d, write_data: %d\n", status, dataIdx+1, read_data[dataIdx], write_data[dataIdx]);
    }
    aosUtFailedMsg(stream, &result, "0x%08X, errors: %d\n", status, errors);
  }

  chprintf(stream, "read config...\n");
  lis331dlh_lld_cfg_t cfg;
  status = lis331dlh_lld_read_config(((ut_lis331dlhdata_t*)(ut->data))->lisd, &cfg);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "write config...\n");
  cfg.registers.ctrl_reg1 = LIS331DLH_LLD_PM_ODR |
      LIS331DLH_LLD_DR_1000HZ_780LP |
      LIS331DLH_LLD_X_AXIS_ENABLE |
      LIS331DLH_LLD_Y_AXIS_ENABLE |
      LIS331DLH_LLD_Z_AXIS_ENABLE;
  cfg.registers.ctrl_reg3 = 0x00;
  status = lis331dlh_lld_write_config(((ut_lis331dlhdata_t*)(ut->data))->lisd, &cfg);
  reg1 = cfg.data[0];
  lis331dlh_lld_read_config(((ut_lis331dlhdata_t*)(ut->data))->lisd, &cfg);
  if (status == APAL_STATUS_SUCCESS && cfg.data[0] == reg1) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read acceleration for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint8_t i = 0; i < 5; ++i) {
    status |= lis331dlh_lld_read_all_data(((ut_lis331dlhdata_t*)(ut->data))->lisd, sdata, &cfg);
    chprintf(stream, "\t\tX = %6d\tY = %6d\tZ = %6d\n", sdata[0], sdata[1], sdata[2]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read X acceleration for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint8_t i = 0; i < 5; ++i) {
    status |= lis331dlh_lld_read_data(((ut_lis331dlhdata_t*)(ut->data))->lisd, &(sdata[0]), LIS331DLH_LLD_X_AXIS, &cfg);
    chprintf(stream, "\t\tX = %6d\n", sdata[0]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read Y acceleration for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint8_t i = 0; i < 5; ++i) {
    status |= lis331dlh_lld_read_data(((ut_lis331dlhdata_t*)(ut->data))->lisd, &(sdata[0]), LIS331DLH_LLD_Y_AXIS, &cfg);
    chprintf(stream, "\t\tY = %6d\n", sdata[0]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read Z acceleration for five seconds...\n");
  status = APAL_STATUS_SUCCESS;
  for (uint8_t i = 0; i < 5; ++i) {
    status |= lis331dlh_lld_read_data(((ut_lis331dlhdata_t*)(ut->data))->lisd, &(sdata[0]), LIS331DLH_LLD_Z_AXIS, &cfg);
    chprintf(stream, "\t\tZ = %6d\n", sdata[0]);
    aosThdSSleep(1);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "read status register...\n");
  status = lis331dlh_lld_read_status_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, &status_reg);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  chprintf(stream, "interrupt...\n");
  intcfg.cfg_reg = LIS331DLH_LLD_INT_CFG_X_HIGH_ENABLE | LIS331DLH_LLD_INT_CFG_Y_HIGH_ENABLE | LIS331DLH_LLD_INT_CFG_Z_HIGH_ENABLE;
  intcfg.threshold = 10;
  intcfg.duration = 1;
  lis331dlh_lld_write_int_config(((ut_lis331dlhdata_t*)(ut->data))->lisd, &intcfg, LIS331DLH_LLD_INT1);
  aosThdSSleep(1);
  chEvtRegister(((ut_lis331dlhdata_t*)(ut->data))->src, &el, 3);
  chprintf(stream, "\t\tmove the AMiRo now to generate interrupts\n");
  aosThdSSleep(1);
  for (uint8_t i = 0; i < 10; i++) {
    event_mask = chEvtWaitOneTimeout(EVENT_MASK(3), TIME_IMMEDIATE);
    status |= lis331dlh_lld_read_all_data(((ut_lis331dlhdata_t*)(ut->data))->lisd, sdata, &cfg);
    status = lis331dlh_lld_read_register(((ut_lis331dlhdata_t*)(ut->data))->lisd, LIS331DLH_LLD_REGISTER_INT1_SOURCE, &data, 1);
    eventflags_t flags = chEvtGetAndClearFlags(&el);
    if (event_mask == EVENT_MASK(3) && (flags & ((ut_lis331dlhdata_t*)(ut->data))->evtflags)) {
      ++success;
      chprintf(stream, "\t\tX = %6d INTERRUPT\n", sdata[0]);
    } else {
      chprintf(stream, "\t\tX = %6d\n", sdata[0]);
    }
    aosThdSSleep(1);
  }
  event_mask = chEvtWaitOneTimeout(EVENT_MASK(3), TIME_IMMEDIATE);
  chEvtUnregister(((ut_lis331dlhdata_t*)(ut->data))->src, &el);
  if (status == APAL_STATUS_SUCCESS && success > 0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailed(stream, &result);
  }

  aosUtInfoMsg(stream, "driver object memory footprint: %u bytes\n", sizeof(LIS331DLHDriver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_LIS331DLH) */

