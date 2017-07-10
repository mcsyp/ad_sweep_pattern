#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>

#include <stdio.h>
#include <math.h>

#include <qrineuronlib.h>
#include "framefeatureextractor.h"
#include "frameclassifier.h"
#include "frametrainer.h"
#include "wavetrainer.h"
#include "waveclassifier.h"
#include "wavefeatureextractor.h"

#include <tooldebug.h>
#include <QDebug>

using namespace qri_neuron_lib;


#define CAT_BIG_BROOM_RIGHT_HIGH 1
#define PROCESSED_0    "../20170707-sound/sweep_big_broom/big_broom_right_high_0.csv"
#define PROCESSED_1    "../20170707-sound/sweep_big_broom/big_broom_right_high_1.csv"
#define PROCESSED_2    "../20170707-sound/sweep_big_broom/big_broom_right_high_2.csv"
#define PROCESSED_3    "../20170707-sound/sweep_big_broom/big_broom_right_high_3.csv"
#define PROCESSED_4    "../20170707-sound/sweep_big_broom/big_broom_right_high_4.csv"
#define PROCESSED_5    "../20170707-sound/sweep_big_broom/big_broom_right_high_5.csv"
#define PROCESSED_6    "../20170707-sound/sweep_big_broom/big_broom_right_high_6.csv"
#define PROCESSED_7    "../20170707-sound/sweep_big_broom/big_broom_right_high_7.csv"

#define CAT_BIG_BROOM_RIGHT_LOW 2
#define PROCESSED_10   "../20170707-sound/sweep_big_broom/big_broom_right_low_0.csv"
#define PROCESSED_11   "../20170707-sound/sweep_big_broom/big_broom_right_low_1.csv"
#define PROCESSED_12   "../20170707-sound/sweep_big_broom/big_broom_right_low_2.csv"
#define PROCESSED_13   "../20170707-sound/sweep_big_broom/big_broom_right_low_3.csv"
#define PROCESSED_14   "../20170707-sound/sweep_big_broom/big_broom_right_low_4.csv"
#define PROCESSED_15   "../20170707-sound/sweep_big_broom/big_broom_right_low_5.csv"
#define PROCESSED_16   "../20170707-sound/sweep_big_broom/big_broom_right_low_6.csv"
#define PROCESSED_17   "../20170707-sound/sweep_big_broom/big_broom_right_low_7.csv"

#define CAT_BIG_BROOM_LEFT_0 3
#define PROCESSED_100   "../20170707-sound/sweep_big_broom/big_broom_left_high_1.csv"
#define PROCESSED_101   "../20170707-sound/sweep_big_broom/big_broom_left_high_0.csv"
#define PROCESSED_102   "../20170626-nanjing-broom/gao_left_high.csv"
#define PROCESSED_103   "../20170626-nanjing-broom/shidai_left_high.csv"
#define PROCESSED_104   "../20170626-nanjing-broom/zhipeng_left_high.csv"

#define CAT_BIG_BROOM_LEFT_1 4
#define PROCESSED_110   "../20170707-sound/sweep_big_broom/big_broom_left_low_1.csv"
#define PROCESSED_111   "../20170707-sound/sweep_big_broom/big_broom_left_low_0.csv"
#define PROCESSED_112   "../20170626-nanjing-broom/gao_left_low.csv"
#define PROCESSED_113   "../20170626-nanjing-broom/shidai_left_low.csv"
#define PROCESSED_114   "../20170626-nanjing-broom/zhipeng_left_low.csv"

#define CAT_SMALL_BROOM_RIGHT_FORWARD 10
#define PROCESSED_20   "../20170707-sound/sweep_small_broom/small_broom_right_forward_0.csv"
#define PROCESSED_21   "../20170707-sound/sweep_small_broom/small_broom_right_forward_1.csv"
#define PROCESSED_22   "../20170707-sound/sweep_small_broom/small_broom_right_forward_2.csv"
#define PROCESSED_23   "../20170707-sound/sweep_small_broom/small_broom_right_forward_3.csv"
#define PROCESSED_24   "../20170707-sound/sweep_small_broom/small_broom_right_forward_4.csv"
#define PROCESSED_25   "../20170707-sound/sweep_small_broom/small_broom_right_forward_5.csv"
#define PROCESSED_26   "../20170707-sound/sweep_small_broom/small_broom_right_forward_6.csv"
#define PROCESSED_27   "../20170707-sound/sweep_small_broom/small_broom_right_forward_7.csv"


