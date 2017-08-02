#include "multipatternservice.h"
#include <QTextStream>
#include <QStringList>
#include <QDebug>


using namespace qri_neuron_lib;
using namespace std;
using namespace feature_engine;

MultiPatternService::MultiPatternService(QObject* parent)
  :QObject(parent),
   feature_basic_({FrameFeatureExtractor::RAW_ROWS,FrameFeatureExtractor::RAW_COLS,FrameFeatureExtractor::RAW_DELTA},
                  {FrameFeatureExtractor::FEATURE_GAP_MIN,FrameFeatureExtractor::FEATURE_GAP_MAX})
{
  //init engine
  NeuronEngineFloat* sweep_engine = new NeuronEngineFloat;
  NeuronEngineFloat* garbage_engine = new NeuronEngineFloat;
  NeuronEngineFloat* wash_engine = new NeuronEngineFloat;
  NeuronEngineFloat* neg_engine = new NeuronEngineFloat;
  engine_map_.insert(P_SWEEP,sweep_engine);
  engine_map_.insert(P_GARBAGE,garbage_engine);
  engine_map_.insert(P_WASH,wash_engine);
  engine_map_.insert(P_NEG,neg_engine);

  //init frame feature calssifier
  FrameFeatureClassifier* sweep_classifier= new FrameFeatureClassifier({SWEEP_MIN_WAVE,SWEEP_MAX_WAVE},*sweep_engine);
  FrameFeatureClassifier* garbage_classifier= new FrameFeatureClassifier({GARBAGE_MIN_WAVE,GARBAGE_MAX_WAVE},*garbage_engine);
  FrameFeatureClassifier* wash_classifier= new FrameFeatureClassifier({WASH_MIN_WAVE,WASH_MAX_WAVE},*wash_engine);
  FrameFeatureClassifier* neg_classifier= new FrameFeatureClassifier({NEGATIVE_MIN_WAVE,NEGATIVE_MAX_WAVE},*neg_engine);
  classifier_map_.insert(P_SWEEP,sweep_classifier);
  classifier_map_.insert(P_GARBAGE,garbage_classifier);
  classifier_map_.insert(P_WASH,wash_classifier);
  classifier_map_.insert(P_NEG,neg_classifier);

  //start the patternd service
  this->ptr_raw_frame_ = new DataFrame(feature_basic_.Window().rows,feature_basic_.Window().cols);
  this->adjust_percentile_times_ = ADJUST_PERCETILE;

  //init the threshold
  this->threshold_percent_.insert(P_SWEEP, 0.08f);
  this->threshold_percent_.insert(P_GARBAGE,0.08f);
  this->threshold_percent_.insert(P_WASH,0.08f);

  //init the threshold peaks
  this->threshold_peaks_.insert(P_SWEEP, 1.0f);
  this->threshold_peaks_.insert(P_GARBAGE,1.0f);
  this->threshold_peaks_.insert(P_WASH,1.0f);


  //init adjust peaks
  this->adjust_peaks_.insert(P_SWEEP, 1.0f);
  this->adjust_peaks_.insert(P_GARBAGE,1.0f);
  this->adjust_peaks_.insert(P_WASH,1.0f);

  //init adjust samples
  this->adjust_samples_.insert(P_SWEEP, 1.0f);
  this->adjust_samples_.insert(P_GARBAGE,1.0f);
  this->adjust_samples_.insert(P_WASH,1.0f);
}

MultiPatternService::~MultiPatternService()
{
  if(this->ptr_raw_frame_)delete this->ptr_raw_frame_;
}

