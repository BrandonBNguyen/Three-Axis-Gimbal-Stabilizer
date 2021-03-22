# Three Axis Gimbal Stabilizer

## Introduction

The goal of this project was to design and develop a gimbal system capable of stabilizing a platform based on input from an accelerometer/magnetometer sensor and to demonstrate knowledge of aerospace concepts (namely aircraft principal axes and how to calculate their orientations from accelerometer readings) and ability to apply them in a hands-on project. In aerospace, a gimbal stabilizer can be used to control the orientation of any platform or instrument where the orientation must be independent of the orientation of the aircraft, such as in a camera or targeting system.

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
