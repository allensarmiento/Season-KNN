#ifndef UTILITY_H
#define UTILITY_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structs.h"

double largestDistance(double *distances, int length) {
  double largest = -99;
  for (int i = 0; i < length; i++) {
    if (distances[i] > largest)
      largest = distances[i];
  }
  return largest;
}

Neighbors addValue(Neighbors n, double distance, int month) {
  for (int i = n.position - 1; i >= 0; i--) {
    if (distance < n.distances[i]) {
      n.distances[i] = distance;
      n.months[i] = month;
      break;
    }
  }
  return n;
}

void printNeighbors(Neighbors n) {
  for (int i = 0; i < n.position; i++) {
    printf("Months: %d, Distance: %f\n", n.months[i], n.distances[i]);
  }
}

// Evaluates which data is not used in the training data and sets those
// as testing data.
Weather* testData(int portion, Weather* data) {
  Weather *test = (Weather *)malloc(sizeof(Weather) * portion);

  int count = 0;
  for (int i = ARRAYSIZE - 1; i >= portion; i--) {
    test[count].month = data[i].month;  
    test[count].temp = data[i].temp;
    test[count].slp = data[i].slp;
    count++;
  }

  return test;
}

// Takes in a percentage of the weather data as training data at random.
Weather* trainData(int portion, Weather* data) {
  Weather *train = (Weather *)malloc(sizeof(Weather) * portion);

  for (int i = 0; i < portion; i++) {
    train[i].month = data[i].month;  
    train[i].temp = data[i].temp;
    train[i].slp = data[i].slp;
  }

  return train;
}

// Calculate the euclidean distance between two points.
float euclideanDistance(float x1, float y1, float x2, float y2) {
  return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}

// Calculate accuracy
void calculateStats(int TP, int TN, int FP, int FN) {
  double accuracy = (TP + TN) / ((TP + TN + FP + FN) * 1.0);
  printf("Accuracy: %f\n", accuracy);

  double error_rate = (FP + FN) / ((TP + TN + FP + FN) * 1.0);
  printf("Error rate: %f\n", error_rate);

  double sensitivity = TP / ((TP + FN) * 1.0);
  printf("Sensitivity: %f\n", sensitivity);

  double specificity = TN / ((FP + TN) * 1.0); 
  printf("Specificity: %f\n", specificity);

  double precision = TP / ((TP + FP) * 1.0);
  printf("Precision: %f\n", precision);

  double recall = TP / ((TP + FN) * 1.0);
  printf("Recall: %f\n", recall);

  printf("\n");
}

#endif
