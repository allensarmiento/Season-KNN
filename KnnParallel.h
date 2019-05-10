#ifndef KNNPARALLEL_H
#define KNNPARALLEL_H
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include "structs.h"
#include "utility.h"

void sortParallel(int *months, double *distances, int size) {
  for (int i = 0; i < size-1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (distances[j] > distances[j+1]) {
        double temp_dist = distances[j];
        distances[j] = distances[i];
        distances[i] = temp_dist;

        int temp_month = months[j];
        months[j] = months[i];
        months[i] = temp_month;
      }
    }
  }
}

Neighbors getNeighbors(Weather* traindata, int trainlen, 
    Weather testInstance, int k) {
  
  Neighbors n;
  n.months = (int *)malloc(sizeof(int)*k);
  n.distances = (double *)malloc(sizeof(double)*k);
  n.position = 0;
  int dist, i;
  int bestMax = -1;

  int *months = (int *)malloc(sizeof(int) * trainlen);
  double *distances = (double *)malloc(sizeof(double) * trainlen);

  #pragma omp parallel shared(months, distances) private(dist, i)
  {
    #pragma omp for schedule(static, 4)
      for (i = 0; i < trainlen; i++) {
        dist = euclideanDistance(traindata[i].temp, traindata[i].slp,
            testInstance.temp, testInstance.slp);
        
        distances[i] = dist;
        months[i] = traindata[i].month;
        
        //printf("Added distance of %f with month %d\n", distances[i], months[i]);
      } // end parallel for
  } // end parallel construct

  // Sort the values
  sortParallel(months, distances, trainlen);

  // Add the top knn values in Neighbors
  //printf("Best distances: ");
  for (int i = 0; i < k; i++) {
    n.months[i] = months[i];
    n.distances[i] = distances[i];
    n.position++;
   // printf("%f ", n.distances[i]);
  }
  //printf("\n");

  free(months);
  free(distances);

  return n;
}

void knnParallel(int k, Weather* traindata, int trainlen,
    Weather* testdata, int testlen) {
  int correct = 0, incorrect = 0;
  int winterCount, summerCount, prediction;
  int i, j;

  #pragma omp for schedule(static, 4)
    for (i = 0; i < testlen; ++i) {
      winterCount = 0;
      summerCount = 0;

      Neighbors n = getNeighbors(traindata, trainlen, testdata[i], k);

      for (j = 0; j < n.position; ++j) {
        if (n.months[j] == 1) {
          winterCount++;
        } else if (n.months[j] == 7) {
          summerCount++;
        }
      }

      if (winterCount >= summerCount) {
        prediction = 1;
      } else {
        prediction = 7;
      }

      if (prediction == testdata[i].month) {
        //printf("%d. Correct prediction\n", i);
        correct++;
      } else {
        //printf("%d. Incorrect prediction\n", i);
        incorrect++;
      }

      free(n.distances);
      free(n.months);
    }

  printf("Correct: %d\n", correct);
  printf("Incorrect: %d\n", incorrect);
}

#endif
