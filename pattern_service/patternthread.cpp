#include "patternthread.h"
#include <QTextStream>
#include <QDateTime>
#include <qdebug.h>

PatternThread::PatternList PatternThread::pattern_list = PatternThread::PatternList();
PatternThread* PatternThread::Available(){
  for(int i=0; i<pattern_list.size();++i){
    PatternThread * t =  pattern_list.at(i);
    if(t && t->isRunning()==false){
      return t;
    }
  }
  return NULL;
}

PatternThread::PatternThread(QObject *parent) : QThread(parent){
  my_index_ = pattern_list.size();
  pattern_list.push_back(this);//save this to the global list
}

PatternThread::~PatternThread()
{
  pattern_list.removeAll(this);
}

bool PatternThread::Setup(QString &work_dir,ConfigParser::ConfigMap &configs){
  if(configs.size()==0)return false;
  if(!this->multi_pattern_.Setup(work_dir,configs)){
    qDebug()<<tr("[%1,%2]fail to load neurons for multi engine").arg(__FILE__).arg(__LINE__);
    return false;
  }

  return true;
}

void PatternThread::StartTask(QString& sign, qint64 start, qint64 end, QString& raw_csv){
  this->signature_ = sign;
  this->raw_csv_ = raw_csv;
  this->start_time_ = start;
  this->end_time_ = end;
  this->start();
}

void PatternThread::run(){
  //step1. time counter
  QTime task_time;
  qint64 task_start_time = QDateTime::currentSecsSinceEpoch();
  task_time.start();//start the time counter

  //if no data exists
  if(this->raw_csv_.size()==0){
    //qDebug()<<tr("[%1,%2]No available csv data received").arg(__FILE__).arg(__LINE__);
    return;
  }

  //step2. classify the result
  MultiPatternService::ResultMap out_map;
  int total_samples = multi_pattern_.Classify(raw_csv_,out_map);


  //step4. compute the frequency
  qDebug()<<endl<<"---------------------------------------------------------";
  QDateTime start_t = QDateTime::fromSecsSinceEpoch(start_time_);
  QDateTime end_t = QDateTime::fromSecsSinceEpoch(end_time_);
  qDebug()<<tr("Signature:%1. From: %2 to %3").arg(signature_).arg(start_t.toString()).arg(end_t.toString());
  float freq = ComputeFrequency(total_samples,start_time_,end_time_);
  if(freq<=0){
    freq = DEFAULT_FREQUENCY;
  }
  qDebug()<<tr("[%1,%2]total samples are:%3, current frequency is %4 hz")
            .arg(__FILE__).arg(__LINE__)
            .arg(total_samples)
            .arg(freq);

  //step5. report to csv
  QTextStream report_stream;
  QString str_report;
  report_stream.setString(&str_report);
  //task info
  qint64 task_end_time = QDateTime::currentSecsSinceEpoch();
  //first line
  report_stream<<signature_<<","<<start_time_<<","<<end_time_<<","
              <<pack_start<<","<<pack_end<<","
              <<task_start_time<<","<<task_end_time<<","
              <<task_time.elapsed()<<endl;

  //step1. record the result
  QList<int> keys = out_map.keys();
  for(int i=0;i<keys.size();++i){
    int type = keys[i];
    qDebug()<<QString("[%1,%2]Type[%3], peaks:%4, samples:%5, percent:%6")
              .arg(__FILE__).arg(__LINE__)
              .arg(type)
              .arg(out_map[type].peaks)
              .arg(out_map[type].samples)
              .arg(out_map[type].percentile);
    report_stream<<type<<","<<out_map[type].peaks<<","<<(out_map[type].samples/freq)<<endl;
  }

  //last step
  emit reportCsvReady(str_report);
}

float PatternThread::ComputeFrequency(int samples, qint64 start, qint64 end){
  qint64 duration = end-start;
  if(duration==0)return 0.0f;
  return (static_cast<float>(samples)/static_cast<float>(duration));
}
