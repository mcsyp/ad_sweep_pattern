#ifndef PATTERNTHREAD_H
#define PATTERNTHREAD_H

#include <QThread>
#include <QByteArray>

#include "configparser.h"
#include "featurebasic.h"
#include "featureclassifier.h"
#include "framefeatureclassifier.h"
#include "pattern_config.h"

#define CONFIG_NEURON_SWEEP    "neuron_sweep"
#define CONFIG_NEURON_GARBAGE  "neuron_garbage"
#define CONFIG_NEURON_WASH     "neuron_wash"

#define CONFIG_FORMULA_PERCENTILE  "formula_arg_percent"

#define CONFIG_THRESHOLD_MIN_SWEEP_PERCENT    "threshold_min_sweep_percent"
#define CONFIG_THRESHOLD_MIN_GARBAGE_PERCENT  "threshold_min_garbage_percent"
#define CONFIG_THRESHOLD_MIN_WASH_PERCENT     "threshold_min_wash_percent"

#define CONFIG_ADJUST_PEAK_SWEEP "adjust_peak_sweep"
#define CONFIG_ADJUST_PEAK_GARBAGE "adjust_peak_garbage"
#define CONFIG_ADJUST_PEAK_WASH "adjust_peak_wash"

#define CONFIG_ADJUST_SAMPLES_SWEEP "adjust_samples_sweep"
#define CONFIG_ADJUST_SAMPLES_GARBAGE "adjust_samples_garbage"
#define CONFIG_ADJUST_SAMPLES_WASH "adjust_samples_wash"


class MultiPatternService : public QObject
{
  Q_OBJECT
public:
  static constexpr float ADJUST_PERCETILE=1000;
  static constexpr float THRESHOLD_MIN_PERCENT=0.08f;
  typedef struct multi_pattern_result_s{
    int p_type;
    int peaks;
    int samples;
    float percentile;
  }multi_pattern_result_t;

  typedef QMap<int, multi_pattern_result_t> ResultMap;//type P_TYPE,
  explicit MultiPatternService(QObject* parnet=NULL);
  ~MultiPatternService();

  bool Setup(const QString& work_dir,const ConfigParser::ConfigMap &configs);

  /*purpose:classify the raw data. NOT in thread
   *input:
   * @raw_data, the raw_data csv format data
   *return:
   * the total samples accepted in the input strcsv
   */
  int Classify(QString & strcsv,ResultMap & map);
public:
  static int LoadEngine(const QString& src_path,NeuronEngineFloat & engine);

private:
  qri_neuron_lib::DataFrame *ptr_raw_frame_;
  feature_engine::FeatureBasic feature_basic_;

  QMap<int, NeuronEngineFloat*> engine_map_;
  QMap<int, FrameFeatureClassifier*> classifier_map_;

  QMap<int, float> threshold_percent_;
  QMap<int, float> adjust_peaks_;
  QMap<int, float> adjust_samples_;
  float adjust_percentile_times_;
};

#endif // PATTERNTHREAD_H