#define CAT_SMALL_BROOM_RIGHT_HRIZON 11
#define PROCESSED_30   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_0.csv"
#define PROCESSED_31   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_1.csv"
#define PROCESSED_32   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_2.csv"
#define PROCESSED_33   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_3.csv"
#define PROCESSED_34   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_4.csv"
#define PROCESSED_35   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_5.csv"
#define PROCESSED_36   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_6.csv"
#define PROCESSED_37   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_7.csv"

#define CAT_PUSH_BIG_BROOM 20
#define PROCESSED_40   "../20170707-sound/push_big_broom/big_broom_right_high_0.csv"
#define PROCESSED_41   "../20170707-sound/push_big_broom/big_broom_right_high_1.csv"
#define PROCESSED_42   "../20170707-sound/push_big_broom/big_broom_right_high_2.csv"
#define PROCESSED_43   "../20170707-sound/push_big_broom/big_broom_right_high_3.csv"
#define PROCESSED_44   "../20170707-sound/push_big_broom/big_broom_right_high_4.csv"
#define PROCESSED_45   "../20170707-sound/push_big_broom/big_broom_right_high_5.csv"
#define PROCESSED_46   "../20170707-sound/push_big_broom/big_broom_right_high_6.csv"
#define PROCESSED_47   "../20170707-sound/push_big_broom/big_broom_right_high_7.csv"

#define PROCESSED_50   "../20170707-sound/push_big_broom/big_broom_right_low_0.csv"
#define PROCESSED_51   "../20170707-sound/push_big_broom/big_broom_right_low_1.csv"
#define PROCESSED_52   "../20170707-sound/push_big_broom/big_broom_right_low_2.csv"
#define PROCESSED_53   "../20170707-sound/push_big_broom/big_broom_right_low_3.csv"
#define PROCESSED_54   "../20170707-sound/push_big_broom/big_broom_right_low_4.csv"
#define PROCESSED_55   "../20170707-sound/push_big_broom/big_broom_right_low_5.csv"
#define PROCESSED_56   "../20170707-sound/push_big_broom/big_broom_right_low_6.csv"
#define PROCESSED_57   "../20170707-sound/push_big_broom/big_broom_right_low_7.csv"

#define CAT_GARBAGE_0 30
#define PROCESSED_60   "../20170707-sound/garbage/dul_bin_mid_0.csv"
#define PROCESSED_61   "../20170707-sound/garbage/dul_bin_mid_1.csv"

#define CAT_GARBAGE_1 31
#define PROCESSED_62   "../20170707-sound/garbage/right_high_left_low_0.csv"
#define PROCESSED_63   "../20170707-sound/garbage/right_high_left_low_1.csv"

#define CAT_EMPTY_BIN_0 40
#define PROCESSED_70   "../20170707-sound/empty_bin/right_circle_0.csv"
#define PROCESSED_71   "../20170707-sound/empty_bin/right_circle_1.csv"
#define CAT_EMPTY_BIN_1 41
#define PROCESSED_72   "../20170707-sound/empty_bin/right_horizon_0.csv"
#define PROCESSED_73   "../20170707-sound/empty_bin/right_horizon_1.csv"
#define CAT_EMPTY_BIN_2 42
#define PROCESSED_74   "../20170707-sound/empty_bin/right_vertical_0.csv"
#define PROCESSED_75   "../20170707-sound/empty_bin/right_vertical_1.csv"

