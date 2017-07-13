#ifndef SWEEPSERVICE_H
#define SWEEPSERVICE_H
#include <QThread>
#include <QVector>
#include "qrineuronlib.h"
#include <configparser.h>

#include "frameclassifier.h"
#include "waveclassifier.h"

#define FRAME_ALL_NEURONS "frame_all"
#define FEATURE_ALL_NEURONS "feature_all"

#define ADJUST_SWEEP_SAMPLES    "adjust_sweep_samples"
#define ADJUST_SWEEP_COUNT      "adjust_sweep_count"

#define ADJUST_GARBAGE_SAMPLES  "adjust_garbage_samples"
#define ADJUST_GARBAGE_COUNT    "adjust_garbage_count"

#define ADJUST_WASH_SAMPLES     "adjust_wash_samples"
#define ADJUST_WASH_COUNT       "adjust_wash_count"

#define THRESHOLD_MIN_SWEEP   "min_sweep_count"
#define THRESHOLD_MIN_GARBAGE "min_garbage_count"
#define THRESHOLD_MIN_WASH    "min_wash_count"

class PatternThread : public QThread{
  Q_OBJECT
public:
  typedef struct report_item_s{
    int type;
    int count;
    int samples;
  }report_item_t;
  typedef QMap<int, report_item_t> ReportMap;//G_TYPE,report_item

  typedef QVector<PatternThread*> PatternList;
  static PatternList pattern_list;

  static constexpr int MAX_THREADS=100;
  static constexpr int DEFAULT_FREQUENCY=25;

  static PatternThread* Available();//get an available thread

  explicit PatternThread(QObject *parent = 0);
  virtual ~PatternThread();

  bool Setup(ConfigParser::ConfigMap & configs);
  void StartTask(QString& sign,qint64 start,qint64 end,QByteArray& raw_data);

  static int LoadEngine(const QString& src_path,NeuronEngineFloat & engine);
signals:
  void reportCsvReady(QString report);

protected:
  virtual void run();
protected:
  static float ComputeFrequency(int samples, qint64 start, qint64 end);

  void PrintResultCatMap(QMap<int,int> & map);
  void AnalyzeCount(const QMap<int,int> &frame_count,
                    const QMap<int,int> &frame_samples,
                    const QMap<int,int> &feature_count,
                    const QMap<int,int> &feature_samples,
                    ReportMap & out_report);
protected:

  WaveClassifier * ptr_feature_all_;
  FrameClassifier* ptr_frame_all_;
  NeuronEngineFloat engine_feature_all_;
  NeuronEngineFloat engine_frame_all_;

  //gesture basic info
  QString signature_;//the task signature
  qint64 start_time_;
  qint64 end_time_;

  QByteArray raw_data_;
  QList<int> global_map_;//from local cat to global cat

  QMap<int, float>  adjust_sample_; //G_TYPE,float value
  QMap<int, float>  adjust_count_;//G_TYPE, float value
  QMap<int, int>    threshold_;//G_TYPE, min_value
public:
  qint64 pack_start;
  qint64 pack_end;
};

#endif // SWEEPSERVICE_H
