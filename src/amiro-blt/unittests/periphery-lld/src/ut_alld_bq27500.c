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

#include <ut_alld_bq27500.h>

#if ((AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ27500)) || defined(__DOXYGEN__)

#include <string.h>
#include <aos_debug.h>
#include <chprintf.h>
#include <aos_thread.h>
#include <alld_bq27500.h>

// change saved unseal keys to test bruteforcing
#ifdef BQ27500_UT_TEST_BRUTEFORCE

#ifdef BQ27500_LLD_DEFAULT_UNSEAL_KEY0
#undef BQ27500_LLD_DEFAULT_UNSEAL_KEY0
#define BQ27500_LLD_DEFAULT_UNSEAL_KEY0 0x1234
#endif

#ifdef BQ27500_LLD_DEFAULT_UNSEAL_KEY1
#undef BQ27500_LLD_DEFAULT_UNSEAL_KEY1
#define BQ27500_LLD_DEFAULT_UNSEAL_KEY1 0x5678
#endif

#endif

bq27500_lld_control_status_t _try_unseal(BQ27500Driver* driver, uint16_t key0, uint16_t key1, apalTime_t timeout) {
  uint16_t dst;
  bq27500_lld_control_status_t ctrl;
  bq27500_lld_send_ctnl_data(driver, key1, timeout);
  aosThdUSleep(1);
  bq27500_lld_send_ctnl_data(driver, key0, timeout);
  aosThdUSleep(1);
  bq27500_lld_sub_command_call(driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, timeout);
  aosThdUSleep(1);
  bq27500_lld_std_command(driver, BQ27500_LLD_STD_CMD_Control, &dst, timeout);
  bq27500_lld_sub_command_read(driver, &ctrl.value, timeout);
  return ctrl;
}

uint8_t _bruteforce_sealed_key_bitflips(BaseSequentialStream* stream, BQ27500Driver* driver, uint16_t key0, uint16_t key1, apalTime_t timeout) {
  bq27500_lld_control_status_t ctrl;
  uint16_t k0;
  uint16_t k1 = key1;
  for (uint8_t i = 0; i < 16; i++) {
    k0 = key0 ^ (1 << i); // flip bit i
    ctrl = _try_unseal(driver, k0, k1, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
      return 1;
    }
  }
  k0 = key0;
  for (uint8_t i = 0; i < 16; i++) {
    k1 = key1 ^ (1 << i); // flip bit i
    ctrl = _try_unseal(driver, k0, k1, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
      return 1;
    }
  }
  return 0;
}