#define CAT_SCRAPE_ADV_0 50
#define PROCESSED_80   "../20170707-sound/scrape_adv/left_crouch_forward_0.csv"
#define PROCESSED_81   "../20170707-sound/scrape_adv/left_crouch_forward_1.csv"
#define PROCESSED_82   "../20170707-sound/scrape_adv/left_crouch_horizon_0.csv"
#define PROCESSED_83   "../20170707-sound/scrape_adv/left_crouch_horizon_1.csv"
#define CAT_SCRAPE_ADV_1 51
#define PROCESSED_84   "../20170707-sound/scrape_adv/right_crouch_forward_0.csv"
#define PROCESSED_85   "../20170707-sound/scrape_adv/right_crouch_forward_1.csv"
#define PROCESSED_86   "../20170707-sound/scrape_adv/right_crouch_horizon_0.csv"
#define PROCESSED_87   "../20170707-sound/scrape_adv/right_crouch_horizon_1.csv"

#define CAT_WASH_ADV 60
#define PROCESSED_90   "../20170707-sound/wash_adv/left_crouch_horizon_0.csv"
#define PROCESSED_91   "../20170707-sound/wash_adv/left_crouch_vertical_0.csv"
#define PROCESSED_92   "../20170707-sound/wash_adv/left_stand_horizon_0.csv"
#define PROCESSED_93   "../20170707-sound/wash_adv/left_stand_vertical_0.csv"
#define PROCESSED_94   "../20170707-sound/wash_adv/right_crouch_horizon_0.csv"
#define PROCESSED_95   "../20170707-sound/wash_adv/right_stand_horizon_0.csv"
#define PROCESSED_96   "../20170707-sound/wash_adv/right_stand_horizon_1.csv"
#define PROCESSED_97   "../20170707-sound/wash_adv/right_stand_vertical_0.csv"
#define PROCESSED_98   "../20170707-sound/wash_adv/right_stand_vertical_1.csv"


int   PushToTrainFrame(const char* src_path,int cat, FrameTrainer & trainer);
float PushToClassifyFrame(const char * src_path, int target_cat ,FrameClassifier & classifier);

int   PushToTrainFeature(const char* src_path,int cat, WaveTrainer & trainer);
float PushToClassifyFeature(const char * src_path, int target_cat ,WaveClassifier & classifier);
float Possibility(int target_cat,WaveClassifier::result_cat_t * result_array,int result_len);

void  PrintEngine(const NeuronEngineFloat &  engine);
void  PrintNeuron(const NeuronFloat *nf);

