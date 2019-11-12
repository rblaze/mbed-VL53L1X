// Copyright (c) 2019, Andrey Sverdlichenko - All Rights Reserved

#include "LaserSensor.h"
#include <mbed.h>

LaserSensor::LaserSensor(mbed::I2C &bus, int address)
    : dev_{VL53L1_RegisterDevice(bus, address)} {
  // Wait for sensor to boot.
  for (uint8_t bootState = 0; bootState == 0; /**/) {
    VL53L1X_BootState(dev_, &bootState);
    if (bootState == 0) {
      // Wait for 1ms. Expected boot time is 1.2ms.
      ThisThread::sleep_for(1);
    }
  }

  // Init device.
  VL53L1X_SensorInit(dev_);
}

bool LaserSensor::measureOnce() {
  bool success = false;

  if (VL53L1X_StartRanging(dev_)) {
    return false;
  }

  for (uint8_t dataReady = 0; dataReady != 0; /**/) {
    if (VL53L1X_CheckForDataReady(dev_, &dataReady)) {
      goto err;
    }
  }

  if (VL53L1X_GetResult(dev_, &result_)) {
    goto err;
  }

  success = true;

err:
  VL53L1X_StopRanging(dev_);
  return success;
}

LaserSensor::RangeStatus LaserSensor::getStatus() const {
  return static_cast<RangeStatus>(result_.Status);
}

uint16_t LaserSensor::getDistance() const { return result_.Distance; }
