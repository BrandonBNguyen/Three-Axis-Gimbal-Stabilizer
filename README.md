# Three Axis Gimbal Stabilizer

## Introduction

The goal of this project was to design and develop a gimbal system capable of stabilizing a platform based on input from an accelerometer/magnetometer sensor and to demonstrate knowledge of aerospace concepts (namely aircraft principal axes and how to calculate their orientations from accelerometer readings) and ability to apply them in a hands-on project. In aerospace, a gimbal stabilizer can be used to control the orientation of any platform or instrument where the orientation must be independent of the orientation of the aircraft, such as in a camera or targeting system.

## Theory

The orientation of an aircraft can be expressed as a series of rotations required to achieve that orientation starting from an initial orientation aligned with the local coordinate system, where the nose, right wing, and underside of the plane are aligned with north, east, and down, respectively, at some location on the Earth. Euler angles are the angles used to express the sequence of rotations about the principal axes of the aircraft to achieve the desired orientation from the described starting orientation. One such sequence of rotations commonly used in aerospace is yaw, pitch, and roll, where the desired orientation is described by the angle with which to yaw, then pitch, then roll the aircraft from the starting orientation aligned with the local coordinate system.

![Axes body rotations as shown for a plane](https://www.grc.nasa.gov/www/k-12/airplane/Images/rotations.gif)

