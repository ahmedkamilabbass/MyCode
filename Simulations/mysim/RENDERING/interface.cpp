#include "interface.h"
#ifdef _GRAPHICS_

Interface::Interface(QWidget *parent, Experiment *_exp, Viewer *_viewer /*, Video *_video */)
  : QWidget(parent)
{
  exp = _exp;
  viewer = _viewer;
  m_engine = new Engine( exp );   // Engine use to run the simulation
  //video = _video;
  frameRate = 12;

  // flag_activate_loadGeno_buttons   = true;
  // flag_activate_play_buttons       = false;
  // flag_activate_step_buttons       = false;
  // flag_activate_pause_buttons      = false;
  // flag_activate_next_buttons       = false;
  // flag_activate_quit_buttons       = true;
  // flag_activate_rec_buttons        = true;
  // flag_activate_stop_rec_buttons   = false;
  
  flag_activate_loadGeno_buttons       = true;
  //flag_activate_reset_eval_loop_botton = true;
  flag_activate_play_buttons           = true;
  flag_activate_step_buttons           = true;
  flag_activate_pause_buttons          = true;
  flag_activate_next_buttons           = true;
  flag_activate_quit_buttons           = true;
  
  m_engine->setDelay( 50 );
  createButtons();
  connect(m_engine, SIGNAL(advanced()), viewer, SLOT(update()));
  connect(m_engine, SIGNAL(advanced()), this, SLOT(changeWindowTitle()));
  
  mainLayout = new QVBoxLayout;
  viewer->setMinimumHeight(300);

  mainLayout->addWidget(viewer, 1);
  mainLayout->addLayout(buttonsLayout, 0);
  setLayout(mainLayout);
  updateButtons();
  windowTitle = tr("Elio Tuci Sim-Artificial-World");
  setWindowTitle(windowTitle);
  resize(1100, 800); //resize the main widget
  move( 50, 50);//position the main widget
}

Interface::~Interface()
{
    delete m_engine;
}

void Interface::open()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Load a genotype"));
  if (!fileName.isEmpty()){
    ba = fileName.toLatin1();
    
    flag_activate_loadGeno_buttons       = false;
    //flag_activate_reset_eval_loop_botton = true;
    flag_activate_play_buttons           = false;
    flag_activate_step_buttons           = false;
    flag_activate_pause_buttons          = false;
    flag_activate_next_buttons           = false;
    flag_activate_quit_buttons           = true;
        
    updateButtons(); 
  }
}

void Interface::updateButtons()
{
  openButton->setEnabled            ( flag_activate_loadGeno_buttons );
  //reset_eval_loopButton->setEnabled ( flag_activate_reset_eval_loop_botton );
  playButton->setEnabled            ( flag_activate_play_buttons     );
  pauseButton->setEnabled           ( flag_activate_pause_buttons    );
  stopButton->setEnabled            ( flag_activate_next_buttons     );
  oneStepButton->setEnabled         ( flag_activate_step_buttons     );
  quitButton->setEnabled            ( flag_activate_quit_buttons     );
  recButton->setEnabled            ( flag_activate_rec_buttons     );
  stopRecButton->setEnabled            ( flag_activate_stop_rec_buttons     );
}

