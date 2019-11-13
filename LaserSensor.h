// Copyright (c) 2019, Andrey Sverdlichenko - All Rights Reserved

#pragma once

#include "VL53L1X_api.h"
#include <I2C.h>

class LaserSensor {
public:
  // Measurment status.
  enum class RangeStatus {
    // When the range status is 0, there is no error.
    NO_ERROR = 0,
    // Range status 1 and 2 are error warnings while range status 4
    // and 7 are errors.
    //
    // When the range status is 1, there is a sigma failure. This means
    // that the repeatability or standard deviation of the measurement is
    // bad due to a decreasing signal noise ratio. Increasing the timing
    // budget can improve the standard deviation and avoid a range status 1.
    SIGMA_FAILURE = 1,
    // When the range status is 2, there is a signal failure. This means
    // that the return signal is too week to return a good answer. The
    // reason is because the target is too far, or the target is not
    // reflective enough, or the target is too small. Increasing the timing
    // buget might help, but there may simply be no target available.
    SIGNAL_FAILURE = 2,
    // When the range status is 4, the sensor is "out of bounds". This
    // means that the sensor is ranging in a "non-appropriated" zone and
    // the measured result may be inconsistent. This status is considered
    // as a warning but, in general, it happens when a target is at the
    // maximum distance possible from the sensor, i.e. around 5 m. However,
    // this is only for very bright targets.
    SENSOR_OUT_OF_BOUNDS = 4,
    // Range status 7 is called "wraparound". This situation may occur
    // when the target is very reflective and the distance to the target/sensor
    // is longer than the physical limited distance measurable by the sensor.
    // Such distances include approximately 5 m when the senor is in Long
    // distance mode and approximately 1.3 m when the sensor is in Short
    // distance mode. Example: a traffic sign located at 6 m can be seen by
    // the sensor and returns a range of 1 m. This is due to "radar aliasing":
    // if only an approximate distance is required, we may add 6 m to the
    // distance returned. However, that is a very approximate estimation.
    WRAPAROUND = 7,
  };

  LaserSensor(mbed::I2C &bus, int address);
  ~LaserSensor();

  // Set scan mode.
  bool setShortDistanceMode() { return VL53L1X_SetDistanceMode(dev_, 1) == 0; }

  bool setLongDistanceMode() { return VL53L1X_SetDistanceMode(dev_, 2) == 0; }

  // The TB values available are [15, 20,33, 50, 100, 200, 500]. This function
  // must be called after VL53L1X_SetDistanceMode.
  // Note:15 ms only works with Short distance mode. 100 ms is the default
  // value.
  // The IMP must be greater than or equal to the TB otherwise the start of
  // ranging is missed and theintermeaurement period is effectively doubled.

  bool setTiming(uint16_t budgetMs, uint32_t periodMs);

  // Get range once. Returns true if result is ready, false for error.
  bool measureOnce();

  // Last scan status.
  RangeStatus getStatus() const;

  // Last scan's distance in mm.
  uint16_t getDistance() const;

  // Get device ID for direct API calls.
  uint16_t dev() const { return dev_; }

  // Default I2C address.
  static const int ADDR_DEFAULT = 0x52;

  // Disable copying. It may be made working, but why bother.
  LaserSensor(const LaserSensor &) = delete;
  LaserSensor &operator=(const LaserSensor &) = delete;

private:
  uint16_t dev_;
  VL53L1X_Result_t result_;
};
