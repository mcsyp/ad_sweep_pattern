#include "featureclassifier.h"
#include <stdlib.h>
#include <QDebug>
using namespace std;
using namespace qri_neuron_lib;
using namespace feature_engine;

const FeatureClassifier::ResultList & FeatureClassifier::LastResult() const{
  return result_list_;
}

FeatureClassifier::FeatureClassifier(const FeatureBasic::param_window_t &window, const FeatureBasic::param_gap_filter_t &gap, FeatureExtractor *extractor)
  :FeatureBasic(window,gap,extractor)
{
  this->ptr_engine_=NULL;
  is_statistic_mode_=false;
}

FeatureClassifier::FeatureClassifier(NeuronEngineFloat &engine,
                                     const FeatureBasic::param_window_t &window,
                                     const FeatureBasic::param_gap_filter_t &gap,
                                     FeatureExtractor* extractor):
  FeatureBasic(engine,window,gap,extractor){
  is_statistic_mode_ = false;
}

FeatureClassifier::~FeatureClassifier(){}

void FeatureClassifier::SetEngine(NeuronEngineFloat &engine){
  this->ptr_engine_ = &engine;
}
const NeuronEngineFloat& FeatureClassifier::Engine() const{return *ptr_engine_;}

bool FeatureClassifier::PushToClassify(float row_data[], int row_len)
{
  if(ptr_raw_frame_==NULL || ptr_engine_==NULL)return false;
  if(row_data==NULL || row_len<=0 || row_len<window_.cols)return false;

  if(is_statistic_mode_){++statistic_.total_samples;}

  if(!CheckDataAccpeted(row_data,row_len)){
    //only if this data is not accepted
    return false;
  }


  if(is_statistic_mode_){++statistic_.accpeted_samples;}

  //push data to raw_frame
  ptr_raw_frame_->Push(row_data,window_.cols);
  if(ptr_raw_frame_->Full()){
    if(is_statistic_mode_){++statistic_.total_frames;}
    ResultList res;
    this->Classify(*ptr_raw_frame_, res);
    ptr_raw_frame_->Pop(window_.delta);
    if(is_statistic_mode_){
      statistic_.total_samples += window_.rows-window_.delta;
      statistic_.accpeted_samples += window_.rows-window_.delta;
    }
    if(res.size()){
      if(is_statistic_mode_){++statistic_.classified_frames;}
      emit resultReady(res);
    }
    return (res.size()!=0);
  }
  return false;
}
void FeatureClassifier::Classify(const DataFrame &frame, FeatureClassifier::ResultList &out_result_list)
{
  if(frame.Empty() || frame.Columns()<this->window_.cols)return;
  if(this->ptr_engine_==NULL) return;
  if(this->ptr_extractor_==NULL) return;
  if(this->ptr_engine_==NULL)return;

  //step1.extract features
  FeatureExtractor::FeatureList feature_list;
  this->ptr_extractor_->ExtractFeatures(frame,feature_list);
  if(feature_list.size()==0){
    return;
  }

  //step2.classify
  result_list_.clear();
  for(int i=0;i<feature_list.size();++i){
    FeatureExtractor::feature_sample_t & sample=feature_list[i];
    int min_nid;
    int ret = ptr_engine_->Classify(&(sample.features[0]),sample.features.size(),&min_nid);
    if(ret){
      result_t r;
      r.cat = ret;
      r.nid = min_nid;
      r.samples = sample.samples;
      r.peaks = sample.peaks;
      r.percentile = static_cast<float>(r.samples)/static_cast<float>(frame.RowLength());
      out_result_list.push_back(r);
      result_list_.push_back(r);//also saves it to last list
    }
  }
}


float FeatureClassifier::CategoryPercentile(int target_cat,const FeatureClassifier::ResultList & result_list){
  int sum=0;
  if(result_list.size()==0)return 0.0f;
  for(int i=0;i<result_list.size();++i){
    int cat = result_list[i].cat;
    if(cat == target_cat){
      ++sum;
    }
  }
  return static_cast<float>(sum)/static_cast<float>(result_list.size());
}

void FeatureClassifier::BeginStatistic(){
  is_statistic_mode_ = true;
  memset(&this->statistic_,0,sizeof(this->statistic_));
}
const FeatureClassifier::result_statistic_t & FeatureClassifier::EndStatistic(){
  is_statistic_mode_ = false;
  return this->statistic_;
}
