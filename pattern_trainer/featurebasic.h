#ifndef FEATUREWINDOW_H
#define FEATUREWINDOW_H
#include "qrineuronlib.h"
#include <QVector>
#include <QObject>

namespace feature_engine{
  class FeatureExtractor : public QObject
  {
    Q_OBJECT
  public:
    typedef struct feature_sample_s{
      int     cat;//the cat of this sample
      int     samples;//the frame len
      int     peaks;//the how many peaks or waves in this sample
      QVector<float> features;
    }feature_sample_t;
    typedef QVector<feature_sample_t> FeatureList;

    explicit FeatureExtractor(QObject *parent = 0):QObject(parent){}

    /*purpose: extract features from the input dataframe
     *input:
     * @raw_frame, the raw dataframe to extract features
     *output:
     * @out_features, the extracted feature samples are saved in this list
     */
    virtual void ExtractFeatures(const qri_neuron_lib::DataFrame & raw_frame,FeatureList & out_features)=0;

    /*purpose: convert form feature vector to array
     *input:
     * features, the feature vector
     * out_array, the feature will be output to this array
     * out_size, the max_size of out_array
     *return :
     * the feature len
     */
    static int toArray(const QVector<float> & features,float out_array[],int out_size);
  };


  class FeatureBasic : public QObject{
    Q_OBJECT

  public:
    static constexpr int  WINDOW_MAX_COLS=6;
    typedef struct param_window_s{
      int rows;
      int cols;
      int delta;
    }param_window_t;
    typedef struct param_gap_filter_s{
      float min;
      float max;
    }param_gap_filter_t;

    explicit FeatureBasic(NeuronEngineFloat & engine,const param_window_t & window, const param_gap_filter_t & gap,FeatureExtractor * extractor);
    explicit FeatureBasic(const param_window_t & window, const param_gap_filter_t & gap,FeatureExtractor* extractor);
    explicit FeatureBasic(const param_window_t & window, const param_gap_filter_t & gap);

    virtual ~FeatureBasic();

    const param_window_t& Window()const;
    const param_gap_filter_t& GapFilter() const;
    /*purpose: check if the input row_data is accepted
     *input:
     * @row_data, one row raw data
     * @row_len, length of the row
     *return:
     * true if this row of data could be accepted
     */
    bool CheckDataAccpeted(float row_data[], int row_len);
    void Reset();
  protected:
    inline bool ValidateWindow(const param_window_t & window);
    inline bool ValidateGap(const param_gap_filter_t& gap);

  protected:
    NeuronEngineFloat * ptr_engine_;
    qri_neuron_lib::DataFrame* ptr_raw_frame_;
    FeatureExtractor * ptr_extractor_;

    param_window_t window_;
    param_gap_filter_t gap_filter_;

  private:
    QVector<qri_neuron_lib::FilterDelta> gap_list_;
  };
}

#endif // FEATUREWINDOW_H
