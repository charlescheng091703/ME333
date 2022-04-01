#ifndef POSITIONCONTROL_H
#define POSITIONCONTROL_H

#include <xc.h> // processor SFR definitions
#include <sys/attribs.h> // __ISR macro
#include "NU32.h"

void positioncontroller_init(void);
float get_position_ki();
float get_position_kp();
float get_position_kd();
void set_desired_angle(int, float);
void set_position_gains(float, float, float);
void set_desired_len(int);
int get_desired_len();

volatile float d_pos[2000]; // desired
volatile float a_pos[2000]; // actual

#endif // POSITIONCONTROL_H
