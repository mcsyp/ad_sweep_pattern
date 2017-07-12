#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QStringList>
#include <QTime>

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


#define CAT_BIG_BROOM_0 100
#define PROCESSED_0    "../20170707-sound/sweep_big_broom/big_broom_right_high_0.csv"
#define PROCESSED_1    "../20170707-sound/sweep_big_broom/big_broom_right_high_1.csv"
#define PROCESSED_2    "../20170707-sound/sweep_big_broom/big_broom_right_high_2.csv"
#define PROCESSED_3    "../20170707-sound/sweep_big_broom/big_broom_right_high_3.csv"
#define PROCESSED_4    "../20170707-sound/sweep_big_broom/big_broom_right_high_4.csv"
#define PROCESSED_5    "../20170707-sound/sweep_big_broom/big_broom_right_high_5.csv"
#define PROCESSED_6    "../20170707-sound/sweep_big_broom/big_broom_right_high_6.csv"
#define PROCESSED_7    "../20170707-sound/sweep_big_broom/big_broom_right_high_7.csv"

#define CAT_BIG_BROOM_1 101
#define PROCESSED_10   "../20170707-sound/sweep_big_broom/big_broom_right_low_0.csv"
#define PROCESSED_11   "../20170707-sound/sweep_big_broom/big_broom_right_low_1.csv"
#define PROCESSED_12   "../20170707-sound/sweep_big_broom/big_broom_right_low_2.csv"
#define PROCESSED_13   "../20170707-sound/sweep_big_broom/big_broom_right_low_3.csv"
#define PROCESSED_14   "../20170707-sound/sweep_big_broom/big_broom_right_low_4.csv"
#define PROCESSED_15   "../20170707-sound/sweep_big_broom/big_broom_right_low_5.csv"
#define PROCESSED_16   "../20170707-sound/sweep_big_broom/big_broom_right_low_6.csv"
#define PROCESSED_17   "../20170707-sound/sweep_big_broom/big_broom_right_low_7.csv"

#define CAT_BIG_BROOM_2 102
#define PROCESSED_100   "../20170707-sound/sweep_big_broom/big_broom_left_high_1.csv"
#define PROCESSED_101   "../20170707-sound/sweep_big_broom/big_broom_left_high_0.csv"
#define PROCESSED_102   "../20170626-nanjing-broom/gao_left_high.csv"
#define PROCESSED_103   "../20170626-nanjing-broom/shidai_left_high.csv"
#define PROCESSED_104   "../20170626-nanjing-broom/zhipeng_left_high.csv"

#define CAT_BIG_BROOM_3 103
#define PROCESSED_110   "../20170707-sound/sweep_big_broom/big_broom_left_low_1.csv"
#define PROCESSED_111   "../20170707-sound/sweep_big_broom/big_broom_left_low_0.csv"
#define PROCESSED_112   "../20170626-nanjing-broom/gao_left_low.csv"
#define PROCESSED_113   "../20170626-nanjing-broom/shidai_left_low.csv"
#define PROCESSED_114   "../20170626-nanjing-broom/zhipeng_left_low.csv"

#define CAT_SMALL_BROOM_0 104
#define PROCESSED_20   "../20170707-sound/sweep_small_broom/small_broom_right_forward_0.csv"
#define PROCESSED_21   "../20170707-sound/sweep_small_broom/small_broom_right_forward_1.csv"
#define PROCESSED_22   "../20170707-sound/sweep_small_broom/small_broom_right_forward_2.csv"
#define PROCESSED_23   "../20170707-sound/sweep_small_broom/small_broom_right_forward_3.csv"
#define PROCESSED_24   "../20170707-sound/sweep_small_broom/small_broom_right_forward_4.csv"
#define PROCESSED_25   "../20170707-sound/sweep_small_broom/small_broom_right_forward_5.csv"
#define PROCESSED_26   "../20170707-sound/sweep_small_broom/small_broom_right_forward_6.csv"
#define PROCESSED_27   "../20170707-sound/sweep_small_broom/small_broom_right_forward_7.csv"


#define CAT_SMALL_BROOM_1 105
#define PROCESSED_30   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_0.csv"
#define PROCESSED_31   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_1.csv"
#define PROCESSED_32   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_2.csv"
#define PROCESSED_33   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_3.csv"
#define PROCESSED_34   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_4.csv"
#define PROCESSED_35   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_5.csv"
#define PROCESSED_36   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_6.csv"
#define PROCESSED_37   "../20170707-sound/sweep_small_broom/small_broom_right_horizon_7.csv"

