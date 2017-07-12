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

  global_map_.insert(ENGINE_SWEEP,G_SWEEP);
  global_map_.insert(ENGINE_GARBAGE,G_GARBAGE);
  global_map_.insert(ENGINE_WASH,G_WASH);

  ptr_feature_all_ = new WaveClassifier(engine_feature_all_);
  ptr_frame_all_ = new FrameClassifier(engine_frame_all_);
}

PatternThread::~PatternThread()
{
  pattern_list.removeAll(this);
}
bool PatternThread::Setup(ConfigParser::ConfigMap &configs){
  if(configs.size()==0)return false;
  if(!configs.contains(FEATURE_ALL_NEURONS)||
     !configs.contains(FEATURE_ALL_NEURONS)){
    qDebug()<<tr("[%1,%2]Fail to find the key words in configs.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  if(LoadEngine(configs[FEATURE_ALL_NEURONS],engine_feature_all_)<=0 ||
     LoadEngine(configs[FRAME_ALL_NEURONS],engine_frame_all_)<=0){
    qDebug()<<tr("[%1,%2]Fail to load all neurons").arg(__FILE__).arg(__LINE__);
    return false;
  }
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

  if(cat_feature_count.size() || cat_frame_count.size()){
    QTextStream report_stream;
    QString str_report;
    report_stream.setString(&str_report);
    //first line
    report_stream<<signature_<<","<<start_time_<<","<<end_time_<<endl;

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

    //last step
    if(report.size()>0) emit reportCsvReady(str_report);
  }
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
  int min_count_array[]={MIN_SWEEP_COUNT,MIN_GARBAGE_COUNT,MIN_WASH_COUNT};
  for(int i=0;i<global_map_.size();++i){
    int g_type = global_map_[i];
    if(g_type==0)continue;

    //get values
    int count_frame = frame_count[g_type];
    int count_feature = feature_count[g_type];
    int samples_frame = frame_samples[g_type];
    int samples_feature = feature_samples[g_type];

    //process
    int count = 0;
    count = (count_frame>count>count_feature)?count_feature:count_frame;
    if(count<=min_count_array[i])continue;
    //count = (int)(0.2*count_frame+0.8*count_feature);
    count = (count_frame>count_feature)?count_frame:count_feature;
    int samples = (int)(0.4*samples_frame+0.6*samples_feature);
    if(samples<=0)continue;

    //insert item
    report_item_t item;
    item.type=g_type;
    item.samples=samples;
    item.count = count;
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
  //qDebug()<<tr("[%1,%2]Loading neurons from %3").arg(__FILE__).arg(__LINE__).arg(src_path);
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
