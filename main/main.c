#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include <stdio.h>
#include "encoder.h"
#include "utilities.h"
#include "ina219.h"
#include "currentcontrol.h"
#include "positioncontrol.h"
// include other header files here

#define BUF_SIZE 200

const int m = 0;  // slope of best fit line relating ADC count to motor current 
const int b = 0;  // intercept of best fit line relating ADC count to motor current 

int main() 
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;        
  __builtin_disable_interrupts();
  UART2_Startup();
  INA219_Startup();
  currentcontroller_init();
  positioncontroller_init();
  __builtin_enable_interrupts();
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1
  T2CONbits.ON = 1;        // turn Timer2 on 
  T4CONbits.ON = 1;        // turn Timer4 on 

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a':
      {
        float ma = INA219_read_current();
        sprintf(buffer,"%f\r\n", ma); 
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c':
      {
        WriteUART2("a");
        while(!get_encoder_flag()) {}
        set_encoder_flag(0);
        int pos = get_encoder_count();
        sprintf(buffer, "%d\r\n", pos);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'd':
      {
        WriteUART2("a");
        while(!get_encoder_flag()) {}
        set_encoder_flag(0);
        int pos = get_encoder_count();
        sprintf(buffer, "%d\r\n", (int) (pos*360.0/(4*334.0)));
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e':
      {
        WriteUART2("b");
        WriteUART2("a");
        while(!get_encoder_flag()) {}
        set_encoder_flag(0);
        int pos = get_encoder_count();
        sprintf(buffer, "%d\r\n", pos);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'f':
      {
        int n = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &n);
        set_pwm(n);
        set_mode(PWM);
        sprintf(buffer,"%d\r\n", get_pwm()); 
        NU32_WriteUART3(buffer);
        break;
      }
      case 'g':
      {
        float n1 = 0, n2 = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f", &n1, &n2);
        set_current_gains(n1, n2);
        sprintf(buffer,"\r\n"); 
        NU32_WriteUART3(buffer);
        break;
      }
      case 'h':
      {
        sprintf(buffer,"%f %f\r\n",get_current_kp(),get_current_ki()); 
        NU32_WriteUART3(buffer);
        break;
      }
      case 'i':
      {
        float n1 = 0, n2 = 0, n3 = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f %f %f", &n1, &n2, &n3);
        set_position_gains(n1, n2, n3);
        sprintf(buffer,"\r\n"); 
        NU32_WriteUART3(buffer);
        break;
      }
      case 'j':
      {
        sprintf(buffer,"%f %f %f\r\n",get_position_kp(),get_position_ki(),get_position_kd()); 
        NU32_WriteUART3(buffer);
        break;
      }
      case 'k':
      {
        set_mode(ITEST);
        while(get_mode()==ITEST){}
        sprintf(buffer,"100\r\n");
        NU32_WriteUART3(buffer);
        for (int i = 0; i < 100; i++) {
          sprintf(buffer,"%f %f\r\n", d_ma[i], a_ma[i]);
          NU32_WriteUART3(buffer);
        }
        break;
      }
      case 'm':
      {
        int len = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &len);
        set_desired_len(len);
        int i = 0;
        float pos = 0;
        for (i = 0; i < len; i++) {
          NU32_ReadUART3(buffer,BUF_SIZE);
          sscanf(buffer, "%f", &pos);
          set_desired_angle(i,pos);
        }
        break;
      }
      case 'n':
      {
        int len = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &len);
        set_desired_len(len);
        int i = 0;
        float pos = 0;
        for (i = 0; i < len; i++) {
          NU32_ReadUART3(buffer,BUF_SIZE);
          sscanf(buffer, "%f", &pos);
          set_desired_angle(i,pos);
        }
        break;
      }
      case 'o':
      {
        set_mode(TRACK);
        while(get_mode()==TRACK){}
        sprintf(buffer,"%d\r\n",get_desired_len());
        NU32_WriteUART3(buffer);
        for (int i = 0; i < get_desired_len(); i++) {
          sprintf(buffer,"%f %f\r\n",d_pos[i], a_pos[i]);
          NU32_WriteUART3(buffer);
        }
        break;
      }
      case 'p':
      {
        set_mode(IDLE);
        sprintf(buffer, "%d\r\n", get_mode());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'q':
      {
        // handle q for quit. Later you may want to return to IDLE mode here. 
        break;
      }
      case 'r':
      {
        //sprintf(buffer, "%d\r\n", get_desired_len());
        sprintf(buffer, "%d\r\n", get_mode());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'x':
      {
        int n1 = 0, n2 = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d %d", &n1, &n2);
        sprintf(buffer,"%d\r\n", n1 + n2); 
        NU32_WriteUART3(buffer);
        break;
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}

