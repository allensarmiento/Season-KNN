#ifndef KNNSEQUENTIALVERSION2_H
#define KNNSEQUENTIALVERSION2_H
#include <stdio.h>
#include "utility.h"
#include "structs.h"

void sortSequential(int *months, double *distances, int size) {
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

// Takes in the test data, points, and centroids to make predictions.
// knn - how many points to compare to
// traindata - training data
// trainlength - length of training data
// testdata - test data
// testlength - length of testing data
void knnSequentialVersion2(int knn, Weather* traindata, int trainlength, Weather* testdata, int testlength) {
  int summer_positive = 0, summer_negative = 0,
      winter_positive = 0, winter_negative = 0;
  int winter_count = 0, summer_count = 0;

  Neighbors n;

  int *months = (int *)malloc(sizeof(int) * trainlength);
  double *distances = (double *)malloc(sizeof(double) * trainlength);

  for (int i = 0; i < testlength; i++) {
    printf("Iteration %d\n", i);
    winter_count = 0;
    summer_count = 0;

    for (int j = 0; j < trainlength; j++) {
      double distance = euclideanDistance(traindata[j].temp, traindata[j].slp, 
                                          testdata[i].temp, testdata[i].slp);

      distances[j] = distance;
      months[j] = traindata[j].month;
    }

    // Sort the values
    sortSequential(months, distances, trainlength);

    n.months = (int *)malloc(sizeof(int) * knn);
    n.distances = (double *)malloc(sizeof(double) * knn);
    n.position = 0;
    // Add the top knn values in Neighbors
    for (int j = 0; j < knn; j++) {
      n.months[j] = months[j];
      n.distances[j] = distances[j];
      n.position++;
    }

    free(months);
    free(distances);

    for (int j = 0; j < n.position; j++) {
      if (n.months[j] == 1) {
        winter_count++;
      } else if (n.months[j] == 7) {
        summer_count++;
      }
    }

    int prediction;
    if (winter_count >= summer_count) {
      prediction = 1;
    } else {
      prediction = 7;
    }

    // Counting number of true positives/negatives and false positives/negatives
    if (prediction == 1) {
      if (testdata[i].month == prediction) {
        winter_positive++;
      } else {
        winter_negative++;
      }
    } else if(prediction == 7) {
      if (testdata[i].month == prediction) {
        summer_positive++;
      } else {
        summer_negative++;
      }
    }

    free(n.months);
    free(n.distances);
  }


  printf("Correct: %d\n", winter_positive + summer_positive);
  printf("Incorrect: %d\n\n", winter_negative + summer_negative);
  
  // Printing stats, since the predicitions are only summer and winter,
  // and not yes or no, we might not need to consider some of the stats
  calculateStats(winter_positive, summer_positive, winter_negative, summer_negative);
}

#endif
