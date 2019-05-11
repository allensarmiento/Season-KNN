#ifndef KNNPARALLEL_H
#define KNNPARALLEL_H
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include "structs.h"
#include "utility.h"

void sortParallel(Distance* d, int size) {
  int i, j;
  Distance temp;
  for (i = 0; i < size-1; ++i) {
    for (j = 0; j < size-i-1; ++j) {
      if (d[j].distance > d[i].distance) {
        temp = d[j];
        d[j] = d[i];
        d[i] = temp;
      }
    }
  }
}

// Retrieves the closest k neighbors from the training data.
Distance* getNeighbors(int k, Weather* trainData,
    int trainLen, Weather testInstance) {
  Distance* distances = (Distance *)malloc(sizeof(Distance)*trainLen);
  Distance* neighbors = (Distance *)malloc(sizeof(Distance)*k);
  int i;

  #pragma omp parallel shared(trainData, testInstance) private(i) 
  {
    #pragma omp for schedule(auto)
    for (i = 0; i < trainLen; ++i) {
      distances[i].distance = euclideanDistance(trainData[i].temp, trainData[i].slp,
          testInstance.temp, testInstance.slp);
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

void knnParallel(int k, Weather* trainData, int trainLen,
    Weather* testData, int testLen) {
  int i, j, winterCount, summerCount;
  int correct = 0, incorrect = 0;
  Distance* neighbor;

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

    if (winterCount > summerCount && testData[i].month == 1) {
      correct++;
    } else if (winterCount < summerCount && testData[i].month == 7) {
      correct++;
    } else {
      incorrect++;
    }
    free(neighbor);
  }

  printf("Correct: %d\n", correct);
  printf("Incorrect: %d\n", incorrect);
}

#endif
