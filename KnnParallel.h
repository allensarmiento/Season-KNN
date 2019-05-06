#ifndef KNNPARALLEL_H
#define KNNPARALLEL_H
#include <stdio.h>
#include <omp.h>
#include <math.h>
#include "structs.h"
#include "utility.h"

Neighbors getNeighbors(Weather* traindata, int trainlen, 
    Weather testInstance, int k) {
  
  Neighbors n;
  n.months = (int *)malloc(sizeof(int)*k);
  n.distances = (double *)malloc(sizeof(double)*k);
  n.position = 0;
  int dist, i;
  int bestMax = -1;

  #pragma omp parallel shared(n, bestMax) private(dist, i)
  {
    #pragma omp for schedule(static, 4)
      for (i = 0; i < trainlen; ++i) {
        if (traindata[i].month == 1 || traindata[i].month == 7) {
          dist = euclideanDistance(traindata[i].temp, traindata[i].slp,
              testInstance.temp, testInstance.slp);

          #pragma omp critical
          {
            if (bestMax == -1 || dist < bestMax) {
              if (n.position >= k) {
                n = addValue(n, dist, traindata[i].month);
              } else {
                n.distances[n.position] = dist;
                n.months[n.position] = traindata[i].month;
              }

              bestMax = largestDistance(n.distances, k);
              n = sort(n);

              if (n.position < k) {
                n.position++;
              }
            }
          }
        }
      } // end parallel for
  } // end parallel construct

  return n;
}

void knnParallel(int k, Weather* traindata, int trainlen,
    Weather* testdata, int testlen) {
  int correct = 0, incorrect = 0;
  int winterCount, summerCount, prediction;

  for (int i = 0; i < testlen; ++i) {
    if (testdata[i].month == 1 || testdata[i].month == 7) {
      winterCount = 0;
      summerCount = 0;

      Neighbors n = getNeighbors(traindata, trainlen, testdata[i], k);

      for (int j = 0; j < n.position; ++j) {
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
        correct++;
      } else {
        incorrect++;
      }

      free(n.distances);
      free(n.months);
    }
  }

  printf("Correct: %d\n", correct);
  printf("Incorrect: %d\n", incorrect);
}



/*
void knnParallel(int numNeighbors, Weather* traindata, int trainlen, 
    Weather* testdata, int testlen) {
  int correct = 0;
  int incorrect = 0;
  Neighbors n;
  int i, j, k;

  #pragma omp parallel shared(correct, incorrect) private(i, j, k)
  {
    for (i = 0; i < testlen; ++i) {
      if (testdata[i].month == 1 || testdata[i].month == 7) {
        n.months = (int *)malloc(sizeof(int)*numNeighbors);
        n.distances = (double *)malloc(sizeof(double)*numNeighbors);
        n.position = 0;
        double bestMax = -99;

        #pragma omp for schedule(static, 4)
        for (j = 0; j<trainlen; ++j) {
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
        for (k = 0; k < n.position; ++k) {
          if (n.months[k] == 1) {
            winterCount++;
          } else if (n.months[k] == 7) {
            summerCount++;
          }
        }

        if (winterCount > summerCount) {
          prediction = 1;
        } else {
          prediction = 7;
        }

        //printf("Predicted: %d, Actual: %d\n", prediction, testdata[i].month);
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
*/






#endif
