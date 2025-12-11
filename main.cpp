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
#include <chrono>
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

struct PheromonePair{
	int index;
	long double pheromone;
};

struct Vehicle{
    int vehicleNumber;
    int vehicleCapacity;
};

//declaration of functions
std::string formatData(const std::string& input);
std::vector<int> splitVector(std::vector<int> vector,int index);

std::ostream& operator<<(std::ostream& os, const PheromonePair& p) {
    os << "{ idx: " << p.index 
       << ", pher: " << p.pheromone 
       << " }";
    return os;
}


std::ostream& operator<<(std::ostream& os, const Customer& c)
{
    os << "Customer(" << c.id
       << ", d=" << c.demand
       << ", x=" << c.x
       << ", y=" << c.y
       << ")";
    return os;
}

template<typename T>
void printVector(std::vector<T> M){
	for( auto i : M){
		std::cout << i << " ";
	}
	std::cout << "\n";
	return;
}

bool solutionCorrectnessCheck(std::vector<Customer> &customers, std::vector<std::vector<long double>> &distanceMatrix) {
    int depot = 0;

    for(int i = 1; i < customers.size(); i++) {
        float arrival = distanceMatrix[depot][i];
        float startService = std::max(arrival, (float)customers[i].readyTime);
        float backToDepot = startService + customers[i].serviceTime + distanceMatrix[i][depot];

        if(arrival > customers[i].dueTime || backToDepot > customers[depot].dueTime) {
            return false;
        }
    }

    return true;
}



