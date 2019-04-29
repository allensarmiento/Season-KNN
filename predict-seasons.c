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

// Cluster struct
//    c - allocate memory to hold weather data
//    centroid - location of centroid
//    count - number of points in c
typedef struct Cluster {
  Weather* c;
  Weather centroid;
  int count;
} Cluster;

// Prints the array containing the points belonging to the cluster
void printPoints(struct Cluster *c) {
  for (int i = 0; i < c->count; i++) {
    printf("(%f, %f)", c->c[i].temp, c->c[i].slp);
  }
  printf("\n\n");
}

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

// Initialize the cluster taking in the following:
//    - data: data imported from csv file
//    - size: size of the training data
//    - month: month we want data for 
Cluster* initCluster(Weather* data, int size, int month) {
  Cluster* c = (Cluster *)malloc(sizeof(Cluster));
  c->c = (Weather *)malloc((size-400) * sizeof(Weather)); // Ran out of memory allocation, so subtracted 400
  c->count = 0;

  for (int i = 0; i < size; i++) {
    if (data[i].month == month) {
      int position = c->count;
      c->c[position].month = data[i].month;
      c->c[position].temp = data[i].temp;
      c->c[position].slp = data[i].slp;
      c->count += 1;
    }
  }

  return c;
}

// Perform k-means clustering.
Cluster* kMeans(Cluster *c) {
  int sum_temp = 0;
  int sum_slp = 0;

  for (int i = 0; i < c->count; i++) {
    sum_temp += c->c[i].temp;
    sum_slp += c->c[i].slp;
  }

  c->centroid.temp = sum_temp / c->count;
  c->centroid.slp = sum_slp / c->count;

  return c;
}

/*void other(Weather* data, Cluster* c1, Cluster* c2) {
  float localDistanceC1;
  float localDistanceC2;

  #pragma omp parallel private(localDistanceC1, localDistanceC2) shared(c1, c2) 
  {
    localDistanceC1 = 0;
    localDistanceC2 = 0;

    #pragma omp for schedule(static, 1)
      for (int i=0; i<ARRAYSIZE; ++i) {
        localDistanceC1 = euclideanDistance(
            data[i].temp,
            data[i].slp,
            c1->centroid.temp,
            c1->centroid.slp);

        localDistanceC2 = euclideanDistance(
            data[i].temp,
            data[i].slp,
            c2->centroid.temp,
            c2->centroid.slp);

        if (localDistanceC1 <= localDistanceC2) {
          int position = c1->count;
          c1->c[position].temp = data[i].temp;
          c1->c[position].slp = data[i].slp;
          c1->count++;
        } else {
          int position = c2->count;
          c2->c[position] = data[i];
          c2->count++;
        }
      }
  }
}*/

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

  for (int i = ARRAYSIZE - 1; i >= portion; i--) {
    test[i].month = data[i].month;  
    test[i].temp = data[i].temp;
    test[i].slp = data[i].slp;
  }

  return test;
}

// Calculate the euclidean distance between two points.
float euclideanDistance(float x1, float y1, float x2, float y2) {
  return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}

// Takes in the test data, points, and centroids to make predictions.
void kNN(Cluster* c1, Cluster* c2, Weather* testdata, int testlength) {
  for (int i = 0; i < testlength; i++) {
    float distanceToC1 = euclideanDistance(c1->centroid.temp, c1->centroid.slp, 
                                           testdata[i].temp, testdata[i].slp);
    float distanceToC2 = euclideanDistance(c2->centroid.temp, c2->centroid.slp, 
                                           testdata[i].temp, testdata[i].slp);

    if (distanceToC1 <= distanceToC2) {
      printf("Prediction: month = 1, Actual: %d\n", testdata[i].month);
      printf("(%f, %f)\n", testdata[i].temp, testdata[i].slp);
    } else {
      printf("Prediction: month = 7, Actual: %d\n", testdata[i].month);
      printf("(%f, %f)\n", testdata[i].temp, testdata[i].slp);
    }
  }
}

int main() {
  char filename[] = "./data/socal_weather.csv";
  printf("Generating dataset...\n");
  Weather* data = generateData(filename);
  printf("Dataset created.\n\n");

  printf("Generating training data of 75 percent...\n");
  int train_length = 2491;
  Weather *train = trainData(train_length, data);
  printf("Training data created.\n\n");

  printf("Generating testing data of 25 percent...\n");
  int test_length = 830;
  Weather *test = testData(test_length, data);
  printf("Testing data created.\n\n");

  printf("Init first cluster...\n");
  Cluster* cluster1 = initCluster(data, train_length, 1);
  printf("Initialization complete.\n\n");
  printf("Computing kmeans of cluster 1...\n");
  cluster1 = kMeans(cluster1);
    printf("Centroid 1 location: (%f, %f)\n", 
           cluster1->centroid.temp, cluster1->centroid.slp);
  printf("Centroid calculation complete.\n\n");

  printf("Init second cluster...\n");
  Cluster* cluster2 = initCluster(data, train_length, 7);
  printf("Initialization complete.\n\n");
  printf("Computing kmeans of cluster 2...\n");
  cluster2 = kMeans(cluster2);
    printf("Centroid 2 location: (%f, %f)\n", 
           cluster2->centroid.temp, cluster2->centroid.slp);
  printf("Centroid calculation complete.\n\n");

  printf("Executing knn...\n");
  kNN(cluster1, cluster2, test, test_length);
  printf("knn complete.\n\n");

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


