// Use this to determine the offset of the readings in the x, y, and z.
// Start by aligning the z-axis upwards such that the x and y axes are horizontal and run
// to determine the x and y offsets. Next, align the z-axis horizontal and run to determine
// the z offset.

#include "mpu9250.h"

struct Reading {
    float x;
    float y;
    float z;
};

struct CalibratedMPU {

    Mpu9250* imu_ptr;

    CalibratedMPU(Mpu9250* imu) {
        imu_ptr = imu;
    }

    Reading get_accel() {
        // Adjust x, y, and z acceleration readings based on offsets determined
        // by calibration.
        auto x =  imu_ptr->accel_y_mps2();
        auto y = -imu_ptr->accel_x_mps2();
        auto z =  imu_ptr->accel_z_mps2();
        return Reading{ x, y, z };
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
    float sum_x = 0;
    float sum_y = 0;
    float sum_z = 0;
    for (int i = 0; i < 1000; i++) {
        if (imu.Read()) {
            auto reading = calibrated_imu.get_accel();
            Serial.print("Trial ");
            Serial.print(i);
            Serial.print(":\t");
            Serial.print(reading.x, 6);
            Serial.print("\t");
            Serial.print(reading.y, 6);
            Serial.print("\t");
            Serial.println(reading.z, 6);

            sum_x += reading.x;
            sum_y += reading.y;
            sum_z += reading.z;
        }
        else i--;
    }

    auto x_offset = sum_x / 1000;
    auto y_offset = sum_y / 1000;
    auto z_offset = sum_z / 1000;

    Serial.print("x_offset: ");
    Serial.print(x_offset, 8);
    Serial.print("\ty_offset: ");
    Serial.print(y_offset, 8);
    Serial.print("\tz_offset: ");
    Serial.print(z_offset, 8);

    while (1) {}
}
