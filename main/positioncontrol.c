#include "positioncontrol.h"
#include "utilities.h"
#include "encoder.h"
#include "currentcontrol.h"

volatile float position_kp = 0.0;
volatile float position_ki = 0.0;
volatile float position_kd = 0.0;
volatile float desired_angle = 0.0;
volatile int d_len = 1;

void __ISR(_TIMER_4_VECTOR, IPL4SOFT) Timer4ISR(void) {

    IFS0bits.T4IF = 0;            // clear interrupt flag
    static int count = 0;
    static float eint = 0;
    static float eprev = 0;

    switch(get_mode()) {
        case TRACK: 
        {
            WriteUART2("a");
            while(!get_encoder_flag()) {}
            set_encoder_flag(0);
            int pos = get_encoder_count();
            float actual_angle = pos*360.0/(4*334.0);
            a_pos[count] = actual_angle;
            float error = d_pos[count] - actual_angle;
            eint += error;
            if (eint > 1000) {
                eint = 1000;
            }
            else if (eint < -1000) {
                eint = -1000;
            }
            float edot = error - eprev;
            eprev = error;
            float u = position_kp*error + position_ki*eint + position_kd*edot;

            set_desired_torque(u);

            count++;
            if (count == d_len) {
                count = 0;
                eint = 0;
                eprev = 0;
                set_mode(IDLE);
            }
            break;
        }
    }
}

float get_position_kp() {
  return position_kp;
}

float get_position_ki() {
  return position_ki;
}

float get_position_kd() {
  return position_kd;
}

void set_desired_angle(int index, float angle) {
    if (index < 2000) {
        d_pos[index] = angle;
    }
}

void set_desired_len(int len) {
    d_len = len;
}

int get_desired_len() {
    return d_len;
}

void set_position_gains(float new_kp, float new_ki, float new_kd) {
  position_kp = new_kp;
  position_ki = new_ki;
  position_kd = new_kd;
}

void positioncontroller_init(void) {         
  T4CONbits.TCKPS = 3;         // Timer4 prescaler N=8 (1:4)      
  PR4 = 49999;                  // set PR so timer rolls over at 200 Hz
  IPC4bits.T4IP = 4;           // step 4: interrupt priority 5
  IPC4bits.T4IS = 0;           // step 4: interrupt priority 0
  IFS0bits.T4IF = 0;           // step 5: clear the int flag
  IEC0bits.T4IE = 1;           // step 6: enable T4 by setting IEC0<8>
  TMR4 = 0;                    // initialize count to 0
}
