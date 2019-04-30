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

// Calculate the euclidean distance between two points.
float euclideanDistance(float x1, float y1, float x2, float y2) {
  return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}

// Takes in the test data, points, and centroids to make predictions.
void kNN(Weather* traindata, int trainlength, Weather* testdata, int testlength) {
  int correct = 0;
  int incorrect = 0;

  for (int i = 0; i < testlength; i++) {
    // Comparing only winter and summer
    if (testdata[i].month != 1 && testdata[i].month != 7) {
      continue;
    }

    int month = 0;
    double best_distance = -99;

    for (int j = 0; j < trainlength; j++) {
      // Comparing only winter and summer
      if (traindata[j].month != 1 && traindata[j].month != 7) {
        continue;
      }

      double distance = euclideanDistance(traindata[j].temp, traindata[j].slp, 
                                          testdata[i].temp, testdata[i].slp);
      if (best_distance == -99 || distance <= best_distance) {
        best_distance = distance;
        month = traindata[j].month;
      }
    }

    if (month == testdata[i].month) {
      printf("Predicted: %d, Actual: %d\n", month, testdata[i].month);
      correct++;
    } else {
      printf("Predicted: %d, Actual: %d\n", month, testdata[i].month);
      incorrect++;
    }
  }

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
  kNN(train, train_length, test, test_length);
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

/*

// Use the training data to predict the testing data.
// Compare the predictions to the actual values.
void predict(Weather train, Weather test) {
    // Call generateCentroids()
    // Call kMeans(points, centroids)
    // Call function kNN(points, centroids, test) 
    //    - This function uses the kMeans to predict the test data
}




*/


