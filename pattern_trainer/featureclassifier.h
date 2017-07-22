#ifndef DATAFRAMEFEATUREEXTRACTOR_H
#define DATAFRAMEFEATUREEXTRACTOR_H

#include <QObject>
#include <QVector>
#include "qrineuronlib.h"
#include "featurebasic.h"

namespace feature_engine {
  class FeatureClassifier: public FeatureBasic{
    Q_OBJECT
  public:
    typedef struct feature_resutl_s{
      int   cat;
      int   nid;//the closest nid
      int   samples;
      int   peaks;//how many peaks in this sample
      float percentile;//the classified samples/accepted sampels of that frame
    }result_t;
    typedef QVector<result_t> ResultList;

    explicit FeatureClassifier(const param_window_t & window, const param_gap_filter_t & gap,FeatureExtractor * extractor);
    explicit FeatureClassifier(NeuronEngineFloat & engine,const param_window_t & window, const param_gap_filter_t & gap,FeatureExtractor * extractor);

    virtual ~FeatureClassifier();

    void SetEngine(NeuronEngineFloat& engine);
    const NeuronEngineFloat& Engine()const;
    /*purpose: push dataframe row by row to engine to classify
     *input:
     * @row_data, one row raw data
     * @row_len, length of the row
     *output:
     * @out_result_list, the result list
     *return:
     * true if there is any classified reuslt ready
     */
    virtual bool PushToClassify(float row_data[],int row_len);
    //return the last Push to classify result
    const ResultList & LastResult() const;

    /*purpose: classify one dataframe
     *input:
     * @raw_frame, the raw data frame
     *output:
     * @out_result_list, all results of this frame  is saved in this list
     */
    virtual void Classify(const qri_neuron_lib::DataFrame & frame,ResultList & out_result_list);

    static float CategoryPercentile(int target_cat,const ResultList & result_list);
    typedef struct result_statistic_s{
      int total_samples;
      int accpeted_samples;
      int total_frames;
      int classified_frames;
    }result_statistic_t;
    void BeginStatistic();//start statistic mode
    const result_statistic_t & EndStatistic();//stop statistic mode

  signals:
    void resultReady(ResultList& result_list);
  protected:
    ResultList result_list_;

    bool is_statistic_mode_;
    result_statistic_t statistic_;
  };
}
#endif // DATAFRAMEFEATUREEXTRACTOR_H
