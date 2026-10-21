#include "file.h"
#include "forward.h"
#include "images.h"
#include "layers.h"
#include "mnist_dataset.h"
#include "network.h"
#include "print.h"
#include "random.h"
#include "server.h"
#include "training.h"

int testingPrint ()
{
  printUnsignedInteger (truncateInteger(452.750));
  print ("\n");

  printDouble (truncateFractional(45002.950));
  print ("\n");

  printUnsignedInteger (truncateInteger(1000 * truncateFractional(45002.950)));
  print ("\n");

  printUnsignedInteger (roundDouble(1000 * truncateFractional(45002.950)));
  print ("\n");

  printUnsignedInteger (roundDouble(0.4));
  print ("\n");

  printDouble (48.541);
  print ("\n"); 

  printDouble (-0.541);
  print ("\n"); 

  printDouble (-45002.950);
  print ("\n");

  printDouble ((100*truncateFractional(-45002.950)));
  print ("\n"); 

  double test = truncateFractional(45002.950);
  printInteger ( * ((long *) (& test)));
  print ("\n"); 
  
  printDouble (0.000005);
  print ("\n"); 

  printDouble (0.123456789);
  print ("\n"); 
  
  printDouble (- 0.123455);
  print ("\n"); 

  printDouble (roundDouble(- 0.5));
  print ("\n"); 
  
  printDouble (truncateFractional(-1.2401943));
  print ("\n");

  union
  {
    double value;
    unsigned long bits;
  }
  x_union;
  x_union.bits = 0x43F0000000000000;
  printDouble (x_union.value);
  print ("\n");

  x_union.bits = 0x7FF0000000000000;
  printDouble (x_union.value);
  print ("\n");

  x_union.bits = 0x7FF0000000000001;
  printDouble (x_union.value);
  print ("\n");

  x_union.bits = 0xFFF0000000000000;
  printDouble (x_union.value);
  print ("\n");

  x_union.bits = 0xFFF0000000000001;
  printDouble (x_union.value);
  print ("\n");

  print ("This is a final test showcasing the print.\n");
  
  return 0;
}

int testingRandom ()
{
  struct xorshift64_state state;
  xorshift64Initialization (& state);
  int bin = 100;
  int result[bin];
  for (int i = 0; i < bin; i++) {
        result[i] = 0;
  }
  int index;
  int n = 1000000;
  double value;
  for (int i = 0; i < n; i++)
  {
    value = ((float) xorshift64(& state)) / ((float) 0xFFFFFFFFFFFFFFFF);
    index = (int) (bin * value);
    result[index]++;
  }
  for (int i = 0; i < bin; i++)
  {
    printInteger (result[i]);
    print ("\n");
  }
  for (int i = 0; i < 10; i++)
  {
    printDouble (normal(& state));
    print ("\n");
  }
  return 0;
}

int testingNormal ()
{
  struct xorshift64_state state;
  xorshift64Initialization (& state);
  int sample_number = 10000;
  int star_number = 100;
  int distribution[10];
  for (int i = 0; i < 10; i++)
  {
    distribution[i] = 0;
  }
  for (int i = 0; i < sample_number; i++)
  {
    double number = 5.0 + 2.0 * normal (& state);
    if ((number >= 0.0) && (number < 10.0))
    {
      distribution[(int) number]++;
    }
  }
  for (int i = 0; i < 10; i++)
  {
    printUnsignedInteger (i);
    print (" to");
    if (i < 9)
    {
      print (" ");
    }
    print (" ");
    printUnsignedInteger (i + 1);
    print (": ");
    for (int j = 0; j < distribution[i] * star_number / sample_number; j++)
    {
      print ("*");
    }
    print ("\n");
  }
}

int testingNormalBis ()
{
  struct xorshift64_state state;
  xorshift64Initialization (& state);
  int sample_number = 100;
  for (int i = 0; i < sample_number; i++)
  {
    printDouble (normal(& state));
    print ("\n");
  } //*/
  //printDouble(squareRoot(78.79325, 1e-15));
  //print("\n");
}