bool MultiPatternService::Setup(const QString& work_dir, const ConfigParser::ConfigMap &configs)
{
  //step1 load engines
  if(!configs.contains(CONFIG_NEURON_SWEEP) ||
     !configs.contains(CONFIG_NEURON_GARBAGE) ||
     !configs.contains(CONFIG_NEURON_WASH) ||
     !configs.contains(CONFIG_NEURON_NEG)){
    qDebug()<<tr("[%1,%2] neuron keywords not exist.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  QString path_sweep;
  path_sweep.append(work_dir).append(configs[CONFIG_NEURON_SWEEP]);
  QString path_garbage;
  path_garbage.append(work_dir).append(configs[CONFIG_NEURON_GARBAGE]);
  QString path_wash;
  path_wash.append(work_dir).append(configs[CONFIG_NEURON_WASH]);
  QString path_neg;
  path_neg.append(work_dir).append(configs[CONFIG_NEURON_NEG]);

  if(0==LoadEngine(path_sweep,*engine_map_[P_SWEEP])||
     0==LoadEngine(path_garbage,*engine_map_[P_GARBAGE])||
     0==LoadEngine(path_wash,*engine_map_[P_WASH]) ||
     0==LoadEngine(path_neg,*engine_map_[P_NEG])){
    qDebug()<<tr("[%1,%2] fail to load neurons from local files.").arg(__FILE__).arg(__LINE__);
    return false;
  }

  if(configs.contains(CONFIG_FORMULA_PERCENTILE)){
    this->adjust_percentile_times_ = configs[CONFIG_FORMULA_PERCENTILE].toFloat();
    qDebug()<<tr("[%1,%2] percentile_times:%3").arg(__FILE__).arg(__LINE__).arg(adjust_percentile_times_);
  }

  //Threshold percent
  if(configs.contains(CONFIG_THRESHOLD_MIN_SWEEP_PERCENT)){
    this->threshold_percent_[P_SWEEP] = configs[CONFIG_THRESHOLD_MIN_SWEEP_PERCENT].toFloat();
    qDebug()<<tr("[%1,%2] sweep min threshold:%3").arg(__FILE__).arg(__LINE__).arg(this->threshold_percent_[P_SWEEP] );
  }
  if(configs.contains(CONFIG_THRESHOLD_MIN_GARBAGE_PERCENT)){
    this->threshold_percent_[P_GARBAGE] = configs[CONFIG_THRESHOLD_MIN_GARBAGE_PERCENT].toFloat();
    qDebug()<<tr("[%1,%2] garbage min threshold:%3").arg(__FILE__).arg(__LINE__).arg(this->threshold_percent_[P_GARBAGE] );
  }
  if(configs.contains(CONFIG_THRESHOLD_MIN_WASH_PERCENT)){
    this->threshold_percent_[P_WASH] = configs[CONFIG_THRESHOLD_MIN_WASH_PERCENT].toFloat();
    qDebug()<<tr("[%1,%2] wash min threshold:%3").arg(__FILE__).arg(__LINE__).arg(this->threshold_percent_[P_WASH] );
  }
  //threshold peaks
  if(configs.contains(CONFIG_THRESHOLD_MIN_SWEEP_PEAKS)){
    this->threshold_peaks_[P_SWEEP] = configs[CONFIG_THRESHOLD_MIN_SWEEP_PEAKS].toFloat();
    qDebug()<<tr("[%1,%2] sweep min threshold peaks:%3").arg(__FILE__).arg(__LINE__).arg(this->threshold_peaks_[P_SWEEP] );
  }
  if(configs.contains(CONFIG_THRESHOLD_MIN_GARBAGE_PEAKS)){
    this->threshold_peaks_[P_GARBAGE] = configs[CONFIG_THRESHOLD_MIN_GARBAGE_PEAKS].toFloat();
    qDebug()<<tr("[%1,%2] garbage min threshold peaks:%3").arg(__FILE__).arg(__LINE__).arg(this->threshold_peaks_[P_GARBAGE] );
  }
  if(configs.contains(CONFIG_THRESHOLD_MIN_WASH_PEAKS)){
    this->threshold_peaks_[P_WASH] = configs[CONFIG_THRESHOLD_MIN_WASH_PEAKS].toFloat();
    qDebug()<<tr("[%1,%2] wash min threshold peaks:%3").arg(__FILE__).arg(__LINE__).arg(this->threshold_peaks_[P_WASH] );
  }

  //PEAKS
  if(configs.contains(CONFIG_ADJUST_PEAK_SWEEP)){
    this->adjust_peaks_[P_SWEEP] = configs[CONFIG_ADJUST_PEAK_SWEEP].toFloat();
    qDebug()<<tr("[%1,%2] sweep peak parameter:%3").arg(__FILE__).arg(__LINE__).arg(this->adjust_peaks_[P_SWEEP] );
  }
  if(configs.contains(CONFIG_ADJUST_PEAK_GARBAGE)){
    this->adjust_peaks_[P_GARBAGE] = configs[CONFIG_ADJUST_PEAK_GARBAGE].toFloat();
    qDebug()<<tr("[%1,%2] garbage peak parameter:%3").arg(__FILE__).arg(__LINE__).arg(this->adjust_peaks_[P_GARBAGE] );
  }
  if(configs.contains(CONFIG_ADJUST_PEAK_WASH)){
    this->adjust_peaks_[P_WASH] = configs[CONFIG_ADJUST_PEAK_WASH].toFloat();
    qDebug()<<tr("[%1,%2] wash peak parameter:%3").arg(__FILE__).arg(__LINE__).arg(this->adjust_peaks_[P_WASH] );
  }

  //SAMPLES
  if(configs.contains(CONFIG_ADJUST_SAMPLES_SWEEP)){
    this->adjust_samples_[P_SWEEP] = configs[CONFIG_ADJUST_SAMPLES_SWEEP].toFloat();
    qDebug()<<tr("[%1,%2] sweep samples parameter:%3").arg(__FILE__).arg(__LINE__).arg(this->adjust_samples_[P_SWEEP] );
  }
  if(configs.contains(CONFIG_ADJUST_SAMPLES_GARBAGE)){
    this->adjust_samples_[P_GARBAGE] = configs[CONFIG_ADJUST_SAMPLES_GARBAGE].toFloat();
    qDebug()<<tr("[%1,%2] garbage samples parameter:%3").arg(__FILE__).arg(__LINE__).arg(this->adjust_samples_[P_GARBAGE] );
  }
  if(configs.contains(CONFIG_ADJUST_SAMPLES_WASH)){
    this->adjust_samples_[P_WASH] = configs[CONFIG_ADJUST_SAMPLES_SWEEP].toFloat();
    qDebug()<<tr("[%1,%2] wash samples parameter:%3").arg(__FILE__).arg(__LINE__).arg(this->adjust_samples_[P_WASH] );
  }
  return true;
}

int MultiPatternService::Classify(QString & strcsv, ResultMap &out_map)
{
  QTextStream stream(&strcsv);
  int frame_samples=0;
  int actual_samples=0;
  int accepted_samples=0;
  QVector<multi_pattern_result_t> final_filter_list;

  //qDebug()<<endl<<tr("[%1,%2] strcsv length is:%3").arg(__FILE__).arg(__LINE__).arg(strcsv.size());
  while(!stream.atEnd()){
    //step1. parse input line
    QString str_line = stream.readLine();
    str_line.remove("\"");
    str_line.remove(" ");
    QStringList str_vals = str_line.split(',');
    if(str_vals.size()<this->feature_basic_.Window().cols){
      continue;
    }
    ++frame_samples;
    ++actual_samples;

    //step2.convert to data buffer
    const int data_size = this->feature_basic_.Window().cols;
    float data_buffer[data_size];
    for(int i=0;i<data_size;++i){
      data_buffer[i]=str_vals[i].toFloat();
    }

    //step3.push to dataframe
    if(!this->feature_basic_.CheckDataAccpeted(data_buffer,data_size)){
      continue;
    }
    ++accepted_samples;//accepted samples

    //step4. push to dataframe
    this->ptr_raw_frame_->Push(data_buffer,data_size);
    if(!this->ptr_raw_frame_->Full()){
      continue;
    }

    //step5. this frame is full
    const QList<int> keys = this->classifier_map_.keys();
    QMap<int, FrameFeatureClassifier::result_t> result_map;//engine type, reuslt
    for(int i=0;i<keys.size();++i){
      int type = keys[i];
      FrameFeatureClassifier* ptr_c =  this->classifier_map_[type];
      //classify the result
      FrameFeatureClassifier::ResultList r_list;
      ptr_c->Classify(*ptr_raw_frame_,r_list);
      if(r_list.size()){
        result_map.insert(type, r_list.first());
      }
    }

    //step6.check if there is best result
    if(result_map.size()){
      //step6.1 find the best result
      int   min_type=0;
      float min_dist=NeuronEngineFloat::DEFAULT_NEURON_AIF_MAX;
      for(int i=0;i<keys.size();++i){
        if(result_map.count(keys[i])==0)continue;
        int type = keys[i];
        float dist = this->engine_map_[keys[i]]->ReadNeuron(result_map[type].nid)->Firing()+(1-result_map[type].percentile)*this->adjust_percentile_times_;
        if(min_dist>dist){
          min_dist = dist;
          min_type = type;
        }
#if 0
    qDebug()<<tr("[%1]cat:%2, peaks:%3, percentile:%4, samples:%5, distance:%6")
              .arg(type)
              .arg(result_map[type].cat)
              .arg(result_map[type].peaks)
              .arg(result_map[type].percentile)
              .arg(result_map[type].samples)
              .arg(dist);
#endif
      }

      //step7. sum up the best match type
      multi_pattern_result_t m;
      m.p_type = min_type;
      m.peaks = result_map[min_type].peaks;
      m.samples = result_map[min_type].samples;
      final_filter_list.push_back(m);//save the best match list
      if(out_map.count(min_type)){
        out_map[min_type].peaks = out_map[min_type].peaks + result_map[min_type].peaks;
        out_map[min_type].samples = out_map[min_type].samples + result_map[min_type].samples;
      }else{
         out_map.insert(min_type,m);
      }

  #if 1
      qDebug()<<tr("Best match is %1 cat:%2, peaks:%3, percentile:%4, samples:%5, min_dist:%6")
                .arg(min_type)
                .arg(result_map[min_type].cat)
                .arg(result_map[min_type].peaks)
                .arg(result_map[min_type].percentile)
                .arg(result_map[min_type].samples)
                .arg(min_dist)
              <<endl;
  #endif
    }

    //last step. pop out delta
    this->ptr_raw_frame_->Pop(feature_basic_.Window().delta);
    int frame_delta = feature_basic_.Window().rows-feature_basic_.Window().delta;
    frame_samples += frame_delta;
    accepted_samples += frame_delta;
  }
  //step1. parse the final filter
  if(final_filter_list.size()>2){
    for(int i=1;i<final_filter_list.size()-1;++i){
      multi_pattern_result_t & curr = final_filter_list[i];
      multi_pattern_result_t & left = final_filter_list[i-1];
      multi_pattern_result_t & right = final_filter_list[i+1];
      if(curr.p_type!=left.p_type && curr.p_type!=right.p_type){
         out_map[curr.p_type].peaks = out_map[curr.p_type].peaks-curr.peaks;
         out_map[curr.p_type].samples = out_map[curr.p_type].samples-curr.samples;
      }
    }
  }

  QList<int>keys = out_map.keys();
  for(int i=0;i<keys.size();++i){
    int type = keys[i];
    float p = static_cast<float>(out_map[type].samples)/static_cast<float>(accepted_samples+1);
#if 1
    qDebug()<<tr("[%1,%2]actual out: type:%3, peaks:%4, samples:%5, accepted_samples:%6, percentile:%7")
              .arg(__FILE__).arg(__LINE__)
              .arg(type)
              .arg(out_map[type].peaks)
              .arg(out_map[type].samples)
              .arg(accepted_samples)
              .arg(p);
#endif
    if(p<this->threshold_percent_[type] ||
       out_map[type].peaks<this->threshold_peaks_[type] ||
       type==P_NEG){
      out_map.remove(type);
      continue;
    }
    out_map[type].percentile = p;
    out_map[type].peaks = out_map[type].peaks*this->adjust_peaks_[type];
    out_map[type].samples = out_map[type].samples* this->adjust_samples_[type];
  }
  return actual_samples;
}

void MultiPatternService::Reset(){
  this->ptr_raw_frame_->Clear();
  this->feature_basic_.Reset();

}
int MultiPatternService::LoadEngine(const QString& src_path,NeuronEngineFloat & engine){
  if(src_path.isEmpty())return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<tr("[%1,%2]Fail to open file %3").arg(__FILE__).arg(__LINE__).arg(src_path);
    return 0;
  }
  QTextStream src_text(&src_file);
  //qDebug()<<tr("[%1,%2]Loading neurons from %3").arg(__FILE__).arg(__LINE__).arg(src_path);
  //step1.reset engine
  engine.ResetEngine();

  //step2.set to restore mode
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