#define CAT_PUSH_BIG_BROOM_0 100
#define PROCESSED_40   "../20170707-sound/push_big_broom/big_broom_right_high_0.csv"
#define PROCESSED_41   "../20170707-sound/push_big_broom/big_broom_right_high_1.csv"
#define PROCESSED_42   "../20170707-sound/push_big_broom/big_broom_right_high_2.csv"
#define PROCESSED_43   "../20170707-sound/push_big_broom/big_broom_right_high_3.csv"
#define PROCESSED_44   "../20170707-sound/push_big_broom/big_broom_right_high_4.csv"
#define PROCESSED_45   "../20170707-sound/push_big_broom/big_broom_right_high_5.csv"
#define PROCESSED_46   "../20170707-sound/push_big_broom/big_broom_right_high_6.csv"
#define PROCESSED_47   "../20170707-sound/push_big_broom/big_broom_right_high_7.csv"

#define CAT_PUSH_BIG_BROOM_1 100
#define PROCESSED_50   "../20170707-sound/push_big_broom/big_broom_right_low_0.csv"
#define PROCESSED_51   "../20170707-sound/push_big_broom/big_broom_right_low_1.csv"
#define PROCESSED_52   "../20170707-sound/push_big_broom/big_broom_right_low_2.csv"
#define PROCESSED_53   "../20170707-sound/push_big_broom/big_broom_right_low_3.csv"
#define PROCESSED_54   "../20170707-sound/push_big_broom/big_broom_right_low_4.csv"
#define PROCESSED_55   "../20170707-sound/push_big_broom/big_broom_right_low_5.csv"
#define PROCESSED_56   "../20170707-sound/push_big_broom/big_broom_right_low_6.csv"
#define PROCESSED_57   "../20170707-sound/push_big_broom/big_broom_right_low_7.csv"

#define CAT_GARBAGE_0 200
#define PROCESSED_60   "../20170707-sound/garbage/dul_bin_mid_0.csv"
#define PROCESSED_61   "../20170707-sound/garbage/dul_bin_mid_1.csv"

#define CAT_GARBAGE_1 200
#define PROCESSED_62   "../20170707-sound/garbage/right_high_left_low_0.csv"
#define PROCESSED_63   "../20170707-sound/garbage/right_high_left_low_1.csv"

#define CAT_WASH_BIN_0 300
#define PROCESSED_70   "../20170707-sound/empty_bin/right_circle_0.csv"
#define PROCESSED_71   "../20170707-sound/empty_bin/right_circle_1.csv"
#define CAT_WASH_BIN_1 300
#define PROCESSED_72   "../20170707-sound/empty_bin/right_horizon_0.csv"
#define PROCESSED_73   "../20170707-sound/empty_bin/right_horizon_1.csv"
#define CAT_WASH_BIN_2 300
#define PROCESSED_74   "../20170707-sound/empty_bin/right_vertical_0.csv"
#define PROCESSED_75   "../20170707-sound/empty_bin/right_vertical_1.csv"

#define CAT_SCRAPE_ADV_0 320
#define PROCESSED_80   "../20170707-sound/scrape_adv/left_crouch_forward_0.csv"
#define PROCESSED_81   "../20170707-sound/scrape_adv/left_crouch_forward_1.csv"
#define PROCESSED_82   "../20170707-sound/scrape_adv/left_crouch_horizon_0.csv"
#define PROCESSED_83   "../20170707-sound/scrape_adv/left_crouch_horizon_1.csv"
#define CAT_SCRAPE_ADV_1 320
#define PROCESSED_84   "../20170707-sound/scrape_adv/right_crouch_forward_0.csv"
#define PROCESSED_85   "../20170707-sound/scrape_adv/right_crouch_forward_1.csv"
#define PROCESSED_86   "../20170707-sound/scrape_adv/right_crouch_horizon_0.csv"
#define PROCESSED_87   "../20170707-sound/scrape_adv/right_crouch_horizon_1.csv"

#define CAT_WASH_ADV 330
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
float Possibility(int target_cat,WaveClassifier::ResultList& result_list);

void  PrintEngine(const NeuronEngineFloat &  engine);
void  PrintNeuron(const NeuronFloat *nf);

