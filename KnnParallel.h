#ifndef KNNPARALLEL_H
#define KNNPARALLEL_H
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include "structs.h"
#include "utility.h"

void knnParallel(int numNeighbors, Weather* traindata, int trainlen, 
    Weather* testdata, int testlen) {
  int correct = 0;
  int incorrect = 0;
  Neighbors n;

  #pragma omp parallel shared(correct, incorrect)
  {
    for (int i=0; i < testlen; ++i) {
      if (testdata[i].month == 1 || testdata[i].month == 7) {
        n.months = (int *)malloc(sizeof(int)*numNeighbors);
        n.distances = (double *)malloc(sizeof(double)*numNeighbors);
        n.position = 0;
        double bestMax = -99;

        #pragma omp for
        for (int j=0; j<trainlen; ++j) {
          // compare only winter and summer
          if (traindata[j].month == 1 || traindata[j].month == 7) {
            double dist = euclideanDistance(traindata[j].temp, traindata[j].slp,
                testdata[i].temp, testdata[i].slp);

            if (bestMax == -99 || dist < bestMax) {
              if (n.position >= numNeighbors) {
                // find positoin to replace
                n = addValue(n, dist, traindata[j].month);
              } else {
                n.distances[n.position] = dist;
                n.months[n.position] = traindata[j].month;
              }
              bestMax = largestDistance(n.distances, n.position);
              n = sort(n);
              if (n.position < numNeighbors) {
                n.position++;
              }
            }
          }
        } // end parallel for

        int summerCount = 0;
        int winterCount = 0;
        int prediction;
        for (int k = 0; k < n.position; ++k) {
          if (n.months[i] == 1) {
            winterCount++;
          } else if (n.months[i] == 7) {
            summerCount++;
          }
        }

        if (winterCount > summerCount) {
          prediction = 1;
        } else {
          prediction = 7;
        }

        printf("Predicted: %d, Actual: %d\n", prediction, testdata[i].month);
        #pragma omp critical
        if (prediction == testdata[i].month) {
          correct++;
        } else {
          incorrect++;
        }
      }
    }
  } // end parallel construct

}

#endif
