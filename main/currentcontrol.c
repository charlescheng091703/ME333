#include "currentcontrol.h"
#include "utilities.h"
#include "ina219.h"

int duty_cycle;
volatile float current_kp = 0.0;
volatile float current_ki = 0.0;
volatile float desired_torque = 0.0;

void __ISR(_TIMER_2_VECTOR, IPL4SOFT) Timer2ISR(void) {
    static int count = 0;
    static float desired_ma = 200.0;
    static float eint = 0;

    switch (get_mode()) {
        case IDLE:
        {
            OC1RS = 0; // duty cycle = OC1RS/(PR3+1) = 25%
            LATDbits.LATD1 = 0;
            break;
        }
        case PWM:
        {
            int p = get_pwm();
            if (p > 0) { 
              OC1RS = p*20; // duty cycle = OC1RS/(PR3+1) = 25%
              LATDbits.LATD1 = 0;
            }
            else {
              OC1RS = -p*20; // duty cycle = OC1RS/(PR3+1) = 25%
              LATDbits.LATD1 = 1;
            }
            break;
        }
        case ITEST:
        {
          if (count == 25) {
            desired_ma = -200.0;
          }
          else if (count == 50) {
            desired_ma = 200.0;
          }
          else if (count == 75) {
            desired_ma = -200.0;
          }

          float ma = INA219_read_current();
          float error = desired_ma - ma;
          eint += error;
          if (eint > 2000) {
            eint = 2000;
          }
          else if (eint < -2000) {
            eint = -2000;
          }
          float u = current_kp*error + current_ki*eint;

          if (u > 100.0) {
            u = 100.0;
          }
          else if (u < -100.0) {
            u = -100.0;
          }

          if (u > 0) { 
            OC1RS = u*20;
            LATDbits.LATD1 = 0;
          }
          else {
            OC1RS = -u*20; 
            LATDbits.LATD1 = 1;
          }

          d_ma[count] = desired_ma;
          a_ma[count] = ma;

          count ++;
          if (count == 100) {
            count = 0;
            desired_ma = 200.0;
            eint = 0;
            set_mode(IDLE);
          }
          break;
        }
        case TRACK:
        {
          float ma = INA219_read_current();
          float error = desired_torque - ma;
          eint += error;
          if (eint > 2000) {
            eint = 2000;
          }
          else if (eint < -2000) {
            eint = -2000;
          }
          float u = current_kp*error + current_ki*eint;

          if (u > 100.0) {
            u = 100.0;
          }
          else if (u < -100.0) {
            u = -100.0;
          }

          if (u > 0) { 
            OC1RS = u*20;
            LATDbits.LATD1 = 0;
          }
          else {
            OC1RS = -u*20; 
            LATDbits.LATD1 = 1;
          }
          break;
        }
    }
    IFS0bits.T2IF = 0;            // clear interrupt flag
}

void __ISR(_TIMER_3_VECTOR, IPL4SOFT) Timer3ISR(void) {
    IFS0bits.T3IF = 0;            // clear interrupt flag
}

int get_pwm(void) {
  return duty_cycle; 
}

void set_pwm(int n) {
  duty_cycle = n; 
}

float get_current_ki() {
  return current_ki;
}

float get_current_kp() {
  return current_kp;
}

void set_current_gains(float new_kp, float new_ki) {
  current_kp = new_kp;
  current_ki = new_ki;
}

void set_desired_torque(float u) {
  desired_torque = u;
}

void currentcontroller_init(void) {
  T3CONbits.TCKPS = 1;          // Timer3 prescaler N=2 (1:4)
  PR3 = 1999;                   // period = (PR3+1) * N * 12.5 ns = 50 us, 20 kHz
  T3CONbits.TGATE = 0;          // not gated input (the default)
  IPC3bits.T3IP = 4;            // step 4: interrupt priority 4
  IPC3bits.T3IS = 0;            // step 4: interrupt priority 0
  IFS0bits.T3IF = 0;            // step 5: clear the int flag
  IEC0bits.T3IE = 1;            // step 6: enable T3 
  TMR3 = 0;                     // initial TMR3 count is 0

  OC1CONbits.OCM = 0b110;   // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OC32 = 0;      // set to use 16bit timerrs 
  OC1RS = 0;            
  OC1R = 0;              
  OC1CONbits.OCTSEL = 1;    // use Timer3 for OC1

  TRISDbits.TRISD1 = 0; // dir pin D1
  LATDbits.LATD1 = 0;

  T2CONbits.TGATE = 0;           
  T2CONbits.TCKPS = 2;         // Timer2 prescaler N=4 (1:4)      
  PR2 = 3999;                  // set PR so timer rolls over at 5kHz
  IPC2bits.T2IP = 4;           // step 4: interrupt priority 4
  IPC2bits.T2IS = 0;           // step 4: interrupt priority 0
  IFS0bits.T2IF = 0;           // step 5: clear the int flag
  IEC0bits.T2IE = 1;           // step 6: enable T2 by setting IEC0<8>
  TMR2 = 0;                    // initialize count to 0
}
