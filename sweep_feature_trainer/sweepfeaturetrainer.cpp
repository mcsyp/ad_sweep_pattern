

/***********************************
 * SweepFeaturePipelineTrainer
 * for data training
 * 20170626
 * song yunpeng
 ***********************************/
SweepFeaturePipelineTrainer::SweepFeaturePipelineTrainer(){}

SweepFeaturePipelineTrainer* SweepFeaturePipelineTrainer::Create(NeuronEngineFloat* engine){
  SweepFeaturePipelineTrainer* trainer=new SweepFeaturePipelineTrainer();
  //create frame
  if(!trainer->Init(engine)){
    delete trainer;
    return NULL;
  }

  return trainer;
}
SweepFeaturePipelineTrainer::~SweepFeaturePipelineTrainer(){
}

bool SweepFeaturePipelineTrainer::PushToSample(int cat, const float *row_data, int row_len){
  static float last_data=0.0f;
  if(cat<=0 || row_data==NULL || row_len<RAW_COLS) return false;

  //step1. check the category
  if(cat!=current_frame_->category_)
  {//if there is a new cat data enters, means the old data is useless, clean and reset the frame
    //step1. process the features
    PostProcessFrameForFeatures(current_frame_,sample_framelist_);

    //step2. update cat info and reset frame
    current_frame_->category_ = cat;
    current_frame_->Clear();
    last_data=0.0f;//reset the last_data
  }

  //delta check
  float gap = ComputeGap(row_data[0],last_data);
  if(gap<MIN_GAP || gap>MAX_GAP){
    return false;
  }
  last_data =  row_data[0];//if passed save the data

  //step2. save the input data
  current_frame_->Push(row_data,RAW_COLS);
  if(current_frame_->Full()){
    //if the frame is full , extract features from it and push to sample list
    PostProcessFrameForFeatures(current_frame_,sample_framelist_);
  }

  //pop the data tail
  current_frame_->Pop(RAW_DELTA);//pop raw data for next process
  return true;
}
int SweepFeaturePipelineTrainer::StartTraining(NeuronEngineFloat &engine, int iterations){
  if(iterations<=0)return 0;

  //step1. reset engine
  engine.ResetEngine();

  //step2. start iterations
  for(int iter=0;iter<iterations;++iter){
    //step1. gerenate rand array
    const int rand_size = framelist.size();
    int rand_array[rand_size];
    GenerateRandomIndex(rand_array,rand_size);

    //step2.train the frame
    for(int i=0;i<rand_size;++i){
      int index = rand_array[i];
      DataFrame* df =  framelist.at(index);
      if(!df) continue;

      //train the vector
      engine.Learn(df->category_,df->Reference(),df->FrameSize());
    }
  }

  return engine.NeuronCount();
}

void SweepFeaturePipelineTrainer::GenerateRandomIndex(int array[], int len){
  static unsigned long rand_counter=0;

  for(int i=0;i<len;i++){
      array[i]=i;
  }
  for(int i=len-1;i>0;i--){
      srand(rand_counter+time(NULL));
      rand_counter+=1;
      int index=rand()%(i+1);
      if(index!=i){
          uint8_t tmp = array[i];
          array[i] = array[index];
          array[index] = tmp;
      }
  }
}
void SweepFeaturePipelineTrainer::Reset(){
  DisposeSampleList();
  current_frame_->Clear();
  engine_->ResetEngine();
}

