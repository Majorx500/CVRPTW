#include <regex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <time.h>

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
std::vector<int> splitVector(const std::vector<int> &vector, int index, bool &emptyVector);

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
                    if(lineCount >= 7){
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
	// Generating TSP solution
	std::vector<bool> visited(N,false);
	int visitedCount= 1, i;
	float c=0,curD;
	i = rand()%(N-1) + 1;
	visited[i] = true;
	res.push_back(i);
	curWeight -= customers[i].demand;
	curD = distanceMatrix[0][i];
	if(curD < customers[i].readyTime) curD = customers[i].readyTime;
	curD += customers[i].serviceTime;
	//std::cout << i << " " << curD << "\n";
	while( visitedCount < N-1){
		float maxD = min(distanceMatrix[i], customers); int maxIt = 0;
		for(int k = 1;k < N; k++){
			if (k == i) continue;
			if(distanceMatrix[i][k] - customers[k].readyTime < maxD || visited[k] || curWeight - customers[k].demand < 0) continue;
			
			maxD = distanceMatrix[i][k] - customers[k].readyTime;	
			maxIt = k;
		}
		if(maxIt == 0)	
		{
			curWeight = maxWeight;
			c+= curD + distanceMatrix[i][0];
			curD = 0;
			i = maxIt;
			res.push_back(maxIt);
			continue;	
		}
		visitedCount++;
		curD += distanceMatrix[i][maxIt];
		if(curD < customers[maxIt].readyTime) curD = customers[maxIt].readyTime;
		curD += customers[maxIt].serviceTime;
		visited[maxIt] = true;
		curWeight -= customers[maxIt].demand;
		res.push_back(maxIt);
		i = maxIt;
	}
	c += curD;
	c += distanceMatrix[res.back()][0];
	res.push_back(0);
	return c;
}

//formats data given by the file to standarized output
std::string formatData(const std::string& input){
    return std::regex_replace(input, std::regex(" {2,}"), " ");
    
}

std::vector<int> copyVector(const std::vector<int> &original){
    std::vector<int> newVector;
    newVector = original;
    /*This is for debugging purposes only :)
    newVector.push_back(12);
    std::cout << "New vector: ";
    printVector(newVector);
    std::cout << "\n";
    printVector(original);
    */

    return original;
}



void repairSolution(std::vector<int>&res){
    //due time and capacity of the customer
    std::vector<int> vectorCopy = copyVector(res);
    std::vector<int> testVector = {0, 3, 2, 1, 4, 0, 1, 5 ,7 ,6, 8, 0}; //only for tests

    std::vector<std::vector<int>> partialVector;
    for(int i = 0; i< testVector.size(); i++){
        if(testVector.at(i) == 0){
            bool emptyVector = true; //makes sure no empty vector will be returned
            partialVector.push_back(splitVector(testVector, i, emptyVector));
            if(emptyVector == true){
                partialVector.pop_back();
            }
        }
    }
        //YAP YAP YAP YAP YAP YAP YAP YAP YAP YAP YAP
        //displaying splited vectors (test only)
        for(int i=0; i< partialVector.size(); i++){
            std::cout<<"Test vector: \n";
            printVector(partialVector.at(i));  
    
        }
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


int main(int argc, char* argv[]){
    srand(time(NULL));
    int maxWeight = 10;
    std::vector<Customer> customers;
    //std::cout << argv[1];
    readFile(argv[1], customers);
    int N = customers.size();
    //std::cout << N << "\n";
    std::vector<int> res; res.push_back(0);
    std::vector<std::vector<bool>> incidenceMatrix(N, std::vector<bool>(N,false));
    std::vector<std::vector<float>> distanceMatrix(N,std::vector<float>(N,0));
    calculateDistance(N,distanceMatrix,customers);
    printMatrix(distanceMatrix,N);
    //std::cout << incidenceMatrix[0].size();
    std::cout << generateInitialSolution(customers,distanceMatrix,res,N,maxWeight)<< "\n";
    printVector(res);
    repairSolution(res);
    //display added customers;
    /*for(int i=0; i<=N; i++){
        std::cout << customers.at(i).id << " ";
        std::cout << customers.at(i).x << " ";
        std::cout << customers.at(i).y << " ";
        std::cout << "\n";
    }*/
    return 0;

}
