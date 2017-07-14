#include "patternthread.h"
#include <QTextStream>
#include <QDateTime>
#include <qdebug.h>
#include "global_service.h"

//GLOBAL 0x3
#define CAT_GARBAGE_0 30
#define CAT_GARBAGE_1 31

//GLOBAL 0x5
#define CAT_WASH_BIN_0 40
#define CAT_WASH_BIN_1 41
#define CAT_WASH_BIN_2 42
#define CAT_SCRAPE_ADV_0 50
#define CAT_SCRAPE_ADV_1 51
#define CAT_WASH_ADV 60

//GLOBAL 0x1
#define CAT_BIG_BROOM_RIGHT_HIGH 1
#define CAT_BIG_BROOM_RIGHT_LOW 2
#define CAT_BIG_BROOM_LEFT_0 3
#define CAT_BIG_BROOM_LEFT_1 4
#define CAT_SMALL_BROOM_RIGHT_FORWARD 10
#define CAT_SMALL_BROOM_RIGHT_HRIZON 11
#define CAT_PUSH_BIG_BROOM 20

PatternThread::PatternList PatternThread::pattern_list = PatternThread::PatternList();
PatternThread* PatternThread::Available(){
  for(auto iter=pattern_list.begin();iter!=pattern_list.end();++iter){
    PatternThread * t = *iter;
    if(t && t->isRunning()==false){
      return t;
    }
  }
  return NULL;
}

PatternThread::PatternThread(QObject *parent) : QThread(parent){
  pattern_list.push_back(this);//save this to the global list

  global_map_.append(G_SWEEP);
  global_map_.append(G_GARBAGE);
  global_map_.append(G_WASH);

  ptr_feature_all_ = new WaveClassifier(engine_feature_all_);
  ptr_frame_all_ = new FrameClassifier(engine_frame_all_);
}

PatternThread::~PatternThread()
{
  pattern_list.removeAll(this);
}

