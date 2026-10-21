#include "exit_program.h"
#include "print.h"

extern int testingPrint ();
extern int testingRandom ();
extern int testingNormal ();
extern int testingNormalBis ();
extern int testingMNISTData ();
extern int testingConvolution ();
extern int testingForwardBackward ();
extern int testingTrain ();
extern int testingWrite ();
extern int testingModelSaveAndLoad ();
extern int testingTrainAndSave ();
extern int testingTest ();
extern int testingServer ();

void startProgram ()
{
  int status = testingServer ();
  exitProgram (0);
}