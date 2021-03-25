# Three Axis Gimbal Stabilizer

## Introduction

The goal of this project was to design and develop a gimbal system capable of stabilizing a platform based on input from an accelerometer/magnetometer sensor and to demonstrate knowledge of aerospace concepts (namely aircraft principal axes and how to calculate their orientations from accelerometer readings) and ability to apply them in a hands-on project. In aerospace, a gimbal stabilizer can be used to control the orientation of any platform or instrument where the orientation must be independent of the orientation of the aircraft, such as in a camera or targeting system.

![Three-Axis Gimbal Stabilizer Showcase](https://github.com/BrandonBNguyen/Three-Axis-Gimbal-Stabilizer/blob/main/Images/stabilizer%20showcase.gif?raw=true)

## Theory

The orientation of an aircraft can be expressed as a series of rotations required to achieve that orientation starting from an initial orientation aligned with the local coordinate system, where the nose, right wing, and underside of the plane are aligned with north, east, and down, respectively, at some location on the Earth. Euler angles are the angles used to express the sequence of rotations about the principal axes of the aircraft to achieve the desired orientation from the described starting orientation. One such sequence of rotations commonly used in aerospace is yaw, pitch, and roll, where the desired orientation is described by the angle with which to yaw, then pitch, then roll the aircraft from the starting orientation aligned with the local coordinate system.

![Axes body rotations as shown for a plane](https://www.grc.nasa.gov/www/k-12/airplane/Images/rotations.gif)

To determine the roll and pitch angles to achieve the current orientation of an aircraft from an accelerometer, which measures acceleration along the body-axis angles, the following equations are used. Note that ![image](https://user-images.githubusercontent.com/19226773/112064150-c2993680-8b1f-11eb-8506-df90c7d99604.png) refers to the axis pointing out the nose of the plane, ![image](https://user-images.githubusercontent.com/19226773/112064219-d9d82400-8b1f-11eb-8d28-8dbb1afd5a1d.png) refers to the axis pointing out the right wing of the plane, and ![image](https://user-images.githubusercontent.com/19226773/112064251-e492b900-8b1f-11eb-9c56-ba1387fe8e43.png) refers to the axis pointing out the bottom of the plane.

![image](https://user-images.githubusercontent.com/19226773/112064676-9631ea00-8b20-11eb-81db-6eaba842998c.png)

![image](https://user-images.githubusercontent.com/19226773/112064768-bcf02080-8b20-11eb-8d58-c80a8a41bf3a.png)

To determine the yaw angle, the magnetometer must be used and the deflection between magnetic north and true north, ![image](https://user-images.githubusercontent.com/19226773/112065019-2cfea680-8b21-11eb-841b-2c8bf099a2ca.png)
, at the aircraft's location must be known.

![image](https://user-images.githubusercontent.com/19226773/112065132-5fa89f00-8b21-11eb-9a88-f0e400b1ec82.png)

Knowing the euler angles for an aircraft, a series of motors can be driven to rotate a platform in roll, pitch, and yaw, in order to keep the platform level relative to the ground regardless of the orientation of the aircraft.

## Part Selection

### Sensor Selection

The [HiLetgo MPU9250/6500](https://www.amazon.com/gp/product/B01I1J0Z7Y/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) was selected because it contains both an accelerometer and magnetometer and would be sufficient to acquire enough information to calculate the roll, pitch, and yaw angles required to drive the motors to level the platform.

![HiLetgo MPU9250/6500](https://github.com/BrandonBNguyen/Three-Axis-Gimbal-Stabilizer/blob/main/Images/MPU9250.PNG?raw=true)

### Actuator Selection

[MG90S micro servo motors](https://www.amazon.com/gp/product/B07F7VJQL5/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) were selected to actuate the gimbals. Servo motors were selected because of the positional feedback built into them, allowing precise control over the direction of their end effectors.

![MG90S micro servo motors](https://github.com/BrandonBNguyen/Three-Axis-Gimbal-Stabilizer/blob/main/Images/MG90S%20Micro%20Servo.PNG?raw=true)

### Microcontroller Selection

An [Arduino Nano](https://www.amazon.com/gp/product/B0713XK923/ref=ppx_yo_dt_b_search_asin_title?ie=UTF8&psc=1) was used as the microcontroller for this project as it has i2c capabilities for communicating with the accelerometer and requisite PWM pins for controlling the servo motors.

![Arduino Nano](https://github.com/BrandonBNguyen/Three-Axis-Gimbal-Stabilizer/blob/main/Images/Arduino%20Nano.PNG?raw=true)

## CAD Model

The gimbal assembly consists of a wheel in the center capable of rotating 360° to turn to a given yaw angle. The wheel is enclosed in a ring capable of rotating 180° to turn to a given pitch angle. Finally, this ring is enclosed in another ring capable of rotating 360° to turn to a given roll angle. The yaw wheel and roll ring are driven by a gear connected to a servo with a 1:2 gear ratio in order to achieve the full 360° range of motion, since the serrvos themselves can only achieve a 180° range of motion.

The Arduino Nano and the MPU9250 accelerometer are connected to the base supporting the gimbal assembly.

![Gimbal Assembly](https://github.com/BrandonBNguyen/Three-Axis-Gimbal-Stabilizer/blob/main/Images/Gimbal%20Assembly.PNG?raw=true)

## Technical Challenges

### MPU9250/6500 Calibration

#### Accelerometer Calibration

Initial testing with the accelerometer yielded unexpected results as along each axis, I was getting values that were too low or too high compared to gravitational acceleration. For example, pointing the +x axis down would yield a value around 11.4 meters per second squared whereas pointing the -x axis would yield a value around -8.2 meters per second squared. Recognizing that the difference between these two values was around double gravitational acceleration, I knew that there was an accelerational offset that had to be incorporated to get the true acceleration along that axis. All axes showed the same behaviour so I had to find the offset along each axis. To find this offset, I wrote `CalibrationCode.ino`, which would average the acceleration value along all axes for 1000 trials. Each axis was aligned with the horizon before running this code to find and record the offset for each axis.

To incorporate these offsets, I wrote a class called `CalibratedMPU` which would be a subclass of `Mpu9250`, used to retrieve measurements from the accelerometer. The function, `get_readings()`, would call the parent functions `accel_x_mps2()`, `accel_y_mps2()`, and `accel_z_mps2()`, add the offsets found to the value returned by those functions, and use those as the true readings for acceleration along those particular axes. `get_readings()` then uses these values to calculate and return the pitch and roll angle.

#### Magnetometer Calibration

Magnetometer calibration involved accounting for hard iron distortion, that is distortion caused by magnetic material mounted nearby to the accelerometer, and soft iron distortion, which is caused by nearby non-magnetic materials that cause distortions to magnetic fields passing through them. Scale biases are implemented to account for both these type of distortions following the pseudocode provided by Mika Tuupola in [this article](https://appelsiini.net/2018/calibrate-magnetometer/). 

Finding the scales and offsets required to be applied to the raw values read from the magnetometer to get accurate readings, these corrections were implemented in the `get_readings()` function, returning corrected magnetic field readings along each axis. These, along with the calculated pitch and roll angles, are used to calculate and return the heading angle.

### Servo Calibration

Although the servo motors were marketed as having a 180° range of motion, that was not the case. In reality, the servos had about 165°~170° range of motion. Additionally, the roll and yaw rings have a 1:2 gear ratio from the output of the servo. Therefore, a scaling factor from the difference in servo command signal to the difference in actual rotation for each ring had to be found. In addition, an offset had to be found for each servo to zero the platform, which was done by leveling the base and varying the signal sent to each servo until the central platform was level. The `CalibratedServo` class was created to incorporate the scales and offsets via a linear mapping for each servo to allow them to rotate the correct amount from the zero position based on pitch, roll, and yaw angles calculated from the MPU output.

## Potential Improvements

The performance of this three-axis gimbal stabilizer can be improved by incorporating a feedback loop. This would require a second MPU to be placed on the central platform, allowing the error between the orientation of the platform and the level orientation to be measured. This error would be used to further correct the orientation, allowing better tracking.
