#ifndef UTILITIES_H
#define UTILITIES_H

#define IDLE 1
#define PWM 2
#define ITEST 3
#define HOLD 4
#define TRACK 5

int get_mode(void);
void set_mode(int mode);

#endif // UTILITIES_H

