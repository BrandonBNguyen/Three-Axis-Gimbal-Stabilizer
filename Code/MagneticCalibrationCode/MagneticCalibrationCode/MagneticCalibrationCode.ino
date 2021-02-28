#include "mpu9250.h"
#include <math.h>

// Adjust offset based on calibration.
const auto a_x_offset = -1.62163782;
const auto a_y_offset = 1.07151603;
const auto a_z_offset = 5.31442928;

struct Reading {
    static float max_m_x;
    static float max_m_y;
    static float max_m_z;
    static float min_m_x;
    static float min_m_y;
    static float min_m_z;
    static float offset_m_x;
    static float offset_m_y;
    static float offset_m_z;
    static float scale_m_x;
    static float scale_m_y;
    static float scale_m_z;

    const float m_x;
    const float m_y;
    const float m_z;
    const float pitch_angle;
    const float roll_angle;
    
    Reading(const float m_x, const float m_y, const float m_z, const float pitch_angle, const float roll_angle)
        : m_x{ m_x }, m_y{ m_y }, m_z{ m_z }, pitch_angle{ pitch_angle }, roll_angle{ roll_angle }
    {
        if (m_x > max_m_x) max_m_x = m_x;
        if (m_x < min_m_x) min_m_x = m_x;
        if (m_y > max_m_y) max_m_y = m_y;
        if (m_y < min_m_y) min_m_y = m_y;
        if (m_z > max_m_z) max_m_z = m_z;
        if (m_z < min_m_z) min_m_z = m_z;

        offset_m_x = (max_m_x + min_m_x) / 2;
        offset_m_y = (max_m_y + min_m_y) / 2;
        offset_m_z = (max_m_z + min_m_z) / 2;

        auto avg_delta_x = (max_m_x - min_m_x) / 2;
        auto avg_delta_y = (max_m_y - min_m_y) / 2;
        auto avg_delta_z = (max_m_z - min_m_z) / 2;
        auto avg_delta = (avg_delta_x + avg_delta_y + avg_delta_z) / 3;
        scale_m_x = avg_delta / avg_delta_x;
        scale_m_y = avg_delta / avg_delta_y;
        scale_m_z = avg_delta / avg_delta_z;
    }

    float corrected_m_x() {
        return (m_x - offset_m_x) * scale_m_x;
    }

    float corrected_m_y() {
        return (m_y - offset_m_y) * scale_m_y;
    }

    float corrected_m_z() {
        return (m_z - offset_m_z) * scale_m_z;
    }

    float heading_angle() {
        auto m_x = corrected_m_x();
        auto m_y = corrected_m_y();
        auto m_z = corrected_m_z();
        auto heading_angle = atan2(m_z * sin(roll_angle) - m_y * cos(roll_angle),
            m_x * cos(pitch_angle) + m_y * sin(pitch_angle) * sin(roll_angle) + m_z * sin(pitch_angle) * cos(roll_angle));
        return heading_angle * 180 / M_PI;
    }
};

// Initialize maxes and mins to maximum and minimum values possible for a float.
float Reading::max_m_x = -3.402823466e+38;
float Reading::max_m_y = -3.402823466e+38;
float Reading::max_m_z = -3.402823466e+38;
float Reading::min_m_x =  3.402823466e+38;
float Reading::min_m_y =  3.402823466e+38;
float Reading::min_m_z =  3.402823466e+38;
float Reading::offset_m_x = 0;
float Reading::offset_m_y = 0;
float Reading::offset_m_z = 0;
float Reading::scale_m_x = 0;
float Reading::scale_m_y = 0;
float Reading::scale_m_z = 0;

struct CalibratedMPU : Mpu9250 {

    using Mpu9250::Mpu9250;

    Reading get_readings() {
        // This is aligned such that the +x axis labeled on the accelerometer corresponds to the
        // 1-body axis, the -y axis corresponds to the 2-body axis, and the -z axis corresponds
        // to the 3-body axis.
        auto m_x = mag_y_ut();
        auto m_y = -mag_x_ut();
        auto m_z = mag_z_ut();
        auto a_x = accel_y_mps2() + a_x_offset;
        auto a_y = -accel_x_mps2() + a_y_offset;
        auto a_z = -accel_z_mps2() + a_z_offset;
        auto pitch_angle = atan2(a_x, sqrt(pow(a_y, 2) + pow(a_z, 2)));
        auto roll_angle = atan2(-a_y, a_z);
        return Reading{ m_x, m_y, m_z, pitch_angle, roll_angle };
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
        Serial.print("Raw Readings:\tm_x: ");
        Serial.print(reading.m_x);
        Serial.print("\tm_y: ");
        Serial.print(reading.m_y);
        Serial.print("\tm_z: ");
        Serial.println(reading.m_z);
        Serial.print("Max Readings:\tm_x: ");
        Serial.print(Reading::max_m_x);
        Serial.print("\tm_y: ");
        Serial.print(Reading::max_m_y);
        Serial.print("\tm_z: ");
        Serial.println(Reading::max_m_z);
        Serial.print("Min Readings:\tm_x: ");
        Serial.print(Reading::min_m_x);
        Serial.print("\tm_y: ");
        Serial.print(Reading::min_m_y);
        Serial.print("\tm_z: ");
        Serial.println(Reading::min_m_z);
        Serial.print("Offsets\tx: ");
        Serial.print(Reading::offset_m_x, 8);
        Serial.print("\ty: ");
        Serial.print(Reading::offset_m_y, 8);
        Serial.print("\tz: ");
        Serial.println(Reading::offset_m_z, 8);
        Serial.print("Scales\tx: ");
        Serial.print(Reading::scale_m_x, 8);
        Serial.print("\ty: ");
        Serial.print(Reading::scale_m_y, 8);
        Serial.print("\tz: ");
        Serial.println(Reading::scale_m_z, 8);
        Serial.print("Corrected Readings:\tm_x: ");
        Serial.print(reading.corrected_m_x());
        Serial.print("\tm_y: ");
        Serial.print(reading.corrected_m_y());
        Serial.print("\tm_z: ");
        Serial.println(reading.corrected_m_z());
        Serial.print("Heading Angle: ");
        Serial.println(reading.heading_angle());
        Serial.println();
        Serial.println();
    }
    delay(100);
}