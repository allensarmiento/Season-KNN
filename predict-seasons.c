#include <stdio.h>
#include <time.h>
#include "readData.h"
#include "utility.h"
#include "structs.h"
#include "KnnParallel.h"
#include "KnnSequential.h"

int main() {
  // Data generation
  char filename[] = "./data/socal_weather.csv";
  printf("Generating dataset...\n");
  Weather* data = generateData(filename);
  printf("Dataset created.\n\n");

  // Training and test data partition
  printf("Generating training data of 75 percent...\n");
  int train_length = 311;
  Weather *train = trainData(train_length, data);
  printf("Training data created.\n\n");

  printf("Generating testing data of 25 percent...\n");
  int test_length = 104;
  Weather *test = testData(test_length, data);
  printf("Testing data created.\n\n");
  
  // Sequential KNN execution
  clock_t t;
  double timeTaken;
  printf("Executing sequential knn...\n");
  t = clock();
  knnSequential(3, train, train_length, test, test_length);
  t = clock() - t;
  timeTaken = ((double)t) / CLOCKS_PER_SEC;
  printf("Sequential knn complete.\n");
  printf("Sequential knn execution time: %f\n\n", timeTaken);

  printf("Executing parallel knn...\n");
  t = clock();
  knnParallel(3, train, train_length, test, test_length);
  t = clock() - t;
  timeTaken = ((double)t) / CLOCKS_PER_SEC;
  printf("Parallel knn complete.\n");
  printf("Parallel knn execution time: %f\n\n", timeTaken);

  // Garbage clean
  free(data);
  free(train);
  free(test);

  return 0;
}
