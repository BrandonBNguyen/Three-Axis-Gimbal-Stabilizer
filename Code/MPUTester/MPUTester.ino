#include "mpu9250.h"
#include <math.h>

// Adjust offset based on calibration.
const auto x_offset = -1.62163782;
const auto y_offset =  1.07151603;
const auto z_offset =  5.31442928;

struct Reading {
    // Calibrated values to be returned by the IMU.
    const float a_x;
    const float a_y;
    const float a_z;
    const float pitch_angle;
    const float roll_angle;

    Reading(const float a_x, const float a_y, const float a_z, const float pitch_angle, const float roll_angle)
        : a_x{ a_x }, a_y{ a_y }, a_z{ a_z }, pitch_angle{ pitch_angle }, roll_angle{ roll_angle } {
    }
};

struct CalibratedMPU : Mpu9250 {

    using Mpu9250::Mpu9250;

    Reading get_readings() {
        // Adjust x, y, and z acceleration readings based on offsets determined by calibration.
        // This is aligned such that the +x axis labeled on the accelerometer corresponds to the
        // 1-body axis, the -y axis corresponds to the 2-body axis, and the -z axis corresponds
        // to the 3-body axis.
        auto a_x =  accel_y_mps2() + x_offset;
        auto a_y = -accel_x_mps2() + y_offset;
        auto a_z = -accel_z_mps2() + z_offset;
        auto pitch_angle = 180 * atan2(a_x, sqrt(pow(a_y, 2) + pow(a_z, 2))) / M_PI;
        auto roll_angle = 180 * atan2(-a_y, a_z) / M_PI;
        return Reading{ a_x, a_y, a_z, pitch_angle, roll_angle };
    }

};

/* An Mpu9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68 */
CalibratedMPU imu(&Wire, 0x68);

void setup() {
    /* Serial to display data */
    Serial.begin(115200);
    while (!Serial) {}
    /* Start communication */
    if (!imu.Begin()) {
        Serial.println("IMU initialization unsuccessful");
        while (1) {}
    }

    imu.ConfigAccelRange(imu.AccelRange::ACCEL_RANGE_2G);
}

void loop() {
    while (!Serial) {}
    if (imu.Read()) {
        auto reading = imu.get_readings();
//        Serial.print(reading.a_x, 6);
//        Serial.print(" ");
//        Serial.print(reading.a_y, 6);
//        Serial.print(" ");
//        Serial.println(reading.a_z, 6);
        Serial.print(reading.pitch_angle, 6);
        Serial.print(" ");
        Serial.println(reading.roll_angle, 6);
    }
    delay(100);
}