void saveFile(std::vector<int> &routesVector, long double distance, std::string fileName) {

    std::ofstream endFile(fileName);
    if (!endFile.is_open()) return;
	endFile << std::fixed <<  std::setprecision(5);
	//close if result unachiveable
	if (distance == -1){
		endFile << distance;
		endFile.close();
		exit(0);
	}


    int routeIndex = 0;
    int routeCount = -1;

	//i believe routesLength and count will be passed seperately. ._.

	std::vector<std::vector<int>> route;
    while (routeIndex < routesVector.size()) {
        route.push_back(splitVector(routesVector, routeIndex));
		routeIndex += route[routeCount+1].size();
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
			//std::cout << line << std::flush;
			if (line.empty() || line.length() < 3) {
                continue;
            }
            switch (lineCount)
            {
            case 3:{
                std::istringstream iss(line);
				iss >> vehicleWeight >>vehicleWeight;
                break;}
            default:{
                if (lineCount >= 6 && line.length() > 10) {

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

int generatePheromoneMatrix(std::vector<std::vector<int>> &pheromoneMatrix){
	return 1;
}





int calculateDistance(int N,std::vector<std::vector<long double>> &distanceM, std::vector<Customer> customers){
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
	for(int j=0; j < N; j++){
	for(int i = 0; i < N ;i++)
		std::cout << "\t" << M[j][i];
	std::cout << "\n";}
}

void makeRCL(std::vector<int> &rcl, std::vector<Customer> customers, std::vector<std::vector<long double>> distanceMatrix,std::vector<bool> visited, long double truckTime, int l,int N, int listSize){
	std::vector<bool> added(N,false);	
	for(int k = 0; k < listSize; k++){
		long double minX = customers[0].dueTime;
		int minI = 0;
		for(int i = 1; i < N; i++){
			long double X = distanceMatrix[i][l];
			if(visited[i] || added[i] || X >= minX) continue;
			minX = X; minI = i;
		}
		if(minI == 0) return;
		added[minI]=true;
		rcl.push_back(minI);
	}
	return;
}

long double calculatePheromoneTrailANT(std::vector<Customer> customers,
									std::vector<std::vector<long double>> &distanceMatrix,
									std::vector<std::vector<double>> &pheromoneMatrix,
									int currentNode, int nextNode, int A, int B){
	long double vis = 1/(distanceMatrix[currentNode][nextNode]);
	std::cout << "Licznik:" << vis << "\n";
	long double pi = pheromoneMatrix[currentNode][nextNode];
	long double licznik = pow(pi, A)*pow(vis, B);
	return licznik;
	
}


long double generateInitialSolutionANT(std::vector<Customer> customers, 
									   std::vector<std::vector<long double>> &distanceMatrix,
									   std::vector<std::vector<double>> &pheromoneMatrix, 
									   std::vector<int> &res, int N, int maxWeight, int A, int B){	
	
	std::vector<Customer> notVisited = customers;
	std::vector<Customer> visited;
	std::vector<PheromonePair> pheroList;
	std::vector<PheromonePair> probList;

	notVisited.erase(notVisited.begin()); //eliminate depot

	int currentNode = customers[0].id;
	float total = 0;					
	int curWeight = 0;				
	long double totalTime = 0, currentTruckTime = 0;
	
	while (!notVisited.empty()) {
		float total = 0.0f;
		pheroList.clear();
		probList.clear();

		//ten for służy do wyliczenia pheromonów
		for (Customer el : notVisited) {

			if (curWeight - el.demand < 0) continue; // jak mamy za mało ładowności to skip
			long double arrivalTime = currentTruckTime + distanceMatrix[currentNode][el.id];
			if (arrivalTime > el.dueTime) continue; //jeżeli za późno to skip
			
			PheromonePair pheromone;
			pheromone.index = el.id;
			pheromone.pheromone = calculatePheromoneTrailANT(customers, distanceMatrix,pheromoneMatrix, currentNode,el.id, A, B);

			total += pheromone.pheromone; //potrebujemy do Sum(pi * vis)
			pheroList.push_back(pheromone);
		}
		// tutaj wracamy do depotu 
		if (pheroList.empty()) {
			totalTime += currentTruckTime + distanceMatrix[currentNode][0]; 
			currentNode = 0;
			visited.push_back(customers[0]);
			curWeight = maxWeight;
			currentTruckTime = 0;
			continue;
		}

		//wyliczamy resztę wzoru czyli dla każdego elementu dzielimi przez total (sum z poprzednego fora)
		for (auto &trail : pheroList) {
			PheromonePair p;
			p.index = trail.index;
			p.pheromone = trail.pheromone / total;
			probList.push_back(p);
		}

		//sort
		std::sort(probList.begin(), probList.end(),
				[](auto& a, auto& b){ return a.pheromone > b.pheromone; });


		//rozkład pradowpodobienstwa 
		long double r = (long double)rand() / RAND_MAX;
		long double cumulative = 0.0;
		int picked = probList.back().index; 

		//dystrybuanta 
		for (auto &p : probList) {
			cumulative += p.pheromone;
			if (r <= cumulative) {
				picked = p.index;
				break;
			}
		}
		long double arrival = currentTruckTime + distanceMatrix[currentNode][picked];
		if (arrival < customers[picked].readyTime) arrival = customers[picked].readyTime;
		currentTruckTime = arrival + customers[picked].serviceTime;
		curWeight -= customers[picked].demand;

		visited.push_back(customers[picked]);
		currentNode = picked;

		//labmda 
		notVisited.erase(std::remove_if(notVisited.begin(), notVisited.end(),[&](const Customer& c){ return c.id == picked; }),notVisited.end());
	}
	// dodajemy do res id punktów
	for(Customer node : visited){
		res.push_back(node.id);
	}
	res.push_back(0);
	return 0;
}

long double generateInitialSolution(std::vector<Customer> customers, std::vector<std::vector<long double>> distanceMatrix, std::vector<int> &res, int N, int maxWeight){	
	res.push_back(0);
	int curWeight = maxWeight;
	std::vector<bool> visited(N,false);
	int visitedCount = 1, i,rclSize = 26, checkedRCL = 0;
	long double totalTime = 0, currentTruckTime = 0;
	std::vector<int> RCL;
	//Construct RCL
	makeRCL(RCL,customers,distanceMatrix,visited,currentTruckTime,0,N,rclSize);
	rclSize = RCL.size();
	i = rand()%rclSize;
	res.push_back(RCL[i]); visited[RCL[i]] = true;
	curWeight -= customers[RCL[i]].demand; currentTruckTime = distanceMatrix[0][RCL[i]];
	if(currentTruckTime < customers[RCL[i]].readyTime) currentTruckTime = customers[RCL[i]].readyTime;
	currentTruckTime += customers[RCL[i]].serviceTime;
	while( visitedCount < N){
		RCL.clear();
		makeRCL(RCL,customers,distanceMatrix,visited,currentTruckTime,res.back(),N,rclSize);
		if(RCL.size() == 0) break;
		i = rand()%RCL.size();
		//std::cout << RCL[i] << " " << std::flush;
		long double arriveTime = currentTruckTime + distanceMatrix[res.back()][RCL[i]];
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

std::vector<int> splitVector(std::vector<int> vector, int index) {
    std::vector<int> splitedVector;
    while (index < vector.size() && vector[index] != 0) {
		
        splitedVector.push_back(vector[index]);
        index++;
    }
    return splitedVector;
}

long double countDistance(std::vector<int> res, std::vector<Customer> customers, std::vector<std::vector<long double>> distanceMatrix){
	long double time = 0,truckTime=0;
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

bool isRouteValid(const std::vector<int> &route, const std::vector<Customer> &customers, const std::vector<std::vector<long double>> &distanceMatrix, int truckCapacity)
{
    long double totalTime = 0;
    long double currentTime = 0;
    int currentLoad = 0;
	for (int i = 0; i < route.size() -1 ; i++) {
		
        int from = route[i];
        int to = route[i + 1];
        long double travelTime = distanceMatrix[from][to];
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
void sortByDueTime(std::vector<int> &res, std::vector<Customer> customers){
	int N = res.size();
	for(int i = 0; i < N -1; i++){
		int minDueTime = customers[0].dueTime, minDueTimeI = i;
		for(int j = i; j < N; j++){
			if(customers[res[j]].dueTime < minDueTime) {minDueTime = customers[res[j]].dueTime; minDueTimeI = j;}
		}
		std::swap(res[minDueTimeI],res[i]);
	}
}

int main(int argc, char* argv[]){
	srand(time(NULL));
	using clock = std::chrono::steady_clock;
	auto start = clock::now();
	std::string fileName = argv[2];
	int maxseconds = atoi(argv[3]);
    int maxWeight = 200;
    std::vector<Customer> customers;
    readFile(argv[1], customers, maxWeight);
    int N = customers.size(), truckCount = 0, bestTruckCount = N;
    std::vector<int> bestRoute,tmpRoute, bestBestRoute;
    std::vector<std::vector<long double>> distanceMatrix(N,std::vector<long double>(N,0));
    std::vector<std::vector<double>> pheromoneMatrix(N,std::vector<double>(N,1));
	std::cout << "Print pheromone Matrix: ";
	printMatrix(pheromoneMatrix, N);
	long double bestDistance = 0, bestbestDistance = (unsigned int) -1;
    calculateDistance(N,distanceMatrix,customers);
	
	generateInitialSolutionANT(customers,distanceMatrix,pheromoneMatrix,bestRoute,N,maxWeight,1,1);
	printVector(bestRoute);
	//printVector(customers);
	bool ok = solutionCorrectnessCheck(customers, distanceMatrix);
	if(!ok){
		saveFile(bestRoute, -1,fileName);
	}

	while(true){
		truckCount = 0;
		N = customers.size();
		bestRoute.clear();
		tmpRoute.clear();
		bestDistance = generateInitialSolution(customers,distanceMatrix,bestRoute,N,maxWeight);
		bool foundImpr = false;
		tmpRoute = bestRoute;
		N = bestRoute.size();
		for(int i = 0; i<N-1;i++){
			if(bestRoute[i] == 0) truckCount++;
		}
		//merge
		if(bestRoute.size() > customers.size() + 1){
			int i=0,k,j=1,l=j+1;
			while(i < bestRoute.size() -2){
				k = i+1;
				while( k < bestRoute.size() -1 && bestRoute[k]!= 0) k++;
				while(j < bestRoute.size() -2){
					while( j< bestRoute.size() -2 && bestRoute[j] != 0) j++;
					l=j+1;
					while(l < bestRoute.size() -1 && bestRoute[l] != 0) l++;
					long double savings = distanceMatrix[bestRoute[k-1]][0] + distanceMatrix[bestRoute[j+1]][0] - distanceMatrix[bestRoute[k-1]][bestRoute[j+1]];
					if(savings > 0){
						std::vector<int> r1 = splitVector(bestRoute,i+1),r2 = splitVector(bestRoute,j+1);
						r1.insert(r1.end(),r2.begin(),r2.end());
						sortByDueTime(r1,customers);
						r2 = {0}; r2.insert(r2.end(),r1.begin(),r1.end());
						r2.push_back(0);
						if(isRouteValid(r2,customers,distanceMatrix,maxWeight)){
							r2.erase(r2.begin());
							bestRoute.erase(bestRoute.begin()+i+1,bestRoute.begin()+k+1);
							bestRoute.erase(bestRoute.begin()+j-k+i+1,bestRoute.begin()+l-k+i+1);
							bestRoute.insert(bestRoute.end(),r2.begin(),r2.end());
							k = i+1;
							while( k < bestRoute.size() -1 && bestRoute[k]!= 0) k++;
						}
					}
					j= l+1;
				}
				i=k;
			}
		}
		N = bestRoute.size();
		
		//2opt
		do{
			foundImpr = false;
			for(int i = 1; i < N-2; i++){
				if(tmpRoute[i] == 0) continue;
				for(int j = i+1; j < N-1; j++){
					if(tmpRoute[j] == 0) continue;
					long double dL = - distanceMatrix[tmpRoute[i]][tmpRoute[i+1]] - distanceMatrix[tmpRoute[j]][tmpRoute[j+1]] + distanceMatrix[tmpRoute[i+1]][tmpRoute[j+1]] + distanceMatrix[tmpRoute[i]][tmpRoute[j]];	
					swapEdges(tmpRoute,i,j);
					bool valid =  isRouteValid(tmpRoute,customers,distanceMatrix,maxWeight);
					if(dL < 0 && valid){
						long double tmpDistance = countDistance(tmpRoute,customers,distanceMatrix);
						if(tmpDistance >= bestDistance) continue;
						foundImpr = true;
						bestRoute = tmpRoute;
						bestDistance = tmpDistance;
					}else{
						tmpRoute = bestRoute;
					}
				}	
				auto now = clock::now();
				if(now - start >= std::chrono::seconds(maxseconds-10)){
					if(bestDistance < bestbestDistance){
						bestbestDistance = bestDistance;
						bestBestRoute = bestRoute;
					}
					saveFile(bestBestRoute, bestbestDistance,fileName);
					return 0;
				}
			}
		if(bestDistance < bestbestDistance){
			bestbestDistance = bestDistance;
			bestBestRoute = bestRoute;
		}
		}while(foundImpr);
		
		truckCount = 0;
		for(int i = 0; i<N-1;i++){
			if(bestRoute[i] == 0) truckCount++;
		};
		if(bestDistance < bestbestDistance){
			bestbestDistance = bestDistance;
			bestBestRoute = bestRoute;
		}
		auto now = clock::now();
		if(now - start >= std::chrono::seconds(maxseconds-10)){
			break;
		}
		
	}
	
	saveFile(bestBestRoute, bestbestDistance,fileName);
    return 0;

}