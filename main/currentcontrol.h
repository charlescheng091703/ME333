#ifndef CURRENTCONTROL_H
#define CURRENTCONTROL_H

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro

#include "NU32.h"

void currentcontroller_init(void);
void set_pwm(int n); 
int get_pwm(void); 
float get_current_ki();
float get_current_kp();
void set_current_gains(float, float);
void set_desired_torque(float);

volatile float d_ma[100]; // desired
volatile float a_ma[100]; // actual

#endif // CURRENTCONTROL_H