typedef struct training_case_s{
  int cat;
  const char* path;
}training_case_t;

const int g_sweep_size=54;
training_case_t g_sweep_array[]={{CAT_BIG_BROOM_0,PROCESSED_0},
                                 {CAT_BIG_BROOM_0,PROCESSED_1},
                                 {CAT_BIG_BROOM_0,PROCESSED_2},
                                 {CAT_BIG_BROOM_0,PROCESSED_3},
                                 {CAT_BIG_BROOM_0,PROCESSED_4},
                                 {CAT_BIG_BROOM_0,PROCESSED_5},
                                 {CAT_BIG_BROOM_0,PROCESSED_6},
                                 {CAT_BIG_BROOM_0,PROCESSED_7},

                                 {CAT_BIG_BROOM_1,PROCESSED_10},
                                 {CAT_BIG_BROOM_1,PROCESSED_11},
                                 {CAT_BIG_BROOM_1,PROCESSED_12},
                                 {CAT_BIG_BROOM_1,PROCESSED_13},
                                 {CAT_BIG_BROOM_1,PROCESSED_14},
                                 {CAT_BIG_BROOM_1,PROCESSED_15},
                                 {CAT_BIG_BROOM_1,PROCESSED_16},
                                 {CAT_BIG_BROOM_1,PROCESSED_17},

                                 {CAT_BIG_BROOM_2,PROCESSED_100},
                                 {CAT_BIG_BROOM_2,PROCESSED_101},
                                 {CAT_BIG_BROOM_2,PROCESSED_102},
                                 {CAT_BIG_BROOM_2,PROCESSED_103},
                                 {CAT_BIG_BROOM_2,PROCESSED_104},

                                 {CAT_BIG_BROOM_3,PROCESSED_110},
                                 {CAT_BIG_BROOM_3,PROCESSED_111},
                                 {CAT_BIG_BROOM_3,PROCESSED_112},
                                 {CAT_BIG_BROOM_3,PROCESSED_113},
                                 {CAT_BIG_BROOM_3,PROCESSED_114},

                                 {CAT_SMALL_BROOM_0,PROCESSED_20},
                                 {CAT_SMALL_BROOM_0,PROCESSED_21},
                                 {CAT_SMALL_BROOM_0,PROCESSED_22},
                                 {CAT_SMALL_BROOM_0,PROCESSED_23},
                                 {CAT_SMALL_BROOM_0,PROCESSED_24},
                                 {CAT_SMALL_BROOM_0,PROCESSED_25},

                                 {CAT_SMALL_BROOM_1,PROCESSED_30},
                                 {CAT_SMALL_BROOM_1,PROCESSED_31},
                                 {CAT_SMALL_BROOM_1,PROCESSED_32},
                                 {CAT_SMALL_BROOM_1,PROCESSED_33},
                                 {CAT_SMALL_BROOM_1,PROCESSED_34},
                                 {CAT_SMALL_BROOM_1,PROCESSED_35},

                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_40},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_41},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_42},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_43},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_44},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_45},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_46},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_47},

                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_50},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_51},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_52},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_53},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_54},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_55},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_56},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_57},
                                };
const int g_sweep_validate_size=14;
training_case_t g_sweep_validate_array[]={{CAT_BIG_BROOM_0,PROCESSED_6},
                                          {CAT_BIG_BROOM_0,PROCESSED_7},
                                          {CAT_BIG_BROOM_2,PROCESSED_104},
                                          {CAT_BIG_BROOM_3,PROCESSED_114},
                                          {CAT_BIG_BROOM_1,PROCESSED_15},
                                          {CAT_BIG_BROOM_1,PROCESSED_17},
                                          {CAT_SMALL_BROOM_0,PROCESSED_23},
                                          {CAT_SMALL_BROOM_0,PROCESSED_24},
                                          {CAT_SMALL_BROOM_1,PROCESSED_32},
                                          {CAT_SMALL_BROOM_1,PROCESSED_33},
                                          {CAT_PUSH_BIG_BROOM_0,PROCESSED_45},
                                          {CAT_PUSH_BIG_BROOM_0,PROCESSED_47},
                                          {CAT_PUSH_BIG_BROOM_0,PROCESSED_53},
                                          {CAT_PUSH_BIG_BROOM_0,PROCESSED_56}
                                         };

