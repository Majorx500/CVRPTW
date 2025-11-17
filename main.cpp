#include <regex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <unistd.h>

//declaration of structures
struct Customer{
    int id;
    int x;
    int y;
    int demand;
    int readyTime;
    int dueTime;
    int serviceTime;
};

struct Vehicle{
    int vehicleNumber;
    int vehicleCapacity;
};

//declaration of functions
std::string formatData(const std::string& input);
void readFile(const char* name, std::vector<Customer>& customers);
std::vector<int> splitVector(const std::vector<int> &vector,int &index);


template<typename T>
void printVector(std::vector<T> M){
	for( auto i : M){
		std::cout << i << " ";
	}
	std::cout << "\n";
	return;
}



void saveFile(std::vector<int> &routesVector) {

    std::ofstream endFile("result.txt");
    if (!endFile.is_open()) return;

    int routeIndex = 0;
    int routeCount = 0;

	//i believe routesLength and count will be passed seperately. ._.


    while (routeIndex < routesVector.size()) {
        std::vector<int> route = splitVector(routesVector, routeIndex);
        if (!route.empty()) {
            routeCount++;  // count routes
            for (int v : route) {
                endFile << v << " ";
            }
            endFile << "\n";
        }
        routeIndex++;  
    }
    endFile.close();
}


std::vector<int> splitVector(const std::vector<int> &vector, int &index) {

    std::vector<int> splitedVector;

    while (index < vector.size() && vector[index] != 0) {
        splitedVector.push_back(vector[index]);
        index++;
    }
    return splitedVector;
}


void readFile(const char* name, std::vector<Customer>& customers, int &vehicleWeight) {
    std::ifstream file(name);
    if (file.is_open()) {
        std::string line;
        int lineCount = 0;
        while (std::getline(file, line)) {
            line = formatData(line);

            if (line.empty()) {
                continue;
            }

            switch (lineCount)
            {
            case 3:{
                std::istringstream iss(line);
				iss >> vehicleWeight;
                break;}
            default:{
                if (lineCount >= 6) {
                    Customer customer;
                    std::istringstream iss(line);
                    iss >> customer.id >> customer.x >> customer.y >> customer.demand >> customer.readyTime >> customer.dueTime >> customer.serviceTime;
                    customers.push_back(customer);
                }
                break;
			}}
            lineCount++;
        }

    }
	file.close();
}


int calculateDistance(int N,std::vector<std::vector<float>> &distanceM, std::vector<Customer> customers){
	int dx,dy;
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			if(i == j) distanceM[i][j] = -1;
			dx = customers[j].x - customers[i].x;
			dy = customers[j].y - customers[i].y;
			if(dx == 0) distanceM[i][j] = std::abs(dy);
			else if(dy == 0) distanceM[i][j] = std::abs(dx);
			else	distanceM[i][j] = std::sqrt(dx*dx + dy*dy);
		}
	}
	return 0;
}
template<typename T>
void printMatrix(std::vector<std::vector<T>> M, int N){
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			std::cout << M[i][j] << "\t";
		}
		std::cout << "\n";
	}
}

template<typename T>
T max(std::vector<T> V, int &k){
	T max = V[0];
	for(int i = 1; i < V.size(); i++){
		if(V[i] > max){
			k = i;
			max = V[i];
		}
	}
	return max;
}

float min(std::vector<float> V, std::vector<Customer> c){
	float m = V[0] - c[0].readyTime;
	for(int i = 1; i < V.size(); i++){
		if(V[i] - c[i].readyTime < m) m = V[i] - c[i].readyTime;
	}
	return m;
}


