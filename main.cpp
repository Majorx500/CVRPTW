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


//Gets file given as an argument and processes data to relevant components.
void readFile(const char* name, std::vector<Customer>& customers){
    //std::cout << "Print Hello World\n";
    std::ifstream file(name);
    if(file.is_open()){
        std::string line;
        int lineCount = 0;
            while(!file.eof()){
                std::getline(file, line);
                //count only important lines;
                if(line.empty()){
                    continue;
                }else{
                    line = formatData(line);
                }

                switch (lineCount)
                {
                case 1: 
                    /* set problem name varaible */
                    break;
                case 4: 
                    //std::istringstream iss(line);
                    // Then add K and Q to the vehicles.
                    break;
                default:
                    if(lineCount >= 6){
                        Customer customer;
                       // std::cout << line << "\n";
                        std::istringstream iss(line);  
                        iss >> customer.id >> customer.x >> customer.y >> customer.demand >> customer.readyTime >> customer.dueTime >> customer.serviceTime;
                        customers.push_back(customer);
                    }
                    break;
                }
                //get only clients' coordinates;
                lineCount++;
            }
    
    }else{
        std::cout << "File wasn't open";
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
	for(int i = 0; i < N; i++){
		std::cout << i << "\t";
		for(int j = 0; j < N; j++){
			std::cout << M[i][j] << "\t";
		}
		std::cout << "\n";
	}
	std::cout << std::setprecision(ss);
	
}

template<typename T>
void printVector(std::vector<T> M){
	for( auto i : M){
		std::cout << i << " ";
	}
	std::cout << "\n";
	return;
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
	currentTruckTime = distanceMatrix[0][i] + customers[i].serviceTime;
	// Generate Initial CVRP Solution
	while( visitedCount < N-1){
 		int shortestDistI = -1;
		float shortestDistance = max(distanceMatrix[i]);	
		for(int k = 1;k < N; k++){
			if(k == i || visited[k] || distanceMatrix[i][k] > shortestDistance) continue;
			if(currentTruckTime + distanceMatrix[i][k] > customers[k].dueTime) continue;
			shortestDistance = distanceMatrix[i][k];	
			shortestDistI = k;
		}
		//std::cout << shortestDistance << " ";
		if(shortestDistI == -1 || curWeight - customers[shortestDistI].demand < 0){
			curWeight = maxWeight;
			currentTruckTime = 0;
			res.push_back(0);
			i = 0;
			continue;
		}
		float arriveTime = currentTruckTime + distanceMatrix[i][shortestDistI];
		if(arriveTime < customers[shortestDistI].readyTime) arriveTime = customers[shortestDistI].readyTime;
		curWeight -= customers[shortestDistI].demand;
		visited[shortestDistI] = true;
		res.push_back(shortestDistI);
		i = shortestDistI;
		visitedCount++;
	}
	res.push_back(0);
	//printVector(res);
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

std::vector<int> splitVector(const std::vector<int> &vector, int index, bool &emptyVector){
    std::vector<int> splitedVector;
    emptyVector = true;
    index++;
    splitedVector.push_back(0);
    while(index < vector.size() && vector.at(index) != 0){
            splitedVector.push_back(vector.at(index));
            emptyVector = false;
            index++;
        }
    splitedVector.push_back(0);
    return splitedVector;
}

void repairSubRoutes(std::vector<int> &route, const std::vector<Customer> &customers, const std::vector<std::vector<float>> &distanceMatrix)
{
    int testTruckCapacity = 20;
    float currentTime = 0;
    int currentLoad = 0;
    printVector(route);std::cout <<"\t:\t";

    for (int i = 0; i < route.size() - 1; i++) {
        int from = route[i];
        int to = route[i + 1];
        float travelTime = distanceMatrix[from][to];
        //std::cout << "travel time: " << travelTime << "\n";

        currentTime += travelTime;

        if (currentTime > customers[to].dueTime) {
            //std::cout << "Cannot arrive in time to " << customers[to].id << "\n";
            return;
        }

        //waiting till window open
        if (currentTime < customers[to].readyTime)
            currentTime = customers[to].readyTime;

        currentTime += customers[to].serviceTime;
        currentLoad += customers[to].demand;

        if (currentLoad > testTruckCapacity) {
            //std::cout << "TO: " <<  to;
            //std::cout << " Over truck capacity " << customers[to].id << "\n";
            return;
        }
    }
	printVector(route);std::cout << "\n";
    //std::cout << "Route correct!\n";
    //std::cout << "Time: " << currentTime;
    //std::cout << "Load: " << currentLoad;
}




void repairSolution(std::vector<int>& res, const std::vector<Customer>& customers, const std::vector<std::vector<float>>& distanceMatrix)
{
    //due time and capacity of the customer
    std::vector<int> vectorCopy = res;
    //std::vector<int> testVector = { 0, 3, 2, 1, 0}; //only for tests

    std::vector<std::vector<int>> partialVector;
    for(int i = 0; i< vectorCopy.size(); i++){
        if(vectorCopy.at(i) == 0){
            bool emptyVector = true; //makes sure no empty vector will be returned
            partialVector.push_back(splitVector(vectorCopy, i, emptyVector));
            if(emptyVector == true){
                partialVector.pop_back();
            }
        }
    }
	for (int i = 0; i < partialVector.size(); i++) 
            repairSubRoutes(partialVector.at(i), customers, distanceMatrix);
}



float countTime(std::vector<int> res, std::vector<Customer> customers, std::vector<std::vector<float>> distanceMatrix){
	float time = 0;
	int N = res.size();
	for(int i = 1; i < N; i++){
		time += distanceMatrix[res[i-1]][res[i]];
		if(res[i] != 0)	time += customers[res[i]].serviceTime;
	}
	return time;

}

void swapEdges(std::vector<int> res,int i, int j){
	i+=1;
	while(i < j){
		int tmp = res[i];
		res[i] = res[j];
		res[j] = tmp;
		i++; j--;
	}
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    int maxWeight = 200,j;
    std::vector<Customer> customers;
    //std::cout << argv[1];
    readFile(argv[1], customers);
    int N = customers.size();
    //std::cout << N << "\n";
    std::vector<int> bestRoute,tmpRoute; bestRoute.push_back(0);
    std::vector<std::vector<bool>> incidenceMatrix(N, std::vector<bool>(N,false));
    std::vector<std::vector<float>> distanceMatrix(N,std::vector<float>(N,0));
    float bestDistance;
    calculateDistance(N,distanceMatrix,customers);
    //printMatrix(distanceMatrix,N);
    //std::cout << incidenceMatrix[0].size();
	bestDistance = generateInitialSolution(customers,distanceMatrix,bestRoute,N,maxWeight);
    	printVector(bestRoute);
	//prettyPrintRes(bestRoute,customers);
	//printMatrix(distanceMatrix,N);
	bool foundImpr = false;
	tmpRoute = bestRoute;
	N = bestRoute.size();
	//printMatrix(distanceMatrix,26);
	std::cout << bestDistance << ":" << countTime(bestRoute,customers,distanceMatrix) << "\n";
	do{
		for(int i = 1; i < N-2; i++){
			if(tmpRoute[i] == 0) continue;
			for(int j = i+1; j < N-1; j++){
				if(tmpRoute[j] == 0) continue;
				float dL = - distanceMatrix[tmpRoute[i]][tmpRoute[i+1]] - distanceMatrix[tmpRoute[j]][tmpRoute[j+1]] + distanceMatrix[tmpRoute[i+1]][tmpRoute[j+1]] + distanceMatrix[tmpRoute[i]][tmpRoute[j]];	
				if(dL < 0){
					swapEdges(tmpRoute,i,j);
					repairSolution(tmpRoute, customers,distanceMatrix);
					float tmpDistance = countTime(tmpRoute,customers,distanceMatrix);
					if(tmpDistance >= bestDistance) continue;
					//foundImpr = true;
					bestRoute = tmpRoute;
				}
			}	
		}
	}while(foundImpr);
	std::cout << "\n";
	printVector(bestRoute);
	//prettyPrintRes(bestRoute,customers);
	
    //display added customers;
	/*
	*/
	std::cout << "\n";
    return 0;

}