void  TrainCaseFrameSweep(){
  NeuronEngineFloat engine_frame_;//(NeuronEngineFloat::MODE_KNN);
  FrameTrainer frame_trainer;
  FrameClassifier frame_classifier(engine_frame_);

  do{
    //push high left
    PushToTrainFrame(PROCESSED_0,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_1,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_2,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_3,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_4,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_5,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_6,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);
    PushToTrainFrame(PROCESSED_7,CAT_BIG_BROOM_RIGHT_HIGH,frame_trainer);

    PushToTrainFrame(PROCESSED_10,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_11,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_12,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_13,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_14,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_15,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_16,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);
    PushToTrainFrame(PROCESSED_17,CAT_BIG_BROOM_RIGHT_LOW,frame_trainer);

    PushToTrainFrame(PROCESSED_100,CAT_BIG_BROOM_LEFT_0,frame_trainer);
    PushToTrainFrame(PROCESSED_101,CAT_BIG_BROOM_LEFT_0,frame_trainer);
    PushToTrainFrame(PROCESSED_102,CAT_BIG_BROOM_LEFT_0,frame_trainer);
    PushToTrainFrame(PROCESSED_103,CAT_BIG_BROOM_LEFT_0,frame_trainer);
    PushToTrainFrame(PROCESSED_104,CAT_BIG_BROOM_LEFT_0,frame_trainer);

    PushToTrainFrame(PROCESSED_110,CAT_BIG_BROOM_LEFT_1,frame_trainer);
    PushToTrainFrame(PROCESSED_111,CAT_BIG_BROOM_LEFT_1,frame_trainer);
    PushToTrainFrame(PROCESSED_112,CAT_BIG_BROOM_LEFT_1,frame_trainer);
    PushToTrainFrame(PROCESSED_113,CAT_BIG_BROOM_LEFT_1,frame_trainer);
    PushToTrainFrame(PROCESSED_114,CAT_BIG_BROOM_LEFT_1,frame_trainer);

    PushToTrainFrame(PROCESSED_20,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_trainer);
    PushToTrainFrame(PROCESSED_21,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_trainer);
    PushToTrainFrame(PROCESSED_22,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_trainer);
    PushToTrainFrame(PROCESSED_23,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_trainer);
    PushToTrainFrame(PROCESSED_24,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_trainer);
    PushToTrainFrame(PROCESSED_25,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_trainer);

    PushToTrainFrame(PROCESSED_30,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_trainer);
    PushToTrainFrame(PROCESSED_31,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_trainer);
    PushToTrainFrame(PROCESSED_32,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_trainer);
    PushToTrainFrame(PROCESSED_33,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_trainer);
    PushToTrainFrame(PROCESSED_34,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_trainer);
    PushToTrainFrame(PROCESSED_35,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_trainer);

    PushToTrainFrame(PROCESSED_40,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_41,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_42,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_43,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_44,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_45,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_46,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_47,CAT_PUSH_BIG_BROOM,frame_trainer);

    PushToTrainFrame(PROCESSED_50,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_51,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_52,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_53,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_54,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_55,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_56,CAT_PUSH_BIG_BROOM,frame_trainer);
    PushToTrainFrame(PROCESSED_57,CAT_PUSH_BIG_BROOM,frame_trainer);
    printf("%d samples are in the list.\n",frame_trainer.sample_list_.size());

    int neurons = frame_trainer.StartTraining(engine_frame_,20);
    printf("used neurons=%d\n",neurons);


    //positive test
    PushToClassifyFrame(PROCESSED_6,CAT_BIG_BROOM_RIGHT_HIGH,frame_classifier);
    PushToClassifyFrame(PROCESSED_7,CAT_BIG_BROOM_RIGHT_HIGH,frame_classifier);

    PushToClassifyFrame(PROCESSED_104,CAT_BIG_BROOM_LEFT_0,frame_classifier);
    PushToClassifyFrame(PROCESSED_114,CAT_BIG_BROOM_LEFT_1,frame_classifier);

    PushToClassifyFrame(PROCESSED_15,CAT_BIG_BROOM_RIGHT_LOW,frame_classifier);
    PushToClassifyFrame(PROCESSED_17,CAT_BIG_BROOM_RIGHT_LOW,frame_classifier);

    PushToClassifyFrame(PROCESSED_24,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_classifier);
    PushToClassifyFrame(PROCESSED_23,CAT_SMALL_BROOM_RIGHT_FORWARD,frame_classifier);

    PushToClassifyFrame(PROCESSED_33,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_classifier);
    PushToClassifyFrame(PROCESSED_32,CAT_SMALL_BROOM_RIGHT_HRIZON,frame_classifier);

    PushToClassifyFrame(PROCESSED_45,CAT_PUSH_BIG_BROOM,frame_classifier);
    PushToClassifyFrame(PROCESSED_47,CAT_PUSH_BIG_BROOM,frame_classifier);

    PushToClassifyFrame(PROCESSED_56,CAT_PUSH_BIG_BROOM,frame_classifier);
    PushToClassifyFrame(PROCESSED_53,CAT_PUSH_BIG_BROOM,frame_classifier);


  }while(0);

}
void  TrainCaseFrameGarbage(){
  NeuronEngineFloat engine_frame_;//(NeuronEngineFloat::MODE_KNN);
  FrameTrainer frame_trainer;
  FrameClassifier frame_classifier(engine_frame_);
  do{
    //push high left
    PushToTrainFrame(PROCESSED_60,CAT_GARBAGE_0,frame_trainer);
    PushToTrainFrame(PROCESSED_61,CAT_GARBAGE_0,frame_trainer);
    PushToTrainFrame(PROCESSED_62,CAT_GARBAGE_1,frame_trainer);
    PushToTrainFrame(PROCESSED_63,CAT_GARBAGE_1,frame_trainer);

    PushToTrainFrame(PROCESSED_70,CAT_EMPTY_BIN_0,frame_trainer);
    PushToTrainFrame(PROCESSED_71,CAT_EMPTY_BIN_0,frame_trainer);
    PushToTrainFrame(PROCESSED_72,CAT_EMPTY_BIN_1,frame_trainer);
    PushToTrainFrame(PROCESSED_73,CAT_EMPTY_BIN_1,frame_trainer);
    PushToTrainFrame(PROCESSED_74,CAT_EMPTY_BIN_2,frame_trainer);
    PushToTrainFrame(PROCESSED_75,CAT_EMPTY_BIN_2,frame_trainer);

    PushToTrainFrame(PROCESSED_80,CAT_SCRAPE_ADV_0,frame_trainer);
    PushToTrainFrame(PROCESSED_81,CAT_SCRAPE_ADV_0,frame_trainer);
    PushToTrainFrame(PROCESSED_82,CAT_SCRAPE_ADV_0,frame_trainer);
    PushToTrainFrame(PROCESSED_83,CAT_SCRAPE_ADV_0,frame_trainer);
    PushToTrainFrame(PROCESSED_84,CAT_SCRAPE_ADV_1,frame_trainer);
    PushToTrainFrame(PROCESSED_85,CAT_SCRAPE_ADV_1,frame_trainer);
    PushToTrainFrame(PROCESSED_86,CAT_SCRAPE_ADV_1,frame_trainer);
    PushToTrainFrame(PROCESSED_87,CAT_SCRAPE_ADV_1,frame_trainer);

    PushToTrainFrame(PROCESSED_90,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_91,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_92,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_93,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_94,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_95,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_96,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_97,CAT_WASH_ADV,frame_trainer);
    PushToTrainFrame(PROCESSED_98,CAT_WASH_ADV,frame_trainer);

    printf("%d samples are in the list.\n",frame_trainer.sample_list_.size());

    int neurons = frame_trainer.StartTraining(engine_frame_,20);
    printf("used neurons=%d\n",neurons);

    //positive test
    PushToClassifyFrame(PROCESSED_63,CAT_GARBAGE_1,frame_classifier);
    PushToClassifyFrame(PROCESSED_61,CAT_GARBAGE_0,frame_classifier);

    PushToClassifyFrame(PROCESSED_71,CAT_EMPTY_BIN_0,frame_classifier);
    PushToClassifyFrame(PROCESSED_73,CAT_EMPTY_BIN_1,frame_classifier);

    PushToClassifyFrame(PROCESSED_85,CAT_SCRAPE_ADV_1,frame_classifier);
    PushToClassifyFrame(PROCESSED_86,CAT_SCRAPE_ADV_1,frame_classifier);

    PushToClassifyFrame(PROCESSED_92,CAT_WASH_ADV,frame_classifier);
    PushToClassifyFrame(PROCESSED_93,CAT_WASH_ADV,frame_classifier);

  }while(0);
}
void  TrainCaseSweep(){
  NeuronEngineFloat engine_feature_;
  WaveTrainer feature_trainer;
  WaveClassifier feature_classifier(engine_feature_);

  do{
    //push high left
    PushToTrainFeature(PROCESSED_0,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_1,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_2,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_3,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_4,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_5,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_6,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);
    PushToTrainFeature(PROCESSED_7,CAT_BIG_BROOM_RIGHT_HIGH,feature_trainer);

    PushToTrainFeature(PROCESSED_10,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_11,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_12,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_13,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_14,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_15,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_16,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);
    PushToTrainFeature(PROCESSED_17,CAT_BIG_BROOM_RIGHT_LOW,feature_trainer);

    PushToTrainFeature(PROCESSED_100,CAT_BIG_BROOM_LEFT_0,feature_trainer);
    PushToTrainFeature(PROCESSED_101,CAT_BIG_BROOM_LEFT_0,feature_trainer);
    PushToTrainFeature(PROCESSED_102,CAT_BIG_BROOM_LEFT_0,feature_trainer);
    PushToTrainFeature(PROCESSED_103,CAT_BIG_BROOM_LEFT_0,feature_trainer);

    PushToTrainFeature(PROCESSED_110,CAT_BIG_BROOM_LEFT_1,feature_trainer);
    PushToTrainFeature(PROCESSED_111,CAT_BIG_BROOM_LEFT_1,feature_trainer);
    PushToTrainFeature(PROCESSED_112,CAT_BIG_BROOM_LEFT_1,feature_trainer);
    PushToTrainFeature(PROCESSED_113,CAT_BIG_BROOM_LEFT_1,feature_trainer);

    PushToTrainFeature(PROCESSED_20,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_trainer);
    PushToTrainFeature(PROCESSED_21,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_trainer);
    PushToTrainFeature(PROCESSED_22,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_trainer);
    PushToTrainFeature(PROCESSED_23,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_trainer);
    PushToTrainFeature(PROCESSED_24,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_trainer);
    PushToTrainFeature(PROCESSED_25,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_trainer);

    PushToTrainFeature(PROCESSED_30,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_trainer);
    PushToTrainFeature(PROCESSED_31,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_trainer);
    PushToTrainFeature(PROCESSED_32,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_trainer);
    PushToTrainFeature(PROCESSED_33,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_trainer);
    PushToTrainFeature(PROCESSED_34,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_trainer);
    PushToTrainFeature(PROCESSED_35,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_trainer);

    PushToTrainFeature(PROCESSED_40,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_41,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_42,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_43,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_44,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_45,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_46,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_47,CAT_PUSH_BIG_BROOM,feature_trainer);

    PushToTrainFeature(PROCESSED_50,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_51,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_52,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_53,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_54,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_55,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_56,CAT_PUSH_BIG_BROOM,feature_trainer);
    PushToTrainFeature(PROCESSED_57,CAT_PUSH_BIG_BROOM,feature_trainer);
    printf("%d samples are in the list.\n",feature_trainer.sample_list_.size());

    int neurons = feature_trainer.StartTraining(engine_feature_,20);
    printf("used neurons=%d\n",neurons);


    //positive test
    PushToClassifyFeature(PROCESSED_6,CAT_BIG_BROOM_RIGHT_HIGH,feature_classifier);
    PushToClassifyFeature(PROCESSED_7,CAT_BIG_BROOM_RIGHT_HIGH,feature_classifier);

    PushToClassifyFeature(PROCESSED_104,CAT_BIG_BROOM_LEFT_0,feature_classifier);
    PushToClassifyFeature(PROCESSED_114,CAT_BIG_BROOM_LEFT_1,feature_classifier);

    PushToClassifyFeature(PROCESSED_15,CAT_BIG_BROOM_RIGHT_LOW,feature_classifier);
    PushToClassifyFeature(PROCESSED_17,CAT_BIG_BROOM_RIGHT_LOW,feature_classifier);

    PushToClassifyFeature(PROCESSED_24,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_classifier);
    PushToClassifyFeature(PROCESSED_23,CAT_SMALL_BROOM_RIGHT_FORWARD,feature_classifier);

    PushToClassifyFeature(PROCESSED_33,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_classifier);
    PushToClassifyFeature(PROCESSED_32,CAT_SMALL_BROOM_RIGHT_HRIZON,feature_classifier);

    PushToClassifyFeature(PROCESSED_45,CAT_PUSH_BIG_BROOM,feature_classifier);
    PushToClassifyFeature(PROCESSED_47,CAT_PUSH_BIG_BROOM,feature_classifier);

    PushToClassifyFeature(PROCESSED_56,CAT_PUSH_BIG_BROOM,feature_classifier);
    PushToClassifyFeature(PROCESSED_53,CAT_PUSH_BIG_BROOM,feature_classifier);


  }while(0);
}

