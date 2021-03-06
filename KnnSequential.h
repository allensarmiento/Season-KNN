#ifndef KNNSEQUENTIAL_H
#define KNNSEQUENTIAL_H
#include <stdio.h>
#include "utility.h"
#include "structs.h"

Neighbors sort(Neighbors n) {
  for (int i = 0; i < n.position - 1; i++) {
    for (int j = 1; j < n.position - i - 1; j++) {
      if (n.distances[j] > n.distances[i]) {
        double temp_dist = n.distances[j];
        n.distances[j] = n.distances[i];
        n.distances[i] = temp_dist;

        int temp_month = n.months[j];
        n.months[j] = n.months[i];
        n.months[i] = temp_month;
      }
    }
  }

  return n;
}

// Takes in the test data, points, and centroids to make predictions.
// knn - how many points to compare to
// traindata - training data
// trainlength - length of training data
// testdata - test data
// testlength - length of testing data
void knnSequential(int knn, Weather* traindata, int trainlength, Weather* testdata, int testlength) {
  int correct = 0;
  int incorrect = 0;
  Neighbors n;

  for (int i = 0; i < testlength; i++) {
    // Comparing only winter and summer
    if (testdata[i].month != 1 && testdata[i].month != 7)
      continue;

    n.months = (int*)malloc(sizeof(int) * knn);
    n.distances = (double*)malloc(sizeof(double) * knn);
    n.position = 0;
    double best_max = -99; // best maximum value in array

    for (int j = 0; j < trainlength; j++) {
      // Comparing only winter and summer
      if (traindata[j].month != 1 && traindata[j].month != 7) {
        continue;
      }

      double distance = euclideanDistance(traindata[j].temp, traindata[j].slp, 
                                          testdata[i].temp, testdata[i].slp);

      if (best_max == -99 || distance < best_max) {
        if (n.position >= knn) {
          // Find position to replace
          n = addValue(n, distance, traindata[j].month);
        } else {
          n.distances[n.position] = distance;
          n.months[n.position] = traindata[j].month;
        }
        best_max = largestDistance(n.distances, n.position);
        n = sort(n);
        if (n.position < knn) {
          n.position++;
        }
      }
    }

    int summer_count = 0;
    int winter_count = 0;
    int prediction;
    for (int i = 0; i < n.position; i++) {
      if (n.months[i] == 1) winter_count++;
      else if (n.months[i] == 7) summer_count++;
    }

    if (winter_count >= summer_count) {
      prediction = 1; 
    } else {
      prediction = 7;
    }

    //printf("Predicted: %d, Actual: %d\n", prediction, testdata[i].month);
    if (prediction == testdata[i].month) {
      correct++;
    } else {
      incorrect++;
    }
  }

  free(n.months);
  free(n.distances);

  printf("Correct: %d\n", correct);
  printf("Incorrect: %d\n", incorrect);
}

#endif