const int g_wash_size=23;
training_case_t g_wash_array[]={{CAT_WASH_BIN_0,PROCESSED_70},
                                   {CAT_WASH_BIN_0,PROCESSED_71},
                                   {CAT_WASH_BIN_1,PROCESSED_72},
                                   {CAT_WASH_BIN_1,PROCESSED_73},
                                   {CAT_WASH_BIN_2,PROCESSED_74},
                                   {CAT_WASH_BIN_2,PROCESSED_75},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_80},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_81},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_82},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_83},
                                   {CAT_SCRAPE_ADV_1,PROCESSED_84},
                                   {CAT_SCRAPE_ADV_1,PROCESSED_85},
                                   {CAT_SCRAPE_ADV_1,PROCESSED_86},
                                   {CAT_SCRAPE_ADV_1,PROCESSED_87},

                                   {CAT_WASH_ADV,PROCESSED_90},
                                   {CAT_WASH_ADV,PROCESSED_91},
                                   {CAT_WASH_ADV,PROCESSED_92},
                                   {CAT_WASH_ADV,PROCESSED_93},
                                   {CAT_WASH_ADV,PROCESSED_94},
                                   {CAT_WASH_ADV,PROCESSED_95},
                                   {CAT_WASH_ADV,PROCESSED_96},
                                   {CAT_WASH_ADV,PROCESSED_97},
                                   {CAT_WASH_ADV,PROCESSED_98},
                                };
const int g_wash_validate_size=6;
training_case_t g_wash_validate_array[]={ {CAT_WASH_BIN_0,PROCESSED_71},
                                             {CAT_WASH_BIN_1,PROCESSED_73},
                                             {CAT_SCRAPE_ADV_1,PROCESSED_85},
                                             {CAT_SCRAPE_ADV_1,PROCESSED_86},
                                             {CAT_WASH_ADV,PROCESSED_92},
                                             {CAT_WASH_ADV,PROCESSED_93},
                                           };

const int g_garbage_size=4;
training_case_t g_garbage_array[]={{CAT_GARBAGE_0,PROCESSED_60},
                                   {CAT_GARBAGE_0,PROCESSED_61},
                                   {CAT_GARBAGE_1,PROCESSED_62},
                                   {CAT_GARBAGE_1,PROCESSED_63},
                                };
const int g_garbage_validate_size=2;
training_case_t g_garbage_validate_array[]={ {CAT_GARBAGE_0,PROCESSED_60},
                                             {CAT_GARBAGE_1,PROCESSED_63}
                                           };

const int g_nagtive_size=19;
training_case_t g_nagtive_array[]={{CAT_GARBAGE_0,PROCESSED_0},
                                   {CAT_GARBAGE_0,PROCESSED_1},
                                   {CAT_GARBAGE_1,PROCESSED_2},
                                   {CAT_GARBAGE_1,PROCESSED_3},
                                   {CAT_WASH_ADV,PROCESSED_4},
                                   {CAT_WASH_ADV,PROCESSED_5},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_11},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_12},
                                   {CAT_WASH_BIN_1,PROCESSED_13},
                                   {CAT_WASH_BIN_1,PROCESSED_14},
                                   {CAT_WASH_BIN_2,PROCESSED_16},
                                   {CAT_WASH_BIN_1,PROCESSED_112},
                                   {CAT_WASH_BIN_1,PROCESSED_113},
                                   {CAT_WASH_BIN_2,PROCESSED_114},

                                   {CAT_PUSH_BIG_BROOM_0,PROCESSED_98},
                                   {CAT_SMALL_BROOM_0,PROCESSED_60},
                                   {CAT_SMALL_BROOM_0,PROCESSED_62},
                                   {CAT_SMALL_BROOM_0,PROCESSED_96},
                                   {CAT_SMALL_BROOM_1,PROCESSED_96},
                                  };

const int g_all_size=81;
training_case_t g_all_array[]={{CAT_BIG_BROOM_0,PROCESSED_0},
                                 {CAT_BIG_BROOM_0,PROCESSED_1},
                                 {CAT_BIG_BROOM_0,PROCESSED_2},
                                 {CAT_BIG_BROOM_0,PROCESSED_3},
                                 {CAT_BIG_BROOM_0,PROCESSED_4},
                                 {CAT_BIG_BROOM_0,PROCESSED_5},
                                 {CAT_BIG_BROOM_0,PROCESSED_6},
                                 {CAT_BIG_BROOM_0,PROCESSED_7},

                                 {CAT_BIG_BROOM_1,PROCESSED_10},
                                 {CAT_BIG_BROOM_1,PROCESSED_11},
                                 {CAT_BIG_BROOM_1,PROCESSED_12},
                                 {CAT_BIG_BROOM_1,PROCESSED_13},
                                 {CAT_BIG_BROOM_1,PROCESSED_14},
                                 {CAT_BIG_BROOM_1,PROCESSED_15},
                                 {CAT_BIG_BROOM_1,PROCESSED_16},
                                 {CAT_BIG_BROOM_1,PROCESSED_17},

                                 {CAT_BIG_BROOM_2,PROCESSED_100},
                                 {CAT_BIG_BROOM_2,PROCESSED_101},
                                 {CAT_BIG_BROOM_2,PROCESSED_102},
                                 {CAT_BIG_BROOM_2,PROCESSED_103},
                                 {CAT_BIG_BROOM_2,PROCESSED_104},

                                 {CAT_BIG_BROOM_3,PROCESSED_110},
                                 {CAT_BIG_BROOM_3,PROCESSED_111},
                                 {CAT_BIG_BROOM_3,PROCESSED_112},
                                 {CAT_BIG_BROOM_3,PROCESSED_113},
                                 {CAT_BIG_BROOM_3,PROCESSED_114},

                                 {CAT_SMALL_BROOM_0,PROCESSED_20},
                                 {CAT_SMALL_BROOM_0,PROCESSED_21},
                                 {CAT_SMALL_BROOM_0,PROCESSED_22},
                                 {CAT_SMALL_BROOM_0,PROCESSED_23},
                                 {CAT_SMALL_BROOM_0,PROCESSED_24},
                                 {CAT_SMALL_BROOM_0,PROCESSED_25},

                                 {CAT_SMALL_BROOM_1,PROCESSED_30},
                                 {CAT_SMALL_BROOM_1,PROCESSED_31},
                                 {CAT_SMALL_BROOM_1,PROCESSED_32},
                                 {CAT_SMALL_BROOM_1,PROCESSED_33},
                                 {CAT_SMALL_BROOM_1,PROCESSED_34},
                                 {CAT_SMALL_BROOM_1,PROCESSED_35},

                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_40},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_41},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_42},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_43},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_44},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_45},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_46},
                                 {CAT_PUSH_BIG_BROOM_0,PROCESSED_47},

                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_50},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_51},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_52},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_53},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_54},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_55},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_56},
                                 {CAT_PUSH_BIG_BROOM_1,PROCESSED_57},

                                  {CAT_WASH_BIN_0,PROCESSED_70},
                                  {CAT_WASH_BIN_0,PROCESSED_71},
                                  {CAT_WASH_BIN_1,PROCESSED_72},
                                  {CAT_WASH_BIN_1,PROCESSED_73},
                                  {CAT_WASH_BIN_2,PROCESSED_74},
                                  {CAT_WASH_BIN_2,PROCESSED_75},
                                  {CAT_SCRAPE_ADV_0,PROCESSED_80},
                                  {CAT_SCRAPE_ADV_0,PROCESSED_81},
                                  {CAT_SCRAPE_ADV_0,PROCESSED_82},
                                  {CAT_SCRAPE_ADV_0,PROCESSED_83},
                                  {CAT_SCRAPE_ADV_1,PROCESSED_84},
                                  {CAT_SCRAPE_ADV_1,PROCESSED_85},
                                  {CAT_SCRAPE_ADV_1,PROCESSED_86},
                                  {CAT_SCRAPE_ADV_1,PROCESSED_87},

                                  {CAT_WASH_ADV,PROCESSED_90},
                                  {CAT_WASH_ADV,PROCESSED_91},
                                  {CAT_WASH_ADV,PROCESSED_92},
                                  {CAT_WASH_ADV,PROCESSED_93},
                                  {CAT_WASH_ADV,PROCESSED_94},
                                  {CAT_WASH_ADV,PROCESSED_95},
                                  {CAT_WASH_ADV,PROCESSED_96},
                                  {CAT_WASH_ADV,PROCESSED_97},
                                  {CAT_WASH_ADV,PROCESSED_98},

                                  {CAT_GARBAGE_0,PROCESSED_60},
                                  {CAT_GARBAGE_0,PROCESSED_61},
                                  {CAT_GARBAGE_1,PROCESSED_62},
                                  {CAT_GARBAGE_1,PROCESSED_63},
                                };
