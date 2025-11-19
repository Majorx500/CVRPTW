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
#include <iomanip>
#include<chrono>
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
std::vector<int> splitVector(const std::vector<int> &vector,int &index);


template<typename T>
void printVector(std::vector<T> M){
	for( auto i : M){
		std::cout << i << " ";
	}
	std::cout << "\n";
	return;
}



void saveFile(std::vector<int> &routesVector, float distance) {

    std::ofstream endFile("result.txt");
    if (!endFile.is_open()) return;

    int routeIndex = 0;
    int routeCount = 0;

	//i believe routesLength and count will be passed seperately. ._.

	std::vector<std::vector<int>> route;
    while (routeIndex < routesVector.size()) {
        route.push_back(splitVector(routesVector, routeIndex));
        if (!route.empty()) {
            routeCount++;  // count routes
        }
        routeIndex++;  
    }

	endFile << routeCount << " " << distance << " \n";

	for (int i = 0; i < route.size(); i++) {
				for(int v : route.at(i)){
					endFile << v << " ";
				}
            endFile << "\n";
			}
            
    endFile.close();
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
					std::cout << "LIne: " << line;
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
	std::streamsize ss = std::cout.precision();
	std::cout << std::setprecision(4);
	for(int i = 0; i < N ;i++)
		std::cout << "\t" << i;
	std::cout << "\n";
}

template<typename T>
T max(std::vector<T> V, int &k){
	T max = V[0];
	for(int i = 1; i < V.size(); i++){
		if(V[i] > max){
			k = i;
		}
	}
}