int testingMNISTData ()
{
  dataset2D X_train;
  dataset1D y_train;
  readImages ("/code/data/train-images-idx3-ubyte", & X_train);
  readLabels ("/code/data/train-labels-idx1-ubyte", & y_train);
  normalizeImages (& X_train);
  resizeImages (& X_train, 32, 32);
  denormalizeImages (& X_train);
  print ("The first image is:\n");
  displayImage (& X_train, 0, 0);
  print ("\nThe last image is:\n");
  displayImage (& X_train, 59999, 0);
  print ("\n");
  free (X_train.samples);
  free (y_train.samples);
}

int testingConvolution ()
{
  dataset2D input;
  dataset2D output;
  convolutionalLayer layer;
  input.samples = (float * ) malloc (sizeof(float) * 2 * 6 * 6 * 1);
  for (int i = 0; i < 2 * 6 * 6 * 1; i++)
  {
    input.samples[i] = i;
  }
  input.channel_count = 1;
  input.column_count = 6;
  input.row_count = 6;
  input.sample_count = 2;
  initializeConvolutionalLayer (& layer, 1, 3, 3, 2, 1, rectifiedLinearUnit, rectifiedLinearUnitPrime);
  for (int i = 0; i < 3 * 3 * 1 * 3; i++)
  {
    layer.filters[i] = i;
  }
  convolve (& layer, & input, & output);
  displayImage (& input, 0, 0);
  displayImage (& output, 0, 0);
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      printDouble (layer.filters[i * 3 * 3 * 1 + j * 3 * 1 + 0]);
      print (" ");
    }
    print ("\n");
  }
}

int testingForwardBackward ()
{
  dataset2D X_train;
  dataset1D y_train;

  float learning_rate;
  LeNet5 network;
  dataset1D labels;
  dataset2D input_2d_1;
  dataset2D input_2d_2;
  dataset2D input_2d_3;
  dataset2D input_2d_4;
  dataset2D input_2d_5;
  dataset1D input_1d_1;
  dataset1D input_1d_2;
  dataset1D input_1d_3;
  dataset1D output;
  dataset2D input_2d_1_gradients; 
  dataset2D input_2d_2_gradients; 
  dataset2D input_2d_3_gradients; 
  dataset2D input_2d_4_gradients; 
  dataset2D input_2d_5_gradients;
  dataset1D input_1d_1_gradients; 
  dataset1D input_1d_2_gradients; 
  dataset1D input_1d_3_gradients; 
  dataset1D output_gradients;

  learning_rate = 0.01;
  network.c1;
  initializeLeNet5 (& network, 0);
  readLabels ("/code/data/train-labels-idx1-ubyte", & y_train);
  readImages ("/code/data/train-images-idx3-ubyte", & X_train);
  batchLabels (32, 0, & y_train, & labels);
  batchImages (32, 0, & X_train, & input_2d_1);

  forwardLeNet5 (& network, & input_2d_1, & input_2d_2, & input_2d_3, & input_2d_4, & input_2d_5, & input_1d_1, & input_1d_2, & input_1d_3, & output);
  for (int i = 0; i < 32; i++)
  {
    print ("For the ");
    printUnsignedInteger (i);
    print ("th image, the prediction is:\n");
    for (int j = 0; j < 10; j++)
    {
      printDouble (output.samples[i * 10 + j]);
      print ("\n");
    }
    print ("\n\n\n");
  }

  backwardLeNet5
  (
    learning_rate,
    & network, 
    & labels,
    & input_2d_1, 
    & input_2d_2, 
    & input_2d_3, 
    & input_2d_4, 
    & input_2d_5,
    & input_1d_1, 
    & input_1d_2, 
    & input_1d_3, 
    & output,
    & input_2d_1_gradients, 
    & input_2d_2_gradients, 
    & input_2d_3_gradients, 
    & input_2d_4_gradients, 
    & input_2d_5_gradients,
    & input_1d_1_gradients, 
    & input_1d_2_gradients, 
    & input_1d_3_gradients, 
    & output_gradients
  );
}