const int g_all_validate_size=22;
training_case_t g_all_validate_array[]={  {CAT_BIG_BROOM_0,PROCESSED_6},
                                          {CAT_BIG_BROOM_0,PROCESSED_7},
                                          {CAT_BIG_BROOM_2,PROCESSED_104},
                                          {CAT_BIG_BROOM_3,PROCESSED_114},
                                          {CAT_BIG_BROOM_1,PROCESSED_15},
                                          {CAT_BIG_BROOM_1,PROCESSED_17},
                                          {CAT_SMALL_BROOM_0,PROCESSED_23},
                                          {CAT_SMALL_BROOM_0,PROCESSED_24},
                                          {CAT_SMALL_BROOM_1,PROCESSED_32},
                                          {CAT_SMALL_BROOM_1,PROCESSED_33},
                                          {CAT_PUSH_BIG_BROOM_1,PROCESSED_45},
                                          {CAT_PUSH_BIG_BROOM_1,PROCESSED_47},
                                          {CAT_PUSH_BIG_BROOM_0,PROCESSED_53},
                                          {CAT_PUSH_BIG_BROOM_0,PROCESSED_56},

                                          {CAT_WASH_BIN_0,PROCESSED_71},
                                          {CAT_WASH_BIN_1,PROCESSED_73},
                                          {CAT_SCRAPE_ADV_1,PROCESSED_85},
                                          {CAT_SCRAPE_ADV_1,PROCESSED_86},
                                          {CAT_WASH_ADV,PROCESSED_92},
                                          {CAT_WASH_ADV,PROCESSED_93},

                                          {CAT_GARBAGE_0,PROCESSED_60},
                                          {CAT_GARBAGE_1,PROCESSED_63}
                                         };


void  TrainCaseFrame(const char* dst_path,
                       training_case_t train_array[], int train_size,
                       training_case_t validate_array[],int validate_size){
  FrameTrainer frame_trainer;
  //push high left
  for(int i=0;i<train_size;++i){
    PushToTrainFrame(train_array[i].path,train_array[i].cat,frame_trainer);
  }
  printf("%d samples are in the list.\n",frame_trainer.sample_list_.size());

  const int engine_size=10;
  NeuronEngineFloat engine_array[engine_size];
  float max_accuracy=0;
  int max_index=0;
  for(int i=0;i<engine_size;++i){
    FrameClassifier classifier(engine_array[i]);
    int neurons = frame_trainer.StartTraining(engine_array[i],20);
    float accuracy=0.0f;

    for(int k=0;k<validate_size;++k){
      accuracy += PushToClassifyFrame(validate_array[k].path,validate_array[k].cat, classifier);
    }
    accuracy = accuracy/validate_size;
    printf("[%d] %d neurons are used. Accuracy is %.2f\n",i,neurons,accuracy);
    if(accuracy>max_accuracy){
      max_accuracy = accuracy;
      max_index = i;
    }
  }
  printf("Engine[%d]'s accuracy is %.2f. %d neurons are used.\n",max_index,max_accuracy,engine_array[max_index].NeuronCount());
  SaveEngineFloat(&engine_array[max_index],dst_path);

  do{
    FrameClassifier classifier(engine_array[max_index]);
    float accuracy=0.0f;

    for(int k=0;k<g_nagtive_size;++k){
      accuracy += PushToClassifyFrame(g_nagtive_array[k].path,g_nagtive_array[k].cat, classifier);
    }
    accuracy = accuracy/g_nagtive_size;
    printf("Negtive Accuracy is %.2f\n",1.0f-accuracy);
  }while(0);
  printf("----------------------------------------------------------------------------------------\n\n");
}

