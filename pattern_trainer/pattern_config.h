#ifndef PATTERN_TRAINER_CONFIG_H
#define PATTERN_TRAINER_CONFIG_H

//sweep pattern wave size
#define SWEEP_MIN_WAVE 10
#define SWEEP_MAX_WAVE 100

//garbage pattern wave size
#define GARBAGE_MIN_WAVE 15
#define GARBAGE_MAX_WAVE 100

//wash pattern wave size
#define WASH_MIN_WAVE 5
#define WASH_MAX_WAVE 80

//nagtive pattern wave size
#define NEGATIVE_MIN_WAVE 2
#define NEGATIVE_MAX_WAVE 80

enum PATTERN_TYPE{
  P_SWEEP=0x1,
  P_GARBAGE=0x3,
  P_WASH=0x4,

  P_NEG=0x0A
};
#endif // PATTERN_TRAINER_CONFIG_H
