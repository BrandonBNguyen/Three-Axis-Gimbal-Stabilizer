// Use this to determine the offset of the readings in the x, y, and z.
// Start by aligning the z-axis upwards such that the x and y axes are horizontal and run
// to determine the x and y offsets. Next, align the z-axis horizontal and run to determine
// the z offset.

#include "mpu9250.h"

struct Reading {
    float a_x;
    float a_y;
    float a_z;
    float m_x;
    float m_y;
    float m_z;
};

struct CalibratedMPU {

    Mpu9250* imu_ptr;

    CalibratedMPU(Mpu9250* imu) {
        imu_ptr = imu;
    }

    Reading get_accel() {
        // Adjust x, y, and z acceleration readings based on offsets determined
        // by calibration.
        auto a_x =  imu_ptr->accel_y_mps2();
        auto a_y = -imu_ptr->accel_x_mps2();
        auto a_z =  imu_ptr->accel_z_mps2();
        auto m_x =  imu_ptr->mag_y_ut();
        auto m_y = -imu_ptr->mag_x_ut();
        auto m_z =  imu_ptr->mag_z_ut();
        return Reading{ a_x, a_y, a_z, m_x, m_y, m_z };
    }

};

/* An Mpu9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68 */
Mpu9250 imu(&Wire, 0x68);
CalibratedMPU calibrated_imu{ &imu };

void setup() {
    /* Serial to display data */
    Serial.begin(115200);
    while (!Serial) {}
    /* Start communication */
    if (!imu.Begin()) {
        Serial.println("IMU initialization unsuccessful");
        while (1) {}
    }

    imu.ConfigAccelRange(0x00);
}

void loop() {
    float sum_a_x = 0;
    float sum_a_y = 0;
    float sum_a_z = 0;
    float sum_m_x = 0;
    float sum_m_y = 0;
    float sum_m_z = 0;
    for (int i = 0; i < 1000; i++) {
        if (imu.Read()) {
            auto reading = calibrated_imu.get_accel();
            Serial.print("Trial ");
            Serial.print(i);
            Serial.print(":\t");
            Serial.print(reading.a_x, 6);
            Serial.print("\t");
            Serial.print(reading.a_y, 6);
            Serial.print("\t");
            Serial.print(reading.a_z, 6);
            Serial.print(":\t");
            Serial.print(reading.m_x, 6);
            Serial.print("\t");
            Serial.print(reading.m_y, 6);
            Serial.print("\t");
            Serial.println(reading.m_z, 6);

            sum_a_x += reading.a_x;
            sum_a_y += reading.a_y;
            sum_a_z += reading.a_z;
            sum_m_x += reading.m_x;
            sum_m_y += reading.m_y;
            sum_m_z += reading.m_z;
        }
        else i--;
    }

    auto a_x_offset = sum_a_x / 1000;
    auto a_y_offset = sum_a_y / 1000;
    auto a_z_offset = sum_a_z / 1000;
    auto m_x_offset = sum_m_x / 1000;
    auto m_y_offset = sum_m_y / 1000;
    auto m_z_offset = sum_m_z / 1000;

    Serial.print("a_x_offset: ");
    Serial.print(a_x_offset, 8);
    Serial.print("\ta_y_offset: ");
    Serial.print(a_y_offset, 8);
    Serial.print("\ta_z_offset: ");
    Serial.println(a_z_offset, 8);

    Serial.print("m_x_average: ");
    Serial.print(m_x_offset, 8);
    Serial.print("\tm_y_average: ");
    Serial.print(m_y_offset, 8);
    Serial.print("\tm_z_average: ");
    Serial.print(m_z_offset, 8);

    while (1) {}
}