void  TrainCaseFeature(const char* dst_path,
                       training_case_t train_array[], int train_size,
                       training_case_t validate_array[],int validate_size){
  WaveTrainer feature_trainer;
  QTime time;
  time.start();
  //push high left
  for(int i=0;i<train_size;++i){
    PushToTrainFeature(train_array[i].path,train_array[i].cat,feature_trainer);
  }
  printf("%d samples are in the list takes %d ms\n",feature_trainer.sample_list_.size(),time.elapsed());

  const int engine_size=10;
  NeuronEngineFloat engine_array[engine_size];
  float max_accuracy=0;
  int max_index=0;
  for(int i=0;i<engine_size;++i){
    time.restart();
    WaveClassifier classifier(engine_array[i]);
    int neurons = feature_trainer.StartTraining(engine_array[i],20);
    int t_train = time.elapsed();

    time.restart();
    float accuracy=0.0f;
    for(int k=0;k<validate_size;++k){
      accuracy += PushToClassifyFeature(validate_array[k].path,validate_array[k].cat, classifier);
    }
    accuracy = accuracy/validate_size;
    int t_validate=time.elapsed();
    printf("[%d] %d neurons are used. Accuracy is %.2f. Training takes %d ms. Validation takes %d ms.\n",
           i,
           neurons,
           accuracy,
           t_train,
           t_validate);

    if(accuracy>max_accuracy){
      max_accuracy = accuracy;
      max_index = i;
    }
  }
  printf("Engine[%d]'s accuracy is %.2f. %d neurons are used.\n",max_index,max_accuracy,engine_array[max_index].NeuronCount());
  SaveEngineFloat(&engine_array[max_index],dst_path);

  do{
    WaveClassifier classifier(engine_array[max_index]);
    float accuracy=0.0f;

    for(int k=0;k<g_nagtive_size;++k){
      accuracy += PushToClassifyFeature(g_nagtive_array[k].path,g_nagtive_array[k].cat, classifier);
    }
    accuracy = accuracy/g_nagtive_size;
    printf("Negtive accuracy is %.2f\n",1.0f-accuracy);
  }while(0);
  printf("----------------------------------------------------------------------------------------\n\n");
}
int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
#if 0
  TrainCaseFrame("feature_sweep_neurons.csv",g_sweep_array,g_sweep_size,g_sweep_validate_array,g_sweep_validate_size);
  TrainCaseFrame("feature_garbage_neurons.csv",g_garbage_array,g_wash_size,g_garbage_validate_array,g_garbage_validate_size);
  TrainCaseFrame("feature_wash_neurons.csv",g_wash_array,g_wash_size,g_wash_validate_array,g_wash_validate_size);

  qDebug()<<endl<<"--------------------------------------------------------------------------------------"<<endl;
  TrainCaseFeature("feature_sweep_neurons.csv",g_sweep_array,g_sweep_size,g_sweep_validate_array,g_sweep_validate_size);
  TrainCaseFeature("feature_garbage_neurons.csv",g_garbage_array,g_wash_size,g_garbage_validate_array,g_garbage_validate_size);
  TrainCaseFeature("feature_wash_neurons.csv",g_wash_array,g_wash_size,g_wash_validate_array,g_wash_validate_size);
#endif

  TrainCaseFrame("frame_neurons.csv",g_all_array,g_all_size,g_all_validate_array,g_all_validate_size);
  qDebug()<<endl<<"--------------------------------------------------------------------------------------"<<endl;
  TrainCaseFeature("feature_neurons.csv",g_all_array,g_all_size,g_all_validate_array,g_all_validate_size);
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

#if 0
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

  WaveClassifier::ResultList result_list;

  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();

    //step1. analyze the row data
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    for(int i=0;i<FrameFeatureExtractor::RAW_COLS;++i){
      row_data[i] = str_data.at(i).toFloat();
    }
    if(classifier.PushToClassify(row_data,FrameFeatureExtractor::RAW_COLS,result_list)){
      ++total_frames;
      if(result_list.size()>0 && Possibility(target_cat,result_list)>0.5f){
        ++correct_frames;
      }
    }
#if 0
      for(int i=0;i<result_len;++i){
        qDebug()<<"["<<i<<"]: cat:"<<result[i].cat<<" frame_len:"<<result[i].frame_len;
      }
#endif
    }

  src_file.close();
  if(total_frames<=0)return 0.0f;

  float accuracy = (float)correct_frames / (float)total_frames;

#if 0
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
float Possibility(int target_cat,WaveClassifier::ResultList& result_list){
  int sum=0;
  if(result_list.size()==0)return 0.0f;
  for(int i=0;i<result_list.size();++i){
    int cat = result_list[i].cat;
    if(cat==target_cat){
      ++sum;
    }
  }
  return static_cast<float>(sum)/static_cast<float>(result_list.size());
}
