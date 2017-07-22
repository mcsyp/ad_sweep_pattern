#ifdef __cplusplus
extern "C"{
#endif
#include "csvsamples.h"

/******************************
 * Sweep pattern related area
 ******************************/
//sweep source data
const int g_sweep_size=55;
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
                                 {CAT_BIG_BROOM_1,PROCESSED_18},

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

//sweep validation
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
/******************************
 * Wash pattern related area
 ******************************/
//wash source data
const int g_wash_size=25;
training_case_t g_wash_array[]={{CAT_WASH_BIN_0,PROCESSED_70},
                                   {CAT_WASH_BIN_0,PROCESSED_71},
                                   {CAT_WASH_BIN_1,PROCESSED_72},
                                   {CAT_WASH_BIN_1,PROCESSED_73},
                                   {CAT_WASH_BIN_2,PROCESSED_74},
                                   {CAT_WASH_BIN_2,PROCESSED_75},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_80},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_81},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_200},
                                   {CAT_SCRAPE_ADV_0,PROCESSED_201},
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
//wash validation data
const int g_wash_validate_size=6;
training_case_t g_wash_validate_array[]={ {CAT_WASH_BIN_0,PROCESSED_71},
                                             {CAT_WASH_BIN_1,PROCESSED_73},
                                             {CAT_SCRAPE_ADV_1,PROCESSED_85},
                                             {CAT_SCRAPE_ADV_1,PROCESSED_86},
                                             {CAT_WASH_ADV,PROCESSED_92},
                                             {CAT_WASH_ADV,PROCESSED_93},
                                           };
/******************************
 * garbage pattern related area
 ******************************/
//garbage source data
const int g_garbage_size=4;
training_case_t g_garbage_array[]={{CAT_GARBAGE_0,PROCESSED_60},
                                   {CAT_GARBAGE_0,PROCESSED_61},
                                   {CAT_GARBAGE_1,PROCESSED_62},
                                   {CAT_GARBAGE_1,PROCESSED_63},
                                };
//garbage source data
const int g_garbage_validate_size=2;
training_case_t g_garbage_validate_array[]={ {CAT_GARBAGE_0,PROCESSED_60},
                                             {CAT_GARBAGE_1,PROCESSED_63}
                                           };
/******************************
 * Nagitive test case area
 ******************************/
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


#ifdef __cplusplus
}
#endif