void Interface::createButtons()
{
  QSize iconSize(36, 36);
  std::string absolute_path = (home + local_path).c_str();
  
  openButton = new QToolButton;
  openButton->setIcon( QIcon( (absolute_path + "/RENDERING/IMAGES/open.png").c_str() ) );
  openButton->setIconSize(iconSize);
  openButton->setToolTip(tr("Load Genotype"));
  connect(openButton, SIGNAL(clicked()), this, SLOT(open()));

//   reset_eval_loopButton = new QToolButton;
//   reset_eval_loopButton->setIcon(QIcon("./IMAGES/reset.png"));
//   reset_eval_loopButton->setIconSize(iconSize);
//   reset_eval_loopButton->setToolTip(tr("Reset Evaluation Loop"));
//   connect(reset_eval_loopButton, SIGNAL(clicked()), this, SLOT(active_reset_eval_loop()));

  
  playButton = new QToolButton;
  playButton->setIcon( QIcon( (absolute_path + "/RENDERING/IMAGES/play.png").c_str() ) );
playButton->setIconSize(iconSize);
  playButton->setToolTip(tr("Run Sim"));
  connect(playButton, SIGNAL(clicked()), this, SLOT(active_play()));

  oneStepButton = new QToolButton;
  oneStepButton->setIcon(QIcon( (absolute_path + "/RENDERING/IMAGES/oneStep.png").c_str() ) );
  oneStepButton->setIconSize(iconSize);
  oneStepButton->setToolTip(tr("Step-by-Step"));
  connect(oneStepButton, SIGNAL(clicked()), this, SLOT(active_advance()));
  
  pauseButton = new QToolButton;
  //pauseButton->setCheckable(true);
  pauseButton->setIcon(QIcon( ( absolute_path + "/RENDERING/IMAGES/pause.png") .c_str() ) );
  pauseButton->setIconSize(iconSize);
  pauseButton->setToolTip(tr("Pause"));
  connect(pauseButton, SIGNAL(clicked(bool)), this, SLOT(active_pause()));
  
  stopButton = new QToolButton;
  stopButton->setIcon(QIcon( ( absolute_path + "/RENDERING/IMAGES/stop.png") .c_str() ) );
  stopButton->setIconSize(iconSize);
  stopButton->setToolTip(tr("Stop"));
  connect(stopButton, SIGNAL(clicked()), this, SLOT(active_stop()));
  
  quitButton = new QToolButton;
  quitButton->setIcon(QIcon( ( absolute_path + "/RENDERING/IMAGES/quit.png") .c_str() ) );
  quitButton->setIconSize(iconSize);
  quitButton->setToolTip(tr("Quit"));
  connect(quitButton, SIGNAL(clicked()), this, SLOT(active_quit()));
  
  recButton = new QToolButton;
  recButton->setIcon(QIcon( ( absolute_path + "/RENDERING/IMAGES/rec.png") .c_str() ) );
  recButton->setIconSize(iconSize);
  recButton->setToolTip(tr("Record"));
  connect(recButton, SIGNAL(clicked()), this, SLOT(active_rec()));
  
  stopRecButton = new QToolButton;
  stopRecButton->setIcon(QIcon( ( absolute_path + "/RENDERING/IMAGES/stop.png") .c_str() ) );
  stopRecButton->setIconSize(iconSize);
  stopRecButton->setToolTip(tr("Stop record"));
  connect(stopRecButton, SIGNAL(clicked()), this, SLOT(active_stop_rec()));

  QSpinBox *integerspinBox = new QSpinBox;
  integerspinBox->setRange (0, 50 );
  integerspinBox->setSingleStep(1);
  integerspinBox->setValue (0);  
  connect(integerspinBox, SIGNAL(valueChanged(int)), this, SLOT(setGenotypeNumber(int )));

  QSpinBox *frameRateBox = new QSpinBox;
  frameRateBox->setRange (5, 50 );
  frameRateBox->setSingleStep(1);
  frameRateBox->setValue (frameRate);
  connect(frameRateBox, SIGNAL(valueChanged(int)), this, SLOT(setFrameRate(int)));

  buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();
  buttonsLayout->addWidget(openButton);
  buttonsLayout->addWidget(integerspinBox);
  //buttonsLayout->addWidget(reset_eval_loopButton);
  buttonsLayout->addWidget(playButton);
  buttonsLayout->addWidget(oneStepButton);
  buttonsLayout->addWidget(pauseButton);
  buttonsLayout->addWidget(stopButton);
  buttonsLayout->addWidget(quitButton);
  buttonsLayout->addWidget(frameRateBox);
  buttonsLayout->addWidget(recButton);
  buttonsLayout->addWidget(stopRecButton);
  buttonsLayout->addStretch();
}

