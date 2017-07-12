#ifndef GLOBAL_SERVICE_H
#define GLOBAL_SERVICE_H
#include <map>
#include <vector>
#include <QFile>
#include <QTextStream>

#include "qrineuronlib.h"
#include "configparser.h"
enum GESTURE_TYPE{
  G_SWEEP=0x1,
  G_GARBAGE=0x3,
  G_WASH=0x5
};

typedef struct gesture_result_s{
  int type;
  int count;
  uint32_t time;
}gesture_result_t;

typedef std::map<const char*,gesture_result_t> GestureMap;
typedef std::vector<gesture_result_t> GestureList;
#endif // GLOBAL_SERVICE_H
