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
  G_PICKUP=0x2,
  G_GARBAGE=0x3,
  G_SCRUB=0x4,
  G_ADV_CLEAR=0x5
};

typedef struct gesture_result_s{
  int type;
  int count;
  uint32_t time;
}gesture_result_t;

typedef std::map<const char*,gesture_result_t> GestureMap;
typedef std::vector<gesture_result_t> GestureList;

extern ConfigParser::ConfigMap g_configs;
int LoadEngine(const QString& src_path,NeuronEngineFloat & engine){
  if(src_path.isEmpty())return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open file:"<<src_path;
    return 0;
  }
  QTextStream src_text(&src_file);

  //step1.reset engine
  engine.ResetEngine();

  engine.BeginRestoreMode();
  while(!src_text.atEnd()){
    QString str_record = src_text.readLine();
    QStringList str_list = str_record.split(',');
    if(str_list.size()<3)continue;

    //read cat,min_aif, aif
    int cat =  str_list[0].toInt();
    int min_aif = str_list[1].toFloat();
    int aif = str_list[2].toFloat();

    //read buffer
    const int neuron_size=1024;
    float neuron_buffer[neuron_size];
    int neuron_len=0;
    for(int i=0;i<str_list.size()-3;++i){
      neuron_buffer[i]=str_list[3+i].toFloat();
      ++neuron_len;
    }

    //restore
    engine.RestoreNeuron(neuron_buffer,neuron_len,cat,aif,min_aif);
  }
  engine.EndRestoreMode();
  return engine.NeuronCount();
}
#endif // GLOBAL_SERVICE_H
