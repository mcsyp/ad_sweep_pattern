#ifndef SWEEPSERVICE_H
#define SWEEPSERVICE_H
#include <QThread>
#include <QVector>
#include <configparser.h>

#include "multipatternservice.h"
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

  bool Setup(QString & work_dir,ConfigParser::ConfigMap & configs);
  void StartTask(QString& sign,qint64 start,qint64 end,QString& raw_csv);

  int Index()const {return my_index_;}
signals:
  void reportCsvReady(QString report);

protected:
  virtual void run();
protected:
  static float ComputeFrequency(int samples, qint64 start, qint64 end);

protected:
  //gesture basic info
  QString signature_;//the task signature
  qint64 start_time_;
  qint64 end_time_;

  QString raw_csv_;
  MultiPatternService multi_pattern_;
private:
  int my_index_;
public:
  qint64 pack_start;
  qint64 pack_end;
};
#endif // SWEEPSERVICE_H