void _bruteforce_sealed_key(BaseSequentialStream* stream, BQ27500Driver* driver, apalTime_t timeout) {
  chprintf(stream, "start bruteforcing sealed keys...\n");
  bq27500_lld_control_status_t ctrl;
  uint16_t key0_reversed = 0x7236;
  uint16_t key1_reversed = 0x1404;
  uint16_t key0 = BQ27500_LLD_DEFAULT_UNSEAL_KEY0;
  uint16_t key1 = BQ27500_LLD_DEFAULT_UNSEAL_KEY1;
  uint16_t k0 = key0;
  uint16_t k1 = key1;

  // testing default keys in different orders
  chprintf(stream, "\ttry reversed byte order and different key order...\n");
  // default unseal keys
  ctrl = _try_unseal(driver, k0, k1, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  // default unseal keys in reversed order
  ctrl = _try_unseal(driver, k1, k0, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  // byte reversed keys
  k0 = key0_reversed;
  k1 = key1_reversed;
  ctrl = _try_unseal(driver, k0, k1, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  // byte reversed keys in reversed order
  ctrl = _try_unseal(driver, k1, k0, timeout);
  if (ctrl.content.ss == 0x0) {
    chprintf(stream, "\t\tSUCCESS!\n");
    chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", k0, k1);
    return;
  }
  chprintf(stream, "\t\tfailed\n");


  // testing single bit flips of the default keys in different orders
  chprintf(stream, "\ttry single bit flips of default keys...\n");
  // default unseal keys
  uint8_t result = 0;
  result = _bruteforce_sealed_key_bitflips(stream, driver, key0, key1, timeout);
  if (result == 1) {
    return;
  }
  // default unseal keys in reversed order
  result = _bruteforce_sealed_key_bitflips(stream, driver, key1, key0, timeout);
  if (result == 1) {
    return;
  }
  // byte reversed keys
  result = _bruteforce_sealed_key_bitflips(stream, driver, key0_reversed, key1_reversed, timeout);
  if (result == 1) {
    return;
  }
  // byte reversed keys in reversed order
  result = _bruteforce_sealed_key_bitflips(stream, driver, key1_reversed, key0_reversed, timeout);
  if (result == 1) {
    return;
  }
  chprintf(stream, "\t\tfailed\n");


  // bruteforcing one of the keys, assuming only one of them was changed
  chprintf(stream, "\ttry bruteforcing a single key...\n");
  // default unseal key0
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, key0, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key0, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 1/8\n");
  // reversed unseal key0
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, key0_reversed, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key0_reversed, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 2/8\n");
  // default unseal key0 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, i, key0, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key0);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 3/8\n");
  // reversed unseal key0 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, i, key0_reversed, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key0_reversed);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 4/8\n");
  // default unseal key1
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, i, key1, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key1);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 5/8\n");
  // reversed unseal key1
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, i, key1_reversed, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, key1_reversed);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 6/8\n");
  // default unseal key1 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, key1, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key1, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 7/8\n");
  // reversed unseal key1 in reversed order
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    ctrl = _try_unseal(driver, key1_reversed, i, timeout);
    if (ctrl.content.ss == 0x0) {
      chprintf(stream, "\t\tSUCCESS!\n");
      chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", key1_reversed, i);
      return;
    }
  }
  chprintf(stream, "\t\tkey failed. 8/8\n");
  chprintf(stream, "\t\tfailed\n");


  // full bruteforce
  chprintf(stream, "\tbruteforcing both keys...\t");
  for (uint32_t i = 0; i <= 0xFFFF; i++) {
    chprintf(stream, "\t\ti: %u\n", i);
    for (uint32_t j = 0; j <= 0xFFFF; j++) {
      ctrl = _try_unseal(driver, i, j, timeout);
      if (ctrl.content.ss == 0x0) {
        chprintf(stream, "\t\tSUCCESS!\n");
        chprintf(stream, "\t\tkey0: 0x%X, key1: 0x%X\n", i, j);
        return;
      }
    }
  }

  chprintf(stream, "\t\tfailed, no keys could be found");
}

