# Season-Clusters  
Project 2 - Attempting to predict the season based on mean temperature and sea level pressure  

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

