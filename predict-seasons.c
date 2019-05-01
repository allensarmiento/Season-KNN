#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define ARRAYSIZE 3321

// Weather struct reprsenting a single entry from the dataset.
//    month - month data column
//    temp - mean temperature column
//    slp - sea level pressure column 
typedef struct Weather {
  int month;
  float temp;
  float slp;
} Weather;

// Neighbors struct representing the neighbors of a Weather datapoint.
//    months - dynamic array of months, parallel array with distances
//    distances - dynamic array of distances, parallel array with months
//    position - the number of values in the array
typedef struct Neighbors {
  int *months;
  double *distances;
  int position;
} Neighbors;

// Take in a csv filename
// Return a pointer to an array of weather objects
Weather* generateData(char fname[]) {
  char* token;
  char line[30];
  const char delim[2] = ",";
  int i = 0;

  // allocate mem to hold weather data
  Weather* weatherData = (Weather *)malloc(sizeof(Weather)*ARRAYSIZE);

  FILE* instream = fopen(fname, "r");

  // Retrieve each line until eof. Each line should contain no
  // more than 30 characters.
  while (fgets(line, 30, instream)) {
    Weather w;
    token = strtok(line, delim);
    w.month = atoi(token);
    token = strtok(NULL, delim);
    w.temp = atof(token);
    token = strtok(NULL, delim);
    w.slp = atof(token);
    weatherData[i] = w;
    i++;
  }
  fclose(instream);
  return weatherData;
}

// Partition the Weather data for training.
// int portion - Partition of data to consider as training data
// Weather* data - The entire text file stored in a Weather object
Weather* trainData(int portion, Weather* data) {
  Weather *train = (Weather *)malloc(sizeof(Weather) * portion);

  for (int i = 0; i < portion; i++) {
    train[i].month = data[i].month;  
    train[i].temp = data[i].temp;
    train[i].slp = data[i].slp;
  }

  return train;
}

// Partition the Weather data for testing.
// int portion - Partition of data to consider as testing data
// Weather* data - The entire text file stored in a Weather object
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

// Calculate the euclidean distance between two points.
float euclideanDistance(float x1, float y1, float x2, float y2) {
  return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}

// Return the largest distance in the distances array.
// double* distances - Distances array in the Neighbors object
// int length - size of the distances array
double largestDistance(double *distances, int length) {
  double largest = -99;
  for (int i = 0; i < length; i++) {
    if (distances[i] > largest)
      largest = distances[i];
  }
  return largest;
}

// Sort the values in Neighbors based on the temperature.
// Since months is a parallel array, we arrange the order based
// on temperature.
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

// Adds the new value into the n array.
// Neighbors n - The current neighbors object
// double distance - Distance to add into the array
// int month - Month to add into the array
// 
// How this works:
// Find the largest element to replace with the new distance
// and month. If we find a position, perform a swap and end the
// function. If no positions were found, the original neighbors
// object is returned.
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

// Prints the months and distances of the neighbors object
// for debugging purposes.
void printNeighbors(Neighbors n) {
  for (int i = 0; i < n.position; i++) {
    printf("Months: %d, Distance: %f\n", n.months[i], n.distances[i]);
  }
}