void  Interface::setGenotypeNumber(int id){
  //if( !ba.isEmpty() ){
    int num_groups = 0;
    vector < vector <chromosome_type> > genes;
    upload_genome_from_file <chromosome_type> ( ba.data(), genes, &num_groups );
    exp->from_genome_to_controllers( genes, id );
    
    //printf(" genotype n %d in file =%s \n",id,ba.data());
    //}
    //else {
    //cerr << " In Interface::setGenotypeNumber :: Genotype file name is missing " << endl;
    //exit(-1);
    //}
  
  exp->init_evaluations_loop( );
  exp->init_single_evaluation( );
  
  flag_activate_loadGeno_buttons       = false;
  //flag_activate_reset_eval_loop_botton = true;
  flag_activate_play_buttons           = true;
  flag_activate_step_buttons           = true;
  flag_activate_pause_buttons          = false;
  flag_activate_next_buttons           = false;
  flag_activate_quit_buttons           = true;
  updateButtons();
}

void  Interface::setFrameRate(int n){
  frameRate = n;
}

void Interface::active_play(){

  flag_activate_loadGeno_buttons       = false;
  //flag_activate_reset_eval_loop_botton = false;
  flag_activate_play_buttons           = false;
  flag_activate_step_buttons           = false;
  flag_activate_pause_buttons          = true;
  flag_activate_next_buttons           = true;
  flag_activate_quit_buttons           = true;
  updateButtons();
  m_engine->play();
}

void Interface::active_advance(){
  flag_activate_loadGeno_buttons       = false;
  //flag_activate_reset_eval_loop_botton = false;
  flag_activate_play_buttons           = true;
  flag_activate_step_buttons           = true;
  flag_activate_pause_buttons          = false;
  flag_activate_next_buttons           = true;
  flag_activate_quit_buttons           = true;
  updateButtons();
  m_engine->advance();
}

void Interface::active_pause(){
  flag_activate_loadGeno_buttons       = false; 
  //flag_activate_reset_eval_loop_botton = false;
  flag_activate_play_buttons           = true;
  flag_activate_step_buttons           = true;
  flag_activate_pause_buttons          = false;
  flag_activate_next_buttons           = true;
  flag_activate_quit_buttons           = true;
  updateButtons();
  m_engine->pause();
}

void Interface::active_stop(){
  flag_activate_loadGeno_buttons       = true;
  //flag_activate_reset_eval_loop_botton = true;
  flag_activate_play_buttons           = true;
  flag_activate_step_buttons           = true;
  flag_activate_pause_buttons          = false;
  flag_activate_next_buttons           = false;
  flag_activate_quit_buttons           = true;
  updateButtons();
  m_engine->stop();
}

void Interface::active_quit(){
  /*
    if(video->isRecording())
    video->StopRec();
  */
  exit(0);
}

void Interface::active_rec(){
  flag_activate_rec_buttons           = false;
  flag_activate_stop_rec_buttons           = true;
  updateButtons();
  //video->StartRec((int)viewer->width(), (int)viewer->height(), (float)frameRate);
}
void Interface::active_stop_rec(){
  flag_activate_rec_buttons           = true;
  flag_activate_stop_rec_buttons           = false;
  updateButtons();
  //video->StopRec();
}

void Interface::changeWindowTitle()
{
  setWindowTitle(windowTitle + " - Eval: " + QString::number( exp->getEvaluation() )
		 + " - Iter: " + QString::number( m_iteration /*exp->getIteration()*/ )
		 );
}

// void Interface::active_reset_eval_loop(){
//   flag_activate_loadGeno_buttons       = true;
//   flag_activate_reset_eval_loop_botton = false;
//   flag_activate_play_buttons           = true;
//   flag_activate_step_buttons           = true;
//   flag_activate_pause_buttons          = false;
//   flag_activate_next_buttons           = false;
//   flag_activate_quit_buttons           = true;
//   updateButtons();
//   exp->init_evaluations_loop( );
// }
#endif