void  TrainCaseGarbage(){
  NeuronEngineFloat engine_feature_;
  WaveTrainer feature_trainer;
  WaveClassifier feature_classifier(engine_feature_);
  do{
    //push high left
    PushToTrainFeature(PROCESSED_60,CAT_GARBAGE_0,feature_trainer);
    PushToTrainFeature(PROCESSED_61,CAT_GARBAGE_0,feature_trainer);
    PushToTrainFeature(PROCESSED_62,CAT_GARBAGE_1,feature_trainer);
    PushToTrainFeature(PROCESSED_63,CAT_GARBAGE_1,feature_trainer);

    PushToTrainFeature(PROCESSED_70,CAT_EMPTY_BIN_0,feature_trainer);
    PushToTrainFeature(PROCESSED_71,CAT_EMPTY_BIN_0,feature_trainer);
    PushToTrainFeature(PROCESSED_72,CAT_EMPTY_BIN_1,feature_trainer);
    PushToTrainFeature(PROCESSED_73,CAT_EMPTY_BIN_1,feature_trainer);
    PushToTrainFeature(PROCESSED_74,CAT_EMPTY_BIN_2,feature_trainer);
    PushToTrainFeature(PROCESSED_75,CAT_EMPTY_BIN_2,feature_trainer);

    PushToTrainFeature(PROCESSED_80,CAT_SCRAPE_ADV_0,feature_trainer);
    PushToTrainFeature(PROCESSED_81,CAT_SCRAPE_ADV_0,feature_trainer);
    PushToTrainFeature(PROCESSED_82,CAT_SCRAPE_ADV_0,feature_trainer);
    PushToTrainFeature(PROCESSED_83,CAT_SCRAPE_ADV_0,feature_trainer);
    PushToTrainFeature(PROCESSED_84,CAT_SCRAPE_ADV_1,feature_trainer);
    PushToTrainFeature(PROCESSED_85,CAT_SCRAPE_ADV_1,feature_trainer);
    PushToTrainFeature(PROCESSED_86,CAT_SCRAPE_ADV_1,feature_trainer);
    PushToTrainFeature(PROCESSED_87,CAT_SCRAPE_ADV_1,feature_trainer);

    PushToTrainFeature(PROCESSED_90,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_91,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_92,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_93,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_94,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_95,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_96,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_97,CAT_WASH_ADV,feature_trainer);
    PushToTrainFeature(PROCESSED_98,CAT_WASH_ADV,feature_trainer);

    printf("%d samples are in the list.\n",feature_trainer.sample_list_.size());

    int neurons = feature_trainer.StartTraining(engine_feature_,20);
    printf("used neurons=%d\n",neurons);

    //positive test
    PushToClassifyFeature(PROCESSED_63,CAT_GARBAGE_1,feature_classifier);
    PushToClassifyFeature(PROCESSED_61,CAT_GARBAGE_0,feature_classifier);

    PushToClassifyFeature(PROCESSED_71,CAT_EMPTY_BIN_0,feature_classifier);
    PushToClassifyFeature(PROCESSED_73,CAT_EMPTY_BIN_1,feature_classifier);

    PushToClassifyFeature(PROCESSED_85,CAT_SCRAPE_ADV_1,feature_classifier);
    PushToClassifyFeature(PROCESSED_86,CAT_SCRAPE_ADV_1,feature_classifier);

    PushToClassifyFeature(PROCESSED_92,CAT_WASH_ADV,feature_classifier);
    PushToClassifyFeature(PROCESSED_93,CAT_WASH_ADV,feature_classifier);

  }while(0);
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  //TrainCaseFrameSweep();
  TrainCaseFrameGarbage();
  //TrainCaseSweep();
  //TrainCaseGarbage();
}


