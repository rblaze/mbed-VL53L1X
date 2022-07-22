add_library(vl53l1x-driver INTERFACE)

target_include_directories(vl53l1x-driver
    INTERFACE
        .
)

target_sources(vl53l1x-driver
    INTERFACE
        LaserSensor.cpp
        vl53l1_platform.cpp
        VL53L1X_api.cpp
        VL53L1X_calibration.cpp
)

