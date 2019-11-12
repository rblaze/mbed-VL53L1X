#pragma once

#include "VL53L1X_api.h"
#include <I2C.h>

class LaserSensor {
public:
  LaserSensor(mbed::I2C &bus, int address);

  // Get range once. Returns true if result is ready, false for error.
  bool measureOnce();

  // Measurment status.
  enum class RangeStatus {
    NO_ERROR = 0,
    SIGMA_FAILURE = 1,
    SIGNAL_FAILURE = 2,
    SENSOR_OUT_OF_BOUNDS = 4,
    WRAPAROUND = 7,
  };

  // Last scan status.
  RangeStatus getStatus() const;

  // Last scan's distance in mm.
  uint16_t getDistance() const;

  // Get device ID for direct API calls.
  uint16_t dev() const { return dev_; }

  // Default I2C address.
  static const int ADDR_DEFAULT = 0x52;

private:
  uint16_t dev_;
  VL53L1X_Result_t result_;
};
