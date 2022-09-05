//
// Created by wfy on 7/12/22.
//

#ifndef CYBERDOG_SDK_LEG_CONTROL_H
#define CYBERDOG_SDK_LEG_CONTROL_H

#endif //CYBERDOG_SDK_LEG_CONTROL_H
/*
	legcontrol.h - Library for controlling a stompy leg.
	Created by Joel Greenwood, Febuary 26, 2017.
	Released into the public domain -- so help you God.
*/

#ifndef legControl_h
#define legControl_h

#include "Arduino.h"

class legControl
{
public:
    legControl();
    int angleToSensor(int joint, float angle);
    float sensorToAngle(int joint, float sensorReading);
    void sensors_to_xyz(float sensors[], float xyz[]);
    void anglesToXYZ(float anglesRad[], float xyz[]);
    float angleToRad(float angle);
    void anglesRadToXYZ(float angles[], float xyz[]);
    void xyzToAngles(float xyz[], float angles[]);
    void xyzToSensors(float xyz[], float sensorGoals[]);
    void goal_XYZ_toSensorVelocities(float startXYZ[], float finalXYZ[], float sensorVelocities[], float time);
};

#endif