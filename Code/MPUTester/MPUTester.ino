#include "mpu9250.h"
#include <math.h>
#include <Servo.h>

// Adjust offset based on calibration.
const auto a_x_offset = -1.62163782;
const auto a_y_offset =  1.07151603;
const auto a_z_offset =  5.31442928;
const auto m_x_offset = -3.11650466;
const auto m_y_offset = 12.11313533;
const auto m_z_offset = 16.39547157;
const auto m_x_scale = 0.91999950;
const auto m_y_scale = 0.95515546;
const auto m_z_scale = 1.15461063;

struct Reading {
    // Calibrated values to be returned by the IMU.
    const float pitch_angle;
    const float roll_angle;
    const float heading_angle;
    
    
    Reading(const float pitch_angle, const float roll_angle, const float heading_angle)
        : pitch_angle{ pitch_angle * 180 / M_PI }, 
          roll_angle{ roll_angle * 180 / M_PI }, 
          heading_angle{ heading_angle * 180 / M_PI }
    {
    
    }
};

struct CalibratedMPU : Mpu9250 {

    using Mpu9250::Mpu9250;

    Reading get_readings() {
        // Adjust x, y, and z acceleration readings based on offsets determined by calibration.
        // This is aligned such that the +x axis labeled on the accelerometer corresponds to the
        // 1-body axis, the -y axis corresponds to the 2-body axis, and the -z axis corresponds
        // to the 3-body axis.

        // Accelerometer Correction
        auto a_x =  accel_y_mps2() + a_x_offset;
        auto a_y = -accel_x_mps2() + a_y_offset;
        auto a_z = -accel_z_mps2() + a_z_offset;

        // Magnetometer Correction
        // Correct Orientation
        auto m_x =  mag_y_ut();
        auto m_y = -mag_x_ut();
        auto m_z =  mag_z_ut();
        // Correct offsets and scaling
        m_x = (m_x - m_x_offset) * m_x_scale;
        m_y = (m_y - m_y_offset) * m_y_scale;
        m_z = (m_z - m_z_offset) * m_z_scale;

        // Pitch, Roll, and Heading Angle Calculations
        auto pitch_angle = atan2(a_x, sqrt(pow(a_y, 2) + pow(a_z, 2)));
        auto roll_angle = atan2(-a_y, a_z);
        auto heading_angle = atan2(m_z * sin(roll_angle) - m_y * cos(roll_angle), 
            m_x*cos(pitch_angle) + m_y*sin(pitch_angle)*sin(roll_angle) + m_z*sin(pitch_angle)*cos(roll_angle) );
        return Reading{ pitch_angle, roll_angle, heading_angle };
    }

};

struct CalibratedServo : Servo {
    CalibratedServo(float scale, float offset) : Servo(), scale{ scale }, offset{ offset } {
      
    }

    void write(float angle) {
      Servo::write(scale * angle + offset);
    }

    private:
      float offset;
      float scale;
};

/* An Mpu9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68 */
CalibratedMPU imu{&Wire, 0x68};
CalibratedServo rollServo{(float)(89-180)/(180), 89};
CalibratedServo pitchServo{((float)89)/84 , 89};

void setup() {
    imu.Begin();
    imu.ConfigAccelRange(imu.AccelRange::ACCEL_RANGE_2G);
    rollServo.attach(3);
    pitchServo.attach(5);
}

void loop() {
    if (imu.Read()) {
        auto reading = imu.get_readings();
        rollServo.write(reading.roll_angle);
        pitchServo.write(reading.pitch_angle);
    }
}