// Takes in the test data, points, and centroids to make predictions.
// knn - how many points to compare to
// traindata - training data
// trainlength - length of training data
// testdata - test data
// testlength - length of testing data
void kNN(int knn, Weather* traindata, int trainlength, Weather* testdata, int testlength) {
  // Neighbors object 
  Neighbors n;

  // Number of correct and incorrect answers
  int correct = 0;
  int incorrect = 0;

  // Iterate through all the test data points to conduct knn prediction
  for (int i = 0; i < testlength; i++) {
    // Comparing only winter and summer
    if (testdata[i].month != 1 && testdata[i].month != 7) {
      continue;
    }

    // Allocate the months and distances array depending on the size of knn
    n.months = (int*)malloc(sizeof(int) * knn);
    n.distances = (double*)malloc(sizeof(double) * knn);
    
    // Current number of elements in the months and distances array.
    // This also represents the position to add new elements.
    n.position = 0;

    // Best maximum value in the array of months and distances
    double best_max = -99;

    // Compare the test data point to each training data point
    for (int j = 0; j < trainlength; j++) {
      // Comparing only winter and summer
      if (traindata[j].month != 1 && traindata[j].month != 7) {
        continue;
      }

      // Calculate the distance from the testing datapoint to the current
      // training data point.
      double distance = euclideanDistance(traindata[j].temp, traindata[j].slp, 
                                          testdata[i].temp, testdata[i].slp);
      
      // If a better calculated distance is found, then it needs to add to
      // the neighbors object or replace an element.
      if (best_max == -99 || distance < best_max) {

        // If we maxed out on the months and distances array, then an element
        // needs to be replaced.
        // If we haven't maxed out on the months and distances array, then just
        // place at the n.position place.
        if (n.position >= knn) {
          // Find position to replace
          n = addValue(n, distance, traindata[j].month);
        } else {
          n.distances[n.position] = distance;
          n.months[n.position] = traindata[j].month;
        }

        // Get the new largest distance, to compare against other calculated
        // distances.
        best_max = largestDistance(n.distances, n.position);

        // Sort the values in the months and distances array to allow the addValue function
        // to work.
        n = sort(n);

        // If we haven't filled up on max positions of months and distances, so move the position
        // over for the next entry.
        if (n.position < knn) {
          n.position++;
        }
      }
    }

    // Summer and winter count to be able to determine which month to classify
    // a point.
    int summer_count = 0;
    int winter_count = 0;

    // The predicted month.
    int prediction;

    // Iterate through the array to calculate the number of 
    // winter and summer.
    for (int i = 0; i < n.position; i++) {
      if (n.months[i] == 1) winter_count++;
      else if (n.months[i] == 7) summer_count++;
    }

    // Determine which prediction to classify the test datapoint.
    if (winter_count >= summer_count) {
      prediction = 1; 
    } else {
      prediction = 7;
    }


    // Print and update results.
    printf("Predicted: %d, Actual: %d\n", prediction, testdata[i].month);
    if (prediction == testdata[i].month) {
      correct++;
    } else {
      incorrect++;
    }
  }

  free(n.months);
  free(n.distances);

  // Display the number of correct and incorrect answers.
  printf("Correct: %d\n", correct);
  printf("Incorrect: %d\n", incorrect);
}

int main() {
  // Data generation
  char filename[] = "./data/socal_weather.csv";
  printf("Generating dataset...\n");
  Weather* data = generateData(filename);
  printf("Dataset created.\n\n");

  // Training and test data partition
  printf("Generating training data of 75 percent...\n");
  int train_length = 2491;
  Weather *train = trainData(train_length, data);
  for (int i = 0; i < train_length; i++) {
    printf("%d, %f, %f\n", train[i].month, train[i].temp, train[i].slp);
  }
  printf("Training data created.\n\n");

  printf("Generating testing data of 25 percent...\n");
  int test_length = 830;
  Weather *test = testData(test_length, data);
  for (int i = 0; i < test_length; i++) {
    printf("%d, %f, %f\n", test[i].month, test[i].temp, test[i].slp);
  }
  printf("Testing data created.\n\n");
  
  // Sequential KNN execution
  clock_t t;
  double timeTaken;
  printf("Executing sequential knn...\n");
  t = clock();
  kNN(3, train, train_length, test, test_length);
  t = clock() - t;
  timeTaken = ((double)t) / CLOCKS_PER_SEC;
  printf("Sequential knn complete.\n");
  printf("Sequential knn execution time: %f\n\n", timeTaken);

  // Garbage clean
  free(data);
  free(train);
  free(test);

  return 0;
}

