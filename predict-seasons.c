#include <stdio.h>
#include <time.h>
#include "utility.h"
#include "structs.h"
#include "KnnParallel.h"
#include "KnnSequential.h"

int main() {
  int* confMatrix;

  // Data generation
  char filename[] = "./data/socal_weather.csv";
  printf("Generating dataset...\n");
  Weather* data = generateData(filename);
  printf("Dataset created.\n\n");

  // Training and test data partition
  printf("Generating training data of 75 percent...\n");
  int trainLength = 311;
  Weather *train = trainData(trainLength, data);
  printf("Training data created.\n\n");

  printf("Generating testing data of 25 percent...\n");
  int testLength = 104;
  Weather *test = testData(testLength, data);
  printf("Testing data created.\n\n");
  
  // Sequential KNN execution
  clock_t t;
  double timeTaken;
  printf("Executing sequential knn...\n");
  t = clock();
  knnSequential(3, train, trainLength, test, testLength);
  t = clock() - t;
  timeTaken = ((double)t) / CLOCKS_PER_SEC;
  printf("Sequential knn complete.\n");
  printf("Sequential knn execution time: %f\n\n", timeTaken);

  // Parallel KNN execution
  printf("Executing parallel knn...\n");
  t = clock();
  confMatrix = knnParallel(3, train, trainLength, test, testLength);
  t = clock() - t;
  timeTaken = ((double)t) / CLOCKS_PER_SEC;
  printf("Correct: %d\n", confMatrix[0] + confMatrix[1]);
  printf("Incorrect: %d\n", confMatrix[2] + confMatrix[3]);
  printf("Parallel knn complete.\n");
  printf("Parallel knn execution time: %f\n\n", timeTaken);
  // Evaluate performance of KNN Parallel
  evaluatePerformance(confMatrix);
  free(confMatrix);

  // Garbage clean
  free(data);
  free(train);
  free(test);

  return 0;
}
