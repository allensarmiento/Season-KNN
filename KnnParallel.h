#ifndef KNNPARALLEL_H
#define KNNPARALLEL_H
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include "structs.h"
#include "utility.h"

// Retrieves the closest k neighbors from the training data. Returns
// an array of Distance structs of those neighbors.
Distance* getNeighbors(int k, Weather* trainData,
    int trainLen, Weather testInstance) {
  Distance* distances = (Distance *)malloc(sizeof(Distance)*trainLen);
  Distance* neighbors = (Distance *)malloc(sizeof(Distance)*k);
  int i;

  #pragma omp parallel shared(trainData, testInstance) private(i) 
  {
    #pragma omp for schedule(auto)
    for (i = 0; i < trainLen; ++i) {
      distances[i].distance = euclideanDistance(trainData[i].temp,
          trainData[i].slp, testInstance.temp, testInstance.slp);
      distances[i].month = trainData[i].month;
    }
  }

  sortParallel(distances, trainLen);

  for (i = 0; i < k; ++i) {
    neighbors[i] = distances[i];
  }

  free(distances);
  return neighbors;
}

// Main driver for the K-Nearest Neighbors function. Compares each test
// instance with the nearest neighbors, according to Euclidean distance,
// and returns a confusion matrix. The confusion matrix has the format:
// [summerPositive, summerNegative, winterPostive, winterNegative].
int* knnParallel(int k, Weather* trainData, int trainLen,
    Weather* testData, int testLen) {
  int i, j, winterCount, summerCount;
  int* confMatrix = (int *)malloc(sizeof(int)*4);
  Distance* neighbor;

  for (i = 0; i < 4; ++i) { // Init confusion matrix
    confMatrix[i] = 0;
  }

  for (i = 0; i < testLen; ++i) {
    winterCount = 0;
    summerCount = 0;
    neighbor = getNeighbors(k, trainData, trainLen, testData[i]);

    for (j = 0; j < k; ++j) {
      if (neighbor[j].month == 1) {
        winterCount++;
      } else {
        summerCount++;
      }
    }

    if (winterCount >= summerCount && testData[i].month == 1) {
      confMatrix[0]++; // True positive
    } else if (winterCount >= summerCount && testData[i].month == 7) {
      confMatrix[3]++; // False negative
    } else if (summerCount > winterCount && testData[i].month == 7) {
      confMatrix[1]++; // True negative
    } else if (summerCount > winterCount && testData[i].month == 1) {
      confMatrix[2]++; // False negative
    }
    free(neighbor);
  }

  return confMatrix;
}

#endif
