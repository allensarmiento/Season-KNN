#ifndef STRUCTS_H
#define STRUCTS_H
// Weather struct reprsenting a single entry from the dataset.
//    month - month data column
//    temp - mean temperature column
//    slp - sea level pressure column 
typedef struct Weather {
  int month;
  float temp;
  float slp;
} Weather;

typedef struct Neighbors {
  int *months;
  double *distances;
  int position;
} Neighbors;

#endif
