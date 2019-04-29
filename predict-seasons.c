#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#define ARRAYSIZE 3321

// Weather struct object representing a single entry from the dataset.
typedef struct Weather {
  int month;
  float temp;
  float slp;
} Weather;

typedef struct Cluster {
  Weather* c;
  Weather centroid;
  int count;
} Cluster;

// Takes in a csv file specified by fname and returns a pointer to an array of
// weather objects.
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

// Initialize a cluster and select a random Weather entry as the first centroid.
// This function should only be called once and returns a struct containing a
// Cluster object.
Cluster* initCluster(Weather* data, int seed) {
  Cluster* c = (Cluster *)malloc(sizeof(Cluster));

  srand(time(NULL)*seed); // Generate 1st centroid coords
  int i = rand() % ARRAYSIZE + 1;
  c->centroid = data[i];
  return c;
}

// Calculate the euclidean distance between two points.
float euclideanDistance(float x1, float y1, float x2, float y2) {
  // NOTE: Pow might need to take in a double, check if this works.
  return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}

// Perform k-means clustering.
// Centroids and points should be passed in.
void kMeans(Weather* data, Cluster* c1, Cluster* c2) {
  float localDistanceC1;
  float localDistanceC2;

#pragma omp parallel private(localDistanceC1, localDistanceC2) shared(c) {
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
  }

}

  // Approach:
  //    1. Identify the closest points to each centroid; this forms the
  //       grouping.
  //    2. In parallel, calculate the euclidean distance between each centroid
  //       and points in its grouping.
  //    3. If the euclidean distance is not proportional, move centroids and
  //       repeat calculation
  //    4. Centroids are now in place, can now apply training and testing data
  //
  // Pseudocode:
  //   
  //   # NOTE: This can be done in parallel, partition the points
  //   #       and pass in the coordinates of each centroid to evaluate
  //   for point in points:
  //      min_distance = None 
  //      centroid_number = None
  //      for centroid in centroids:
  //        if euclideanDistance(point, centroid) < min_distance:
  //            min_distance = euclideanDistance(point, centroid)
  //            centroid_number = centroid
  //      addGrouping(point, centroid_number)
  //
  //   # In parallel, calculate the distance between each point and its
  //   # centroid.
  //   values = []
  //   for points in centroid_group:
  //      values.append( euclideanDistance(points, centroid) ) 
  //          
  //   if centroid location is bad:
  //      # Call function recursively
  //      kMeans(points, centroids)
}

int main() {
  char filename[] = "./data/socal_weather.csv";
  printf("Generating dataset\n");
  Weather* data = generateData(filename);
  printf("Dataset created.\n\n");

  printf("Init first cluster and random centroid.\n");
  Cluster* cluster1 = initCluster(data, 1);
  printf("\tCluster 1 centroid: (%f, %f)\n",
      cluster1->centroid.temp, cluster1->centroid.slp);
  printf("Complete.\n\n");

  printf("Init second cluster and random centroid.\n");
  Cluster* cluster2 = initCluster(data, 12);
  printf("\tCluster 2 centroid: (%f, %f)\n",
      cluster2->centroid.temp, cluster2->centroid.slp);
  printf("Complete.\n\n");

  return 0;
}

/*
// Takes in a percentage of the weather data as training data at random.
void trainData(float percentage, Weather data) {
}

// Evaluates which data is not used in the training data and sets those
// as testing data.
void testData(Weather train, Weather data) {
}

// Use the training data to predict the testing data.
// Compare the predictions to the actual values.
void predict(Weather train, Weather test) {
    // Call generateCentroids()
    // Call kMeans(points, centroids)
    // Call function kNN(points, centroids, test) 
    //    - This function uses the kMeans to predict the test data
}


// Takes in the test data, points, and centroids to make predictions.
void kNN() {
}


*/


