
/*
* This file is part of VL53L1 Platform
*
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved
*
* License terms: BSD 3-clause "New" or "Revised" License.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its contributors
* may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "vl53l1_platform.h"
#include <math.h>
#include <string.h>
#include <time.h>

namespace {

constexpr uint16_t kMaxDevices = MBED_CONF_VL53L1X_DRIVER_MAX_DEVICES;

struct Device {
  mbed::I2C *i2c{nullptr};
  int addr{0};
};

Device knownDevices[kMaxDevices];

} // namespace

uint16_t VL53L1_RegisterDevice(mbed::I2C *i2c, int address) {
  for (uint16_t i = 0; i < kMaxDevices; i++) {
    if (knownDevices[i].i2c == nullptr) {
      knownDevices[i].i2c = i2c;
      knownDevices[i].addr = address;

      return i;
    }
  }

  return UINT16_MAX;
}

void VL53L1_UnregisterDevice(uint16_t dev) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  knownDevices[dev].i2c = nullptr;
  knownDevices[dev].addr = 0;
}

#if 0
int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	return 0; // to be implemented
}
#endif

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata,
                        uint32_t count) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[2] = {(uint8_t)(index >> 8), (uint8_t)index};

  device.i2c->write(device.addr, (const char *)buf, sizeof(buf), true);
  return device.i2c->read(device.addr, (char *)pdata, count);
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[3] = {(uint8_t)(index >> 8), (uint8_t)index, data};
  return device.i2c->write(device.addr, (const char *)buf, sizeof(buf));
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[4] = {(uint8_t)(index >> 8), (uint8_t)index, (uint8_t)(data >> 8),
                    (uint8_t)data};
  return device.i2c->write(device.addr, (const char *)buf, sizeof(buf));
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[6] = {(uint8_t)(index >> 8), (uint8_t)index,
                    (uint8_t)(data >> 24), (uint8_t)(data >> 16),
                    (uint8_t)(data >> 8),  (uint8_t)data};
  return device.i2c->write(device.addr, (const char *)buf, sizeof(buf));
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[2] = {(uint8_t)(index >> 8), (uint8_t)index};

  device.i2c->write(device.addr, (const char *)buf, sizeof(buf), true);
  return device.i2c->read(device.addr, (char *)data, 1);
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[2] = {(uint8_t)(index >> 8), (uint8_t)index};

  device.i2c->write(device.addr, (const char *)buf, sizeof(buf), true);
  auto status = device.i2c->read(device.addr, (char *)buf, sizeof(buf));
  if (status == 0) {
    *data = (buf[0] << 8) | buf[1];
  }
  return status;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
  MBED_ASSERT(dev < kMaxDevices);
  MBED_ASSERT(knownDevices[dev].i2c != nullptr);

  auto &device = knownDevices[dev];
  uint8_t buf[2] = {(uint8_t)(index >> 8), (uint8_t)index};

  device.i2c->write(device.addr, (const char *)buf, sizeof(buf), true);

  uint8_t rdbuf[4];
  auto status = device.i2c->read(device.addr, (char *)rdbuf, sizeof(rdbuf));
  if (status == 0) {
    *data = (rdbuf[0] << 24) | (rdbuf[1] << 16) | (rdbuf[2] << 8) | rdbuf[3];
  }
  return status;
}

#if 0
int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms) {
  return 0;  // to be implemented
}
#endif
