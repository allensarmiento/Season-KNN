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

/*void sort(int *months, double *distances, int size) {
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
}*/

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

#endif