int testingTrain ()
{
  float learning_rate = 0.01;
  int batch_size = 100;
  int epochs = 10;

  LeNet5 network;
  initializeLeNet5 (& network, 1);

  dataset2D X_train;
  dataset1D y_train;
  readImages ("/code/data/train-images-idx3-ubyte", & X_train);
  readLabels ("/code/data/train-labels-idx1-ubyte", & y_train);
  
  normalizeImages (& X_train);
  resizeImages (& X_train, 32, 32);

  training (learning_rate, batch_size, epochs, & network, & X_train, & y_train);

  dataset2D X_test;
  dataset1D y_test;

  readImages ("/code/data/t10k-images-idx3-ubyte", & X_test);
  readLabels ("/code/data/t10k-labels-idx1-ubyte", & y_test);

  test (& network, & X_test, & y_test);
}

int testingWrite ()
{
  const char * filename = "binary_output.bin";
  long file_descriptor;
  long bytesWritten;

  // Example binary data (array of floats)
  float data[] = {3.14f, 2.71f, 1.618f, 0.18f};
  long data_size = sizeof (data);  // Size in bytes

  // Open the file with write-only, create, and truncate flags (0644 permissions)
  openFile (filename, & file_descriptor, 0x41 /* O_WRONLY | O_CREAT */, 0644);
  
  // Write binary data to the file
  writeFile (file_descriptor, (char * ) data, data_size, & bytesWritten);

  char test_data[16];
  long bytesRead;

  closeFile (file_descriptor);

  openFile (filename, & file_descriptor, 0, 0);

  readFile (16, test_data, & file_descriptor, & bytesRead);

  for (int i = 0; i < 4; i++)
  {
    printDouble ((((float * ) & test_data))[i]);
    print ("\n");
  }

  // Close the file
  closeFile (file_descriptor);


  return 0;
}

int testingModelSaveAndLoad ()
{
  LeNet5 network_1;
  initializeLeNet5 (& network_1, 1);

  LeNet5 network_2;
  initializeLeNet5 (& network_2, 1);

  saveNetwork(& network_1, "/code/weights/network_1.data");

  printDouble(network_1.c1.filters[0]);
  print("\n");

  printDouble(network_2.c1.filters[0]);
  print("\n");

  printDouble(network_1.output.weights[10]);
  print("\n");

  printDouble(network_2.output.weights[10]);
  print("\n");

  loadNetwork(& network_2, "/code/weights/network_1.data");

  printDouble(network_2.c1.filters[0]);
  print("\n");

  printDouble(network_2.output.weights[10]);
  print("\n");

  return 0;
}

int testingTrainAndSave ()
{
  float learning_rate = 0.01;
  int batch_size = 100;
  int epochs = 10;

  LeNet5 network;
  initializeLeNet5 (& network, 1);

  dataset2D X_train;
  dataset1D y_train;
  readImages ("/code/data/train-images-idx3-ubyte", & X_train);
  readLabels ("/code/data/train-labels-idx1-ubyte", & y_train);
  
  normalizeImages (& X_train);
  resizeImages (& X_train, 32, 32);
 
  training (learning_rate, batch_size, epochs, & network, & X_train, & y_train);

  dataset2D X_test;
  dataset1D y_test;

  readImages ("/code/data/t10k-images-idx3-ubyte", & X_test);
  readLabels ("/code/data/t10k-labels-idx1-ubyte", & y_test);

  normalizeImages (& X_test);
  resizeImages (& X_test, 32, 32);

  test (& network, & X_test, & y_test);
  
  saveNetwork (& network, "/code/weights/final_weights.data");
}

int testingTest ()
{
  LeNet5 network;
  initializeLeNet5 (& network, 1); 

  loadNetwork (& network, "/code/weights/final_weights.data");

  dataset2D X_test;
  dataset1D y_test;

  readImages ("/code/data/t10k-images-idx3-ubyte", & X_test);
  readLabels ("/code/data/t10k-labels-idx1-ubyte", & y_test);

  normalizeImages (& X_test);
  resizeImages (& X_test, 32, 32);

  test (& network, & X_test, & y_test);
}

int testingServer ()
{
  start_server ();
}