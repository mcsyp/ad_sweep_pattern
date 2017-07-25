#include "featurebasic.h"
#include <QDebug>

using namespace qri_neuron_lib;
using namespace std;
using namespace feature_engine;

int FeatureExtractor::toArray(const QVector<float> &features,
                              float out_array[],
                              int out_size){
  if(out_array==NULL || out_size<features.size())return 0;
  if(features.isEmpty())return 0;

  memcpy(out_array,&features[0],sizeof(float)*features.size());
  return features.size();
}

FeatureBasic::FeatureBasic(NeuronEngineFloat &engine,
                           const FeatureBasic::param_window_t &window,
                           const FeatureBasic::param_gap_filter_t &gap,
                           FeatureExtractor * extractor):
  FeatureBasic(window,gap,extractor)
{
  this->ptr_engine_ = &engine;
}

FeatureBasic::FeatureBasic(const FeatureBasic::param_window_t &window, const FeatureBasic::param_gap_filter_t &gap)
  :FeatureBasic(window,gap,NULL)
{
}

FeatureBasic::FeatureBasic(const FeatureBasic::param_window_t &window, const FeatureBasic::param_gap_filter_t &gap,FeatureExtractor * extractor)
{
  if(!ValidateWindow(window) || !ValidateGap(gap)) {
    qDebug()<<tr("[%1,%2]Invalid window and gap paramters.").arg(__FILE__).arg(__LINE__);
    exit(0);
  }

  this->ptr_engine_ = NULL;
  this->window_ = window;
  this->ptr_extractor_ = extractor;

  //init gap list
  gap_list_.clear();
  for(int i=0;i<window.cols;++i){
    gap_list_.append(FilterDelta(gap.min,gap.max));
  }

  this->ptr_raw_frame_ = new DataFrame(window.rows,window.cols);
}


FeatureBasic::~FeatureBasic(){
  if(this->ptr_raw_frame_==NULL)delete ptr_raw_frame_;
}

const FeatureBasic::param_window_t &FeatureBasic::Window() const{ return this->window_;}
const FeatureBasic::param_gap_filter_t &FeatureBasic::GapFilter() const{return this->gap_filter_;}

bool FeatureBasic::ValidateWindow(const FeatureBasic::param_window_t &window){
  return !(window.cols<=0 ||
           window.rows<=0 ||
           window.rows<window.delta ||
           window.delta<=0 ||
           window.cols>WINDOW_MAX_COLS);
}

bool FeatureBasic::ValidateGap(const FeatureBasic::param_gap_filter_t &gap){
  return !(gap.max<0 || gap.min<0 || gap.min>=gap.max);
}

bool FeatureBasic::CheckDataAccpeted(float row_data[], int row_len){
  bool ret=false;
  if(row_len<window_.cols)return false;

  int min_index = (row_len>gap_list_.size())?gap_list_.size():row_len;
  for(int i=0;i<min_index;++i){
    ret = ret || gap_list_[i].Process(row_data[i]);
  }
  return ret;
}

void FeatureBasic::Reset(){
  for(int i=0;i<gap_list_.size();++i){
    gap_list_[i].Reset();
  }
}