bool PatternThread::Setup(ConfigParser::ConfigMap &configs){
  if(configs.size()==0)return false;

  if(!configs.contains(FRAME_ALL_NEURONS)||
     !configs.contains(FEATURE_ALL_NEURONS)){
    qDebug()<<tr("[%1,%2]Fail to find the key words in configs.").arg(__FILE__).arg(__LINE__);
    return false;
  }
  if(!configs.contains(ADJUST_SWEEP_SAMPLES)||
     !configs.contains(ADJUST_SWEEP_COUNT) ||
     !configs.contains(ADJUST_GARBAGE_SAMPLES)||
     !configs.contains(ADJUST_GARBAGE_COUNT) ||
     !configs.contains(ADJUST_WASH_SAMPLES)||
     !configs.contains(ADJUST_WASH_COUNT)){
    qDebug()<<tr("[%1,%2]Fail to find the key words in configs.").arg(__FILE__).arg(__LINE__);
    return false;
  }
  if(!configs.contains(THRESHOLD_MIN_SWEEP)||
     !configs.contains(THRESHOLD_MIN_GARBAGE) ||
     !configs.contains(THRESHOLD_MIN_WASH)){
    qDebug()<<tr("[%1,%2]Fail to find the key words in configs.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  if(LoadEngine(configs[FEATURE_ALL_NEURONS],engine_feature_all_)<=0 ||
     LoadEngine(configs[FRAME_ALL_NEURONS],engine_frame_all_)<=0){
    qDebug()<<tr("[%1,%2]Fail to load all neurons").arg(__FILE__).arg(__LINE__);
    return false;
  }

  //parse values
  float adjust_sweep_samples= configs[ADJUST_SWEEP_SAMPLES].toFloat();
  float adjust_garbage_samples= configs[ADJUST_GARBAGE_SAMPLES].toFloat();
  float adjust_wash_samples= configs[ADJUST_WASH_SAMPLES].toFloat();
  adjust_sample_.insert(G_SWEEP,adjust_sweep_samples);
  adjust_sample_.insert(G_GARBAGE,adjust_garbage_samples);
  adjust_sample_.insert(G_WASH,adjust_wash_samples);

  float adjust_sweep_count  = configs[ADJUST_SWEEP_COUNT].toFloat();
  float adjust_garbage_count  = configs[ADJUST_GARBAGE_COUNT].toFloat();
  float adjust_wash_count  = configs[ADJUST_WASH_COUNT].toFloat();
  adjust_count_.insert(G_SWEEP,adjust_sweep_count);
  adjust_count_.insert(G_GARBAGE,adjust_garbage_count);
  adjust_count_.insert(G_WASH,adjust_wash_count);

  int min_sweep = configs[THRESHOLD_MIN_SWEEP].toInt();
  int min_garbage = configs[THRESHOLD_MIN_GARBAGE].toInt();
  int min_wash = configs[THRESHOLD_MIN_WASH].toInt();
  threshold_.insert(G_SWEEP,min_sweep);
  threshold_.insert(G_GARBAGE,min_garbage);
  threshold_.insert(G_WASH,min_wash);
  return true;
}

void PatternThread::StartTask(QString& sign, qint64 start, qint64 end, QByteArray& raw_data){
  signature_ = sign;
  raw_data_ = raw_data;
  start_time_ = start;
  end_time_ = end;
  this->start();
}

void PatternThread::run(){
  QTime task_time;
  qint64 task_start_time = QDateTime::currentSecsSinceEpoch();
  task_time.start();//start the time counter

  if(raw_data_.size()==0)return;
  //check neurons
  if(engine_frame_all_.NeuronCount()==0){
    qDebug()<<tr("[%1,%2] no neuron in engine_frame_all_").arg(__FILE__).arg(__LINE__);
    return;
  }
  if(engine_feature_all_.NeuronCount()==0){
    qDebug()<<tr("[%1,%2] no neuron in engine_feature_all_").arg(__FILE__).arg(__LINE__);
    return;
  }

  //step1. initialize stream
  QTextStream raw_stream(&raw_data_);
  const int row_size = 3;//only 3 axis are required here
  float row_buffer[row_size];
  int total_samples=0;

  QMap<int, int> cat_frame_samples;//G_TYPE, sample number
  QMap<int, int> cat_frame_count;//G_TYPE, count
  QMap<int, int> cat_feature_samples;//G_TYPE, sample number
  QMap<int, int> cat_feature_count;//G_TYPE, count

  //step2. parse the data
  while(!raw_stream.atEnd()){
    //step1 parse source data
    QString str_line = raw_stream.readLine().remove(' ');
    QStringList str_val_list = str_line.replace('\"',"").split(',');
    if(str_val_list.size()<row_size)continue;
    //prepare row data
    for(int i=0;i<row_size;++i){
      row_buffer[i] =str_val_list[i].toFloat();
    }

    //step1 push to sweep engine
    //step1. feature engine parser
    WaveClassifier::ResultList wave_result_list;
    if(ptr_feature_all_->PushToClassify(row_buffer,row_size,wave_result_list)){
      for(int k=0;k<wave_result_list.size();++k){
        WaveClassifier::result_cat_t r = wave_result_list[k];
        int cat_index = r.cat/100-1;
        int g_type = global_map_[cat_index];
        if(r.cat>0){
          cat_feature_samples[g_type]=cat_feature_samples[g_type]+r.frame_len;
          cat_feature_count[g_type] = cat_feature_count[g_type]+1;
        }
      }
    }

    //step2.frame engine parse
    if(ptr_frame_all_->PushToClassify(row_buffer,row_size)>0){
      const FrameClassifier::sample_t & last_sample = ptr_frame_all_->LastClassified();
      int cat_index = last_sample.cat/100-1;
      int g_type = global_map_[cat_index];
      cat_frame_samples[g_type] = cat_frame_samples[g_type]+last_sample.frame_len;
      cat_frame_count[g_type] = cat_frame_count[g_type]+1;
    }

    ++total_samples;
  }

  qDebug()<<endl<<"---------------------------------------------------------";
  QDateTime start_t = QDateTime::fromSecsSinceEpoch(start_time_);
  QDateTime end_t = QDateTime::fromSecsSinceEpoch(end_time_);
  qDebug()<<tr("Signature:%1. From: %2 to %3").arg(signature_).arg(start_t.toString()).arg(end_t.toString());

  //step FINAL convert data into report
  float freq = ComputeFrequency(total_samples,start_time_,end_time_);
  if(freq<=0){
    freq = DEFAULT_FREQUENCY;
  }
  qDebug()<<tr("[%1,%2]total samples are:%3, current frequency is %4")
            .arg(__FILE__).arg(__LINE__)
            .arg(total_samples)
            .arg(freq);

  do{
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

    if(cat_feature_count.size() || cat_frame_count.size()){
      ReportMap report;
      AnalyzeCount(cat_frame_count,cat_frame_samples,
                   cat_feature_count,cat_feature_samples,
                   report);
      QList<int> keys = report.keys();
      for(int i=0;i<keys.size();++i){
        int key = keys[i];
        report_item_t  t = report[key];
        report_stream<<t.type<<","<<t.count<<","<<(float)(t.samples)/freq<<endl;
        qDebug()<<tr("[%1] key:%2, count:%3, samples:%4")
                  .arg(i)
                  .arg(t.type)
                  .arg(t.count)
                  .arg(t.samples);
      }
    }
    //last step
    emit reportCsvReady(str_report);
  }while(0);
}

float PatternThread::ComputeFrequency(int samples, qint64 start, qint64 end){
  qint64 duration = end-start;
  if(duration==0)return 0.0f;
  return (static_cast<float>(samples)/static_cast<float>(duration));
}

void PatternThread::PrintResultCatMap(QMap<int, int> &map)
{
  QList<int> keys = map.keys();
  if(keys.size())
    qDebug()<<tr("[%1,%2]Print valus of map:").arg(__FILE__).arg(__LINE__);

  for(int i=0;i<map.size();++i){
    int key = keys[i];
    int value = map[key];
    qDebug()<<tr("[%1]key:%2  value:%3").arg(i).arg(key).arg(value);
  }
}

void PatternThread::AnalyzeCount(const QMap<int, int> &frame_count,
                                 const QMap<int, int> &frame_samples,
                                 const QMap<int, int> &feature_count,
                                 const QMap<int, int> &feature_samples,
                                 PatternThread::ReportMap &out_report)
{
  out_report.clear();
  for(int i=0;i<global_map_.size();++i){
    int g_type = global_map_[i];
    if(g_type==0)continue;

    //get values
    int count_frame = frame_count[g_type];
    int count_feature = feature_count[g_type];
    int samples_frame = frame_samples[g_type];
    int samples_feature = feature_samples[g_type];

    //process
    int mid_count = (count_frame+count_feature)/2;
    int max_count = (count_frame>count_feature)?count_frame:count_feature;
    int max_samples = (samples_frame>samples_feature)?samples_frame:samples_feature;
    qDebug()<<tr("[%1,%2] type:%3, count: %4, samples:%5").arg(__FILE__).arg(__LINE__).arg(g_type).arg(max_count).arg(max_samples);
    qDebug()<<tr("[%1,%2] c_frame:%3,c_feature:%4,s_frame:%5,s_feature:%6")
              .arg(__FILE__).arg(__LINE__)
              .arg(count_frame).arg(count_feature).arg(samples_frame).arg(samples_feature);

    if(mid_count<threshold_[g_type]){
      continue;
    }

    if(max_samples<=0){
      continue;
    }

    //insert item
    report_item_t item;
    item.type=g_type;
    item.samples=max_samples*adjust_sample_[g_type];
    item.count = max_count*adjust_count_[g_type];
     out_report.insert(g_type,item);
  }
}

int PatternThread::LoadEngine(const QString& src_path,NeuronEngineFloat & engine){
  if(src_path.isEmpty())return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<tr("[%1,%2]Fail to open file %3").arg(__FILE__).arg(__LINE__).arg(src_path);
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