void PrintEngine(const NeuronEngineFloat &  engine){
  //dump all neurons
  printf("Neuron number is %d\n", engine.NeuronCount());
  for(int i=0;i<engine.NeuronCount();++i){
    const NeuronFloat * nf = engine.ReadNeuron(i);
    PrintNeuron(nf);
  }
}

void PrintNeuron(const NeuronFloat* nf){
  const int max_buffer_size=1024;
  float nf_buffer[max_buffer_size];
  printf("cat=%d, min_aif=%.2f, aif=%.2f, vector:",nf->Cat(),nf->MinAIF(),nf->Aif());
  int len = nf->ReadNeuronMem(nf_buffer,max_buffer_size);
  for(int k=0;k<len;++k){
    printf(" %.2f ",nf_buffer[k]);
  }
  printf("]\n");
}

int PushToTrainFrame(const char* src_path,int cat, FrameTrainer & trainer){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  float row_data[FrameTrainer::RAW_COLS];
  int sample_size=0;
  //step1. read and push adll data to the frame list
  src_text.readLine();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<FrameTrainer::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }

    //step2. push to trainer;
    sample_size = trainer.PushToSample(cat,row_data,FrameTrainer::RAW_COLS);
  }

  src_file.close();
  return sample_size;
}

float PushToClassifyFrame(const char *src_path, int target_cat, FrameClassifier &classifier){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  float row_data[FrameFeatureExtractor::RAW_COLS];
  int total_frames=0;
  int correct_frames=0;
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<FrameFeatureExtractor::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }
#if 1
    int cat = classifier.PushToClassify(row_data,FrameFeatureExtractor::RAW_COLS);
    if(cat>=0){
      ++total_frames;
      if(cat==target_cat){
        ++correct_frames;
      }
    }