template<typename T>
T max(std::vector<T> V){
	T max = V[0];
	for(int i = 1; i < V.size(); i++){
		if(V[i] > max){

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


void makeRCL(std::vector<int> &rcl, std::vector<Customer> customers, std::vector<std::vector<float>> distanceMatrix,std::vector<bool> visited, float truckTime, int l,int N, int listSize){
	//ReadyTime - Distance
	std::vector<bool> added(N,false);	
	for(int k = 0; k < listSize; k++){
		float minX = customers[0].dueTime;
		int minI = 0;
		for(int i = 1; i < N; i++){
			float X = distanceMatrix[l][i];
			if(visited[i] || added[i] || X >= minX) continue;
			minX = X; minI = i;
		}
		if(minI == 0) return;
		added[minI]=true;
		rcl.push_back(minI);
	}
	return;
}


float generateInitialSolution(std::vector<Customer> customers, std::vector<std::vector<float>> distanceMatrix, std::vector<int> &res, int N, int maxWeight){	
	res.push_back(0);
	int curWeight = maxWeight;
	std::vector<bool> visited(N,false);
	int visitedCount = 1, i,rclSize = 20, checkedRCL = 0;
	float totalTime = 0, currentTruckTime = 0;
	std::vector<int> RCL;
	//Construct RCL
	makeRCL(RCL,customers,distanceMatrix,visited,currentTruckTime,0,N,rclSize);
	rclSize = RCL.size();
	i = rand()%rclSize;
	res.push_back(RCL[i]); visited[RCL[i]] = true;
	curWeight -= customers[RCL[i]].demand; currentTruckTime = distanceMatrix[0][RCL[i]];
	if(currentTruckTime < customers[RCL[i]].readyTime) currentTruckTime = customers[RCL[i]].readyTime;
	currentTruckTime += customers[RCL[i]].serviceTime;
	while( visitedCount < N-1){
		RCL.clear();
		makeRCL(RCL,customers,distanceMatrix,visited,currentTruckTime,res.back(),N,rclSize);
		if(RCL.size() == 0) break;
		i = rand()%RCL.size();
		float arriveTime = currentTruckTime + distanceMatrix[res.back()][RCL[i]];
		if(arriveTime > customers[RCL[i]].dueTime && checkedRCL < rclSize){checkedRCL++; continue;}
		if(curWeight - customers[RCL[i]].demand < 0 || checkedRCL >= rclSize){
		
			curWeight = maxWeight; checkedRCL = 0;
			totalTime += currentTruckTime + distanceMatrix[res.back()][0];
			res.push_back(0);
			currentTruckTime = 0;
			continue;


		}
		checkedRCL = 0;
		if(arriveTime < customers[RCL[i]].readyTime) arriveTime = customers[RCL[i]].readyTime;
		res.push_back(RCL[i]);visitedCount++;
		visited[RCL[i]] = true;
		curWeight -= customers[RCL[i]].demand;
		currentTruckTime = arriveTime + customers[RCL[i]].serviceTime;
		
	}
	if(currentTruckTime != 0) totalTime += currentTruckTime + distanceMatrix[res.back()][0];
	res.push_back(0);
	return totalTime;
}
void prettyPrintRes(std::vector<int> res, std::vector<Customer> cust){
	for(int i = 0; i < res.size();i++){
		if(res[i] ==0){
			std::cout << "(" << cust[0].x << ","<<cust[0].y<<")\n" <<"(" << cust[0].x << ","<<cust[0].y<<"), ";
			continue;
		}
		std::cout << "(" << cust[res[i]].x << ","<<cust[res[i]].y<<"),";
	}
}

//formats data given by the file to standarized output
std::string formatData(const std::string& input){
    return std::regex_replace(input, std::regex(" {2,}"), " ");
    
}
std::vector<int> splitVector(const std::vector<int> &vector, int &index) {
    std::vector<int> splitedVector;
    while (index < vector.size() && vector[index] != 0) {
        splitedVector.push_back(vector[index]);
        index++;
    }
    return splitedVector;


}

float countDistance(std::vector<int> res, std::vector<Customer> customers, std::vector<std::vector<float>> distanceMatrix){
	float time = 0,truckTime=0;
	int N = res.size();
	for(int i = 1; i < N; i++){
		truckTime += distanceMatrix[res[i-1]][res[i]];
		if(truckTime < customers[res[i]].readyTime) truckTime = customers[res[i]].readyTime;
		truckTime += customers[res[i]].serviceTime;
		if(res[i] == 0){
			time += truckTime;
			truckTime = 0;
		}
	}
	return time;

}



void swapEdges(std::vector<int> &res,int i, int j){
	i+=1;
	while(i < j && res[i] != 0 && res[j] != 0){
		std::swap(res[i],res[j]);
		i++; j--;
	}
}

bool isRouteValid(const std::vector<int> &route, const std::vector<Customer> &customers, const std::vector<std::vector<float>> &distanceMatrix, int truckCapacity)
{
    float totalTime = 0;
    float currentTime = 0;
    int currentLoad = 0;

    for (int i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];
        float travelTime = distanceMatrix[from][to];

        currentTime += travelTime;
	if(to == 0){
		totalTime += currentTime;
		currentLoad = 0;
		currentTime = 0;
		continue;
	}
        if (currentTime > customers[to].dueTime) {
            return false;
        }

        // Czekanie na otwarcie okna 
        if (currentTime < customers[to].readyTime)
            currentTime = customers[to].readyTime;

        currentTime += customers[to].serviceTime;
        
        
        currentLoad += customers[to].demand;
        if (currentLoad > truckCapacity) { 
            return false;
        }
    }

    return true; // Trasa przeszła wszystkie testy
}

int main(int argc, char* argv[]){
	srand(time(NULL));
	using clock = std::chrono::steady_clock;
	auto start = clock::now();
	
    int maxWeight = 200;
    std::vector<Customer> customers;
    //std::cout << argv[1];
    readFile(argv[1], customers, maxWeight);
    int N = customers.size(), truckCount = 0, bestTruckCount = N;
    //std::cout << N << "\n";
    std::vector<int> bestRoute,tmpRoute, bestBestRoute;
    std::vector<std::vector<float>> distanceMatrix(N,std::vector<float>(N,0));
    float bestDistance = 0, bestbestDistance = 30000;
    calculateDistance(N,distanceMatrix,customers);
    //printMatrix(distanceMatrix,N);
    //std::cout << incidenceMatrix[0].size
	while(true){
		truckCount = 0;
		N = customers.size();
		bestRoute.clear();
		tmpRoute.clear();
    	generateInitialSolution(customers,distanceMatrix,bestRoute,N,maxWeight);
    	//printVector(bestRoute);
	bool foundImpr = false;
	tmpRoute = bestRoute;
	N = bestRoute.size();
	bestDistance = countDistance(bestRoute,customers,distanceMatrix);
	for(int i = 0; i<N-1;i++){
		if(bestRoute[i] == 0) truckCount++;
	}
	std::cout << bestDistance << " " << truckCount << "\t";
	//merge
		
	//2opt
	do{
		foundImpr = false;
		for(int i = 1; i < N-2; i++){
			if(tmpRoute[i] == 0) continue;
			for(int j = i+1; j < N-1; j++){
				if(tmpRoute[j] == 0) continue;
				float dL = - distanceMatrix[tmpRoute[i]][tmpRoute[i+1]] - distanceMatrix[tmpRoute[j]][tmpRoute[j+1]] + distanceMatrix[tmpRoute[i+1]][tmpRoute[j+1]] + distanceMatrix[tmpRoute[i]][tmpRoute[j]];	
				swapEdges(tmpRoute,i,j);
				bool valid =  isRouteValid(tmpRoute,customers,distanceMatrix,maxWeight);
				if(dL < 0 && valid){
					float tmpDistance = countDistance(tmpRoute,customers,distanceMatrix);
					if(tmpDistance >= bestDistance) continue;
					foundImpr = true;
					bestRoute = tmpRoute;
					bestDistance = tmpDistance;
				}else{
					tmpRoute = bestRoute;
				}
			}	
		}
	}while(foundImpr);
    	truckCount = 0;
    	for(int i = 0; i<N-1;i++){
		if(bestRoute[i] == 0) truckCount++;
	}
    	std::cout << bestDistance << " " << truckCount <<"\n";
	//printVector(bestRoute);
	//prettyPrintRes(bestRoute,customers);
    //display added customers
	if(bestDistance < bestbestDistance){
	bestbestDistance = bestDistance;
	bestBestRoute = bestRoute;

	}
	auto now = clock::now();
	if(now - start >= std::chrono::minutes(1)){
		break;
	}
	}
	saveFile(bestBestRoute, bestbestDistance);
    return 0;

}
