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
	for(int i = 0; i < N; i++){
		for(int j = 0; j < N; j++){
			std::cout << M[i][j] << "\t";
		}
		std::cout << "\n";
	}
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
			totalTime += currentTruckTime + distanceMatrix[res.back()][0]; 
			currentTruckTime = 0;
			res.push_back(0);
			i = 0;
			continue;
		}
		float arriveTime = currentTruckTime + distanceMatrix[res.back()][shortestDistI];
		std::cout << shortestDistI << " : " << arriveTime << "\n";usleep(5000);	
		if(arriveTime < customers[shortestDistI].readyTime) arriveTime = customers[shortestDistI].readyTime;
		if(arriveTime > customers[shortestDistI].dueTime) continue;
		currentTruckTime = arriveTime + customers[shortestDistI].serviceTime;
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
	/*
	bool swapped = false;
	//res = {0, 25, 4, 21, 22, 23, 0, 2, 13, 6, 5, 17, 16, 14, 15, 0, 12, 3, 24, 9, 20, 10, 11, 19, 7, 18, 8, 0, 1, 0};
	while(i < res.size()-1){
		int j = i+1, currentTruckTime = 0;
		while(res[j] != 0) j++;
		std::cout << i << " " << j << "\n";
		int lowestDueTimeI = i;
		//sort route by lowest due time
		for(int k = i; k < j; k++){
			for(int l = k; l < j; l++){
				if(customers[res[l]].readyTime < customers[res[lowestDueTimeI]].readyTime) lowestDueTimeI = l;
			}
			if(k==lowestDueTimeI) continue;
			std::swap(res[k],res[lowestDueTimeI]);
		}
		printVector(res);
		//if customer not in time frame move to new route at the end
		for(int k = i+1; k < j; k++){
			swapped=false;
			float arriveTime = distanceMatrix[res[k-1]][res[k]] + currentTruckTime;
			if(arriveTime > customers[res[k]].dueTime){
				std::cout << k << "\n";
				if(j == res.size()-1) res.push_back(0);
				currentTruckTime -=distanceMatrix[res[k-1]][res[k]];
				for(int l = k; l < res.size()-2; l++){
					std::swap(res[l],res[l+1]);
					
				}
				printVector(res);
				swapped= true;
				//std::cout << res[k] << "\n";
				j--;
			}
			if(swapped){
			k--; continue;
			}
			currentTruckTime += distanceMatrix[res[k-1]][res[k]];
			if(currentTruckTime < customers[res[k]].readyTime) currentTruckTime = customers[res[k]].readyTime;
			currentTruckTime += customers[res[k]].serviceTime;			
			std::cout << currentTruckTime << "\n";
		}
		//std::cout << j << "\n";
		//printVector(res);std::cout << "\n";
		i= j+1;
	}
	if(res.back() != 0) res.push_back(0);
	*/	
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

int main(int argc, char* argv[]){
    srand(time(NULL));
    int maxWeight = 200,j;
    std::vector<Customer> customers;
    //std::cout << argv[1];
    readFile(argv[1], customers);
    int N = customers.size();
    //std::cout << N << "\n";
    std::vector<int> res; res.push_back(0);
    std::vector<std::vector<bool>> incidenceMatrix(N, std::vector<bool>(N,false));
    std::vector<std::vector<float>> distanceMatrix(N,std::vector<float>(N,0));
    calculateDistance(N,distanceMatrix,customers);
    //printMatrix(distanceMatrix,N);
    //std::cout << incidenceMatrix[0].size();
    

	std::cout << generateInitialSolution(customers,distanceMatrix,res,N,maxWeight)<< "\n";
    	printVector(res);
	prettyPrintRes(res,customers);
	//printMatrix(distanceMatrix,N);
	bool foundImpr = false;
	
	do{
		for(int i = 1; i < N-1; i++){
			for(int j = i+1; j < N; j++){

			}
		}
	}while(foundImpr);
	
    //display added customers;
	/*
	*/
	std::cout << "\n";
    return 0;

}
