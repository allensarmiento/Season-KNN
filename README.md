# Season-Clusters  
Project 2 - Attempting to predict the season based on mean temperature and sea level pressure  

## How to Compile:
Make sure the makefile is available and execute the following:
```
make
./main
```

## Pseudocode for KNN:
1. Allocate memory for a months array and a distance array
2. For each testing data, compare each point with each training data and calculate the Euclidean distance
3. Store the calculated Euclidean distance in the distance array along with the corresponding month
4. Sort the distances array alongside the months array to maintain the order
5. Get the smallest knn distances from the distances array as well as the corresponding month
6. Calculate the most occurring months and set that as the prediction
7. If there is a tie, the first month is selected
8. Continue until every testing data point creates a prediction
9. Print out the number of correct and incorrect predictions


## NOTE:
1. Parallel runtime is a lot slower due to the sorting that takes place. Parallel has a sorting of  
   a very large array while sequential only keeps track of the top k values

## TODO:  
1. Import the dataset (Month, Mean temperature, Sea Level Pressure) [Status: COMPLETED]  
2. Figure out which station is which [Status: COMPLETED]   
3. Generate coordinates the Mean Temperature vs. Sea Level Pressure [Status: COMPLETED]   
4. Program k-means cluster for a total of 2 or 4 clusters (Winter, Spring, Summer, Fall)  
    - Design and implement ourselves  
5. Take 75% of the data randomly as training data  
6. Take 25% of the data as test data  
7. Analyze the predictions of the 25% data and see how accurate it is  