#endif

  }
  src_file.close();
  if(total_frames<=0)return 0.0f;

  float accuracy = (float)correct_frames / (float)total_frames;

#if 1
  printf("Validating [%s] with target_cat [%2d]\n",src_path,target_cat);
  printf("[%3d/%3d] samples are correct with target [%2d]. Accuracy is [%.2f]\n",
         correct_frames,
         total_frames,
         target_cat,
         accuracy);
  printf("----------------------------------------------------------------------------------------------\n");
#endif

  return accuracy;
}

int PushToTrainFeature(const char* src_path,int cat, WaveTrainer & trainer){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  const int row_size =WaveFeatureExtractor::RAW_COLS;
  float row_data[row_size];
  int sample_size=0;
  //step1. read and push adll data to the frame list
  src_text.readLine();
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<FrameTrainer::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }

    //step2. push to trainer;
    sample_size = trainer.PushToSample(cat,row_data,row_size);
  }

  src_file.close();
  return sample_size;
}

float PushToClassifyFeature(const char *src_path, int target_cat, WaveClassifier &classifier){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("Fail to open %s.\n",src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  float row_data[FrameFeatureExtractor::RAW_COLS];
  int total_frames=0;
  int correct_frames=0;

  const int result_size = WaveFeatureExtractor::RAW_ROWS;
  WaveClassifier::result_cat_t result[result_size];

  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<FrameFeatureExtractor::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }
    int result_len = classifier.PushToClassify(row_data,FrameFeatureExtractor::RAW_COLS,result,result_size);
    if(result_len>=0){
      ++total_frames;
      if(result_len>0 && Possibility(target_cat,result,result_len)>0.5f){
        ++correct_frames;
      }
#if 0
      for(int i=0;i<result_len;++i){
        qDebug()<<"["<<i<<"]: cat:"<<result[i].cat<<" frame_len:"<<result[i].frame_len;
      }
#endif
    }

  }
  src_file.close();
  if(total_frames<=0)return 0.0f;

  float accuracy = (float)correct_frames / (float)total_frames;

#if 1
  printf("Validating [%s] with target_cat [%2d]\n",src_path,target_cat);
  printf("[%3d/%3d] samples are correct with target [%2d]. Accuracy is [%.2f]\n",
         correct_frames,
         total_frames,
         target_cat,
         accuracy);
  printf("----------------------------------------------------------------------------------------------\n");
#endif

  return accuracy;
}
float Possibility(int target_cat,WaveClassifier::result_cat_t * result_array,int result_len){
  int sum=0;
  if(result_len==0)return 0.0f;
  for(int i=0;i<result_len;++i){
    int cat = result_array[i].cat;
    if(cat==target_cat){
      ++sum;
    }
  }
  float accuracy = static_cast<float>(sum)/static_cast<float>(result_len);
  printf("Correct rate with target [%d] is [%.2f]\n",target_cat,accuracy);
  return accuracy;
}