int generateInitialSolution(std::vector<Customer> customers, std::vector<std::vector<float>> distanceMatrix, std::vector<int> &res, int N, int maxWeight){
	//std::cout << N << "\n";
	int curWeight = maxWeight;
	std::vector<bool> visited(N,false);
	int visitedCount = 1, i;
	float totalTime = 0, currentTruckTime;
	i = rand()%(N-1) + 1;
	visited[i] = true; visited[0] = true;
	res.push_back(i);
	curWeight -= customers[i].demand;
	currentTruckTime = distanceMatrix[0][i];
	// Generate Initial CVRP Solution
	while( visitedCount < N-1){
 		int shortestDistI = 0;
		float shortestDistance = max(distanceMatrix[i],shortestDistI);	
		for(int k = 1;k < N; k++){
			if(k == i || visited[k] || distanceMatrix[i][k] > shortestDistance) continue;
			shortestDistance = distanceMatrix[i][k];	
			shortestDistI = k;
		}
		//std::cout << shortestDistance << " ";
		if(curWeight - customers[shortestDistI].demand < 0 || shortestDistance > distanceMatrix[0][shortestDistI]){
			res.push_back(0);
			curWeight = maxWeight;
			totalTime += currentTruckTime + distanceMatrix[shortestDistI][0]; 

			i = 0;
		}
		currentTruckTime += distanceMatrix[i][shortestDistI] + customers[shortestDistI].serviceTime;
		curWeight -= customers[shortestDistI].demand;
		visited[shortestDistI] = true;
		res.push_back(shortestDistI);
		i = shortestDistI;
		visitedCount++;

	}
	res.push_back(0);
	// Modify CVRP solution to fit time frames
	printVector(res);
	i =1;
	bool swapped = false;
	while(i < res.size()-1){
		int j = i+1, currentTruckTime = 0;
		while(res[j] != 0) j++;
		int lowestDueTimeI = i;
		for(int k = i; k < j; k++){
			for(int l = k; l < j; l++){
				if(customers[res[l]].dueTime < customers[res[lowestDueTimeI]].dueTime) lowestDueTimeI = l;
			}
			if(k==lowestDueTimeI) continue;
			std::swap(res[k],res[lowestDueTimeI]);
		}

		for(int k = i; k < j; k++){
			currentTruckTime += distanceMatrix[res[k-1]][res[k]];
			if(currentTruckTime > customers[res[k]].dueTime){
				std::cout << k << "\n";
				for(int l = k; l < res.size()-2; l++){
					std::swap(res[l],res[l+1]);
				}
			}
			if(currentTruckTime < customers[res[k]].readyTime) currentTruckTime = customers[res[k]].readyTime;
			currentTruckTime += customers[res[k]].serviceTime;
			j--;
		}
		printVector(res);std::cout << "\n";
		i= j+1;
	}
		
		
	
	return totalTime;
}

//formats data given by the file to standarized output
std::string formatData(const std::string& input){
    return std::regex_replace(input, std::regex(" {2,}"), " ");
    
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    int vehicleWeight = 0,j;
    std::vector<Customer> customers;
    //std::cout << argv[1];
    readFile(argv[1], customers, vehicleWeight);
    int N = customers.size();
    //std::cout << N << "\n";
    std::vector<int> res; res.push_back(0);
    std::vector<std::vector<bool>> incidenceMatrix(N, std::vector<bool>(N,false));
    std::vector<std::vector<float>> distanceMatrix(N,std::vector<float>(N,0));
    calculateDistance(N,distanceMatrix,customers);
    //printMatrix(distanceMatrix,N);
    //std::cout << incidenceMatrix[0].size();
    

	std::cout << generateInitialSolution(customers,distanceMatrix,res,N,vehicleWeight)<< "\n";
    printVector(res);
	bool foundImpr = false;
	/*
	do{
		for(int i = 1; i < N-1; i++){
			for(int j = i+1; j < N; j++){

			}
		}
	}while(foundImpr);
	*/
    //display added customers;
	/*
	*/
	std::cout << "\n";
	//test values needs refinement
	std::vector<int> testRes = {0,2,1,6,5,0,1,6,2,0};
	saveFile(testRes);
    return 0;

}
