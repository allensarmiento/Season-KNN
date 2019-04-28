#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAYSIZE 3321

typedef struct Weather {
  int month;
  float temp;
  float slp;
} Weather;

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

  // Retrieve each line until eof. Each line should contain
  // no more than 30 characters.
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

// Generate random centroid locations.
// Centroids locations should be returned.
void generateCentroids() {
}

// Perform k-means clustering.
// Centroids and points should be passed in.
//void kMeans() {
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
//}

// Takes in the test data, points, and centroids to make predictions.
void kNN() {
}

// Calculate the euclidean distance between two points.
double euclideanDistance(int x1, int y1, int x2, int y2) {
  // NOTE: Pow might need to take in a double, check if this works.
  return sqrt( pow(x1 - x2, 2) + pow(y1 - y2, 2) );
}
*/

int main() {
  char filename[] = "./data/socal_weather.csv";
  printf("Generating dataset\n");
  Weather* data = generateData(filename);
  printf("Dataset created.\n");
  return 0;
}
