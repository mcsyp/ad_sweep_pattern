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

class PatternThread : public QThread{
  Q_OBJECT
public:
  enum ENGINE_TYPE{
    ENGINE_SWEEP=0,
    ENGINE_GARBAGE=1,
    ENGINE_WASH,
    ENGINE_NUM
  };
  typedef struct report_item_s{
    int type;
    int count;
    int samples;
  }report_item_t;
  typedef QMap<int, report_item_t> ReportMap;//G_TYPE,report_item

  static constexpr int CAT_NUMBER=3;
  typedef QVector<PatternThread*> PatternList;
  static PatternList pattern_list;

  static constexpr int MAX_THREADS=100;
  static constexpr int DEFAULT_FREQUENCY=25;

  static constexpr int MIN_SWEEP_COUNT=10;
  static constexpr int MIN_GARBAGE_COUNT=4;
  static constexpr int MIN_WASH_COUNT=8;

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
  QMap<int,int> global_map_;//from local cat to global cat
};

#endif // SWEEPSERVICE_H