aos_utresult_t utAlldBq27500Func(BaseSequentialStream* stream, aos_unittest_t* ut)
{
  aosDbgCheck(ut->data != NULL && ((ut_bq27500data_t*)(ut->data))->driver != NULL);

  // local variables
  aos_utresult_t result = {0, 0};
  uint32_t status;
  bq27500_lld_batlow_t bl;
  bq27500_lld_batgood_t bg;
  uint16_t dst;
  bq27500_lld_flags_t flags;
  uint16_t subdata = 0;
  uint8_t original_length;
  char original_name[8+1];
  uint8_t val = 0x00;
  uint8_t block[32];
  char new_name[] = "test";
  uint8_t new_lenght;
  char name[8+1] = {'\0'};
  uint8_t sum = 0;
  bool success;
  bool success2;

  chprintf(stream, "read battery low gpio...\n");
  status = bq27500_lld_read_batlow(((ut_bq27500data_t*)ut->data)->driver, &bl);
  chprintf(stream, "\t\tbattery low: 0x%X\n", bl);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "read battery good gpio...\n");
  status = bq27500_lld_read_batgood(((ut_bq27500data_t*)ut->data)->driver, &bg);
  chprintf(stream, "\t\tbattery good: 0x%X\n", bg);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "std command FLAGS...\n");
  status = bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Flags, &flags.value, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tflags: 0x%04X\n", flags.value);
  chprintf(stream, "\t\tbattery detected: 0x%X\n", flags.content.bat_det);
  chprintf(stream, "\t\tbattery fully charged: 0x%X\n", flags.content.fc);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "std command CTNL...\n");
  status = bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "sub command: CTRL Status...\n");
  aosThdUSleep(1);
  status = bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdUSleep(1);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdUSleep(1);
  bq27500_lld_control_status_t ctrl;
  status |= bq27500_lld_sub_command_read(((ut_bq27500data_t*)ut->data)->driver, &ctrl.value, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tdst: 0x%X\n", ctrl.value);
  chprintf(stream, "\t\tsleep: 0x%X\n", ctrl.content.sleep);
  chprintf(stream, "\t\thibernate: 0x%X\n", ctrl.content.hibernate);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "sub command: firmware version...\n");
  status = bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_FW_VERSION, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1);
  bq27500_lld_version_t version;
  status |= bq27500_lld_sub_command_read(((ut_bq27500data_t*)ut->data)->driver, &version.value, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tfirmware version: %X%X-%X%X\n", version.content.major_high, version.content.major_low, version.content.minor_high, version.content.minor_low);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "sub command: hardware version...\n");
  status = bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_HW_VERSION, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_sub_command_read(((ut_bq27500data_t*)ut->data)->driver, &version.value, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\thardware version: %X%X-%X%X\n", version.content.major_high, version.content.major_low, version.content.minor_high, version.content.minor_low);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "ext command: device name length...\n");
  status = bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DeviceNameLength, BQ27500_LLD_EXT_CMD_READ, &original_length, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tdevice name length: %d \n", original_length);
  if (status == APAL_STATUS_SUCCESS && original_length <= 8) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
    original_length = 8;
  }

  chprintf(stream, "ext command: device name (read)...\n");
  status = bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DeviceName, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)original_name, original_length, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  original_name[original_length] = '\0';
  chprintf(stream, "\t\tdevice name: %s\n", original_name);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "battery info std commands...\n");
  status = bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Temperatur, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\ttemperature: %fK (%fC) \n", (float)dst/10.0f, (float)dst/10.0f-273.5f);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_FullAvailableCapacity, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tfull available capacity: %umAh \n", dst);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_FullChargeCapacity, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tfull charge capacity: %umAh \n", dst);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_RemainingCapacity, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tremaining capacity: %umAh \n", dst);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Voltage, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tvoltage: %umV \n", dst);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_AverageCurrent, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\taverage current: %dmA \n", (int8_t)dst);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_AveragePower, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\taverage power: %dmW \n", (int8_t)dst);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_TimeToFull, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  if (dst != (uint16_t)~0) {
    chprintf(stream, "\t\ttime to full: %umin \n", dst);
  } else {
    chprintf(stream, "\t\ttime to full: (not charging) \n", dst);
  }
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_TimeToEmpty, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  if (dst != (uint16_t)~0) {
    chprintf(stream, "\t\ttime to empty: %umin \n", dst);
  } else {
    chprintf(stream, "\t\ttime to empty: (not discharging) \n", dst);
  }
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "check sealed state...\n");
  status = bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  status |= bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_sub_command_read(((ut_bq27500data_t*)ut->data)->driver, &ctrl.value, ((ut_bq27500data_t*)ut->data)->timeout);
  chprintf(stream, "\t\tsealed: 0x%X\n", ctrl.content.ss);
  chprintf(stream, "\t\tfull access sealed: 0x%X\n", ctrl.content.fas);
  if (status == APAL_STATUS_SUCCESS) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "unseale...\n");
  status = bq27500_lld_send_ctnl_data(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_DEFAULT_UNSEAL_KEY1, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_send_ctnl_data(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_DEFAULT_UNSEAL_KEY0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  status |= bq27500_lld_sub_command_read(((ut_bq27500data_t*)ut->data)->driver, &ctrl.value, ((ut_bq27500data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && ctrl.content.ss == 0x0) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
    _bruteforce_sealed_key(stream, ((ut_bq27500data_t*)ut->data)->driver, ((ut_bq27500data_t*)ut->data)->timeout);
  }

  chprintf(stream, "read device name from data flash...\n");
  status = bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataControl, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  val = 0x30;
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashClass, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  val = 0;
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashBlock, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)block, original_length, 13, ((ut_bq27500data_t*)ut->data)->timeout);
  block[original_length] = '\0';
  chprintf(stream, "\t\tdevice name: %s\n", block);
  if (status == APAL_STATUS_SUCCESS && (strcmp((char*)block, (char*)original_name) == 0)) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\t\tread data: ");
    for (uint8_t blockIdx = 0; blockIdx < original_length; blockIdx++) {
      chprintf(stream, "0x%02X\n", block[blockIdx]);
    }
    aosUtFailedMsg(stream, &result, "0x%08X\n", status);
  }

  chprintf(stream, "change device name in data flash to \"test\"...\n");
  status = bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_WRITE, (uint8_t*)new_name, 5, 13, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)block, 32, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  // compute blockdata checksum
  status |= bq27500_lld_compute_blockdata_checksum(block, &sum);
  // write checksum to BlockDataChecksum, triggering the write of BlackData
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataCheckSum, BQ27500_LLD_EXT_CMD_WRITE, &sum, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DeviceNameLength, BQ27500_LLD_EXT_CMD_READ, &new_lenght, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  // read out device name, to see if changing it was successfull
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DeviceName, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)name, new_lenght, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  name[new_lenght] = '\0';
  chprintf(stream, "\t\tdevice name: %s\n", name);
  success = (strcmp(name, new_name) == 0);

  // change device name back to original name
  val = 0x00;
  status = bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataControl, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  val = 0x30;
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashClass, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  val = 0;
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DataFlashBlock, BQ27500_LLD_EXT_CMD_WRITE, &val, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_WRITE, (uint8_t*)original_name, 7, 13, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockData, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)block, 32, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  // compute blockdata checksum
  sum = 0;
  status |= bq27500_lld_compute_blockdata_checksum(block, &sum);
  // write checksum to BlockDataChecksum, triggering the write of BlackData
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_BlockDataCheckSum, BQ27500_LLD_EXT_CMD_WRITE, &sum, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(1000);
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DeviceNameLength, BQ27500_LLD_EXT_CMD_READ, &original_length, 1, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  // read device name, to see if changing it back to the original name was successfull
  status |= bq27500_lld_ext_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_EXT_CMD_DeviceName, BQ27500_LLD_EXT_CMD_READ, (uint8_t*)name, original_length, 0, ((ut_bq27500data_t*)ut->data)->timeout);
  success2 = (strcmp(name, original_name) == 0);
  name[original_length] = '\0';
  chprintf(stream, "\t\tchanged back to name: %s, original_name: %s\n", name, original_name);
  if (status == APAL_STATUS_OK && ((success && success2) || (ctrl.content.sleep == 0))) {
    aosUtPassed(stream, &result);
  } else {
    aosUtFailedMsg(stream, &result, "0x%08X, changing: 0x%X - changing back: 0x%X\n", status, success, success2);
  }

  chprintf(stream, "seal...\n");
  status = bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_SEALED, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_std_command(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_STD_CMD_Control, &dst, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_sub_command_call(((ut_bq27500data_t*)ut->data)->driver, BQ27500_LLD_SUB_CMD_CONTROL_STATUS, ((ut_bq27500data_t*)ut->data)->timeout);
  aosThdMSleep(50);
  status |= bq27500_lld_sub_command_read(((ut_bq27500data_t*)ut->data)->driver, &ctrl.value, ((ut_bq27500data_t*)ut->data)->timeout);
  if (status == APAL_STATUS_SUCCESS && ctrl.content.ss == 0x1) {
    aosUtPassed(stream, &result);
  } else {
    chprintf(stream, "\tfailed (0x%X)\n", status);
    aosUtFailedMsg(stream, &result, "0x%08X, ctrl 0x%X\n", status, subdata);
    ++result.failed;
  }

  aosUtInfoMsg(stream,"driver object memory footprint: %u bytes\n", sizeof(BQ27500Driver));

  return result;
}

#endif /* (AMIROOS_CFG_TESTS_ENABLE == true) && defined(AMIROLLD_CFG_USE_BQ27500) */

