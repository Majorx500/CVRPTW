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

struct Vehicle{
    int vehicleNumber;
    int vehicleCapacity;
};

//declaration of functions
std::string formatData(const std::string& input);
std::vector<int> splitVector(std::vector<int> vector,int& index);


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



void saveFile(std::vector<std::vector<int>> &routesVector, long double distance, std::string fileName) {

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

	//i believe routesLength and count will be passed seperately. ._.


	endFile << routesVector.size() << " " << distance << " \n";

	for(int i = 0; i < routesVector.size(); i++){
		for(int a : routesVector[i]){
			endFile << a << " ";
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
	
	for(int i = 0; i < N ;i++){
		for(int j = 0; j < N; j++){
			std::cout << M[i][j] << "\t";
		}
		std::cout << "\n";
	}
}

void makeRCL(std::vector<int> &rcl, std::vector<Customer> customers, std::vector<std::vector<long double>> distanceMatrix,std::vector<bool> visited, long double truckTime, int l,int N, int listSize){
	//ReadyTime - Distance
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


std::vector<int> splitVector(std::vector<int> vector, int& index) {
    std::vector<int> splitedVector;
    while (index < vector.size() && vector[index] != 0) {
		
        splitedVector.push_back(vector[index]);
        index++;
    }
    return splitedVector;


}

int splitRoute(std::vector<std::vector<int>>& routes,std::vector<int> route){
	int i = 1;
	std::vector<int> currentRoute;
	while( i < route.size()){
		std::vector<int> currentRoute = splitVector(route,i);
		i++; routes.push_back(currentRoute);
	}
	return 0;
}

int connectRoute(std::vector<std::vector<int>> routes, std::vector<int>& route){
	route.clear(); route.push_back(0);
	for(int i = 0; i < routes.size(); i++){
		for(int j = 0; j < routes[i].size(); j++){
			route.push_back(routes[i][j]);
		}
		route.push_back(0);
	}
	return 0;
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

long double countAntTime(std::vector<int> route, std::vector<Customer> customers, std::vector<std::vector<long double>> distanceMatrix){
	long double time;
	time = std::max(time + distanceMatrix[0][route[0]], time + customers[route[0]].readyTime) + customers[route[0]].serviceTime;
	for(int i = 0; i < route.size()-1; i++){
		time = std::max(time + distanceMatrix[i][route[i+1]], time + customers[route[i+1]].readyTime) + customers[route[i+1]].serviceTime;
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
    int maxIterations = 10; int Q = 5;
	srand(time(NULL));
	using clock = std::chrono::steady_clock;
	auto start = clock::now();
	std::string fileName = argv[2];
	int maxseconds = atoi(argv[3]);
    int maxWeight = 200;
	double Rho = 0.85;
    std::vector<Customer> customers;
    readFile(argv[1], customers, maxWeight);
    int N = customers.size(), truckCount = 0, bestTruckCount = N;
    std::vector<int> bestRoute,tmpRoute, bestBestRoute;
    std::vector<std::vector<long double>> distanceMatrix(N,std::vector<long double>(N,0));
    long double bestDistance = 0, bestbestDistance = (unsigned int) -1;
    calculateDistance(N,distanceMatrix,customers);
	std::vector<std::vector<int>> routes; std::vector<long double> routeTimes;
	bool ok = solutionCorrectnessCheck(customers, distanceMatrix);
	if(!ok){
		saveFile(routes, -1,fileName);
	}
			std::vector<std::vector<long double>>PheromoneIntensity(N,std::vector<long double>(N,1));

	while(true){
		truckCount = 0;
		N = customers.size();
		bestRoute.clear();
		tmpRoute.clear();
		generateInitialSolution(customers,distanceMatrix,bestRoute,N,maxWeight);
		bestDistance = countDistance(bestRoute,customers,distanceMatrix);
		bool foundImpr = false;
		for(int i = 0; i<N-1;i++){
			if(bestRoute[i] == 0) truckCount++;
		}
		//printVector(bestRoute);
		tmpRoute = bestRoute;
		N = bestRoute.size();
		double Pmin = Q/(double)customers.size();
        double Pmax = Q/(double)truckCount;
		std::vector<double> rndValues(tmpRoute.size());
		
		splitRoute(routes,bestRoute);
		//printVector(bestRoute);
		for(int j = 0; j < routes.size();j++){
			routeTimes.push_back(countAntTime(routes[j],customers,distanceMatrix));
		}
        for(int i = 0; i < maxIterations;i++){

			//MUTATION
            double Pm = Pmin + std::pow(Pmax - Pmin,1-(i)/(double)maxIterations);
			//std::cout << Pm << "\n";
			int maxLenI = 1,max2LenI = 0;
			for(int j = 0; j < routeTimes.size();j++){
				if(routeTimes[j] > routeTimes[maxLenI]){
					max2LenI = maxLenI;
					maxLenI = j;
				}
			}
			std::vector<double> randValues;
			for(int j = 0; j < routes[maxLenI].size() + routes[max2LenI].size(); j++){
				randValues.push_back((rand()%10000)/10000.0);
			}
			int minChI1 = 0; int minCHI2 =routes[maxLenI].size();
			for(int j = 1;j < routes[maxLenI].size(); j++){
				if(randValues[j] > Pm) continue;
				if(randValues[j] < randValues[minChI1]) minChI1 = j;
			}
			for(int j = routes[maxLenI].size() + 1; j < randValues.size(); j++){
				if(randValues[j] > Pm) continue;
				if(randValues[j] < randValues[minCHI2]) minCHI2 = j;
			}
			if(randValues[minChI1] < Pm && randValues[minCHI2] < Pm){
				std::swap(routes[maxLenI][minChI1], routes[max2LenI][minCHI2 - routes[maxLenI].size()]);
				routeTimes[maxLenI] = countAntTime(routes[maxLenI],customers,distanceMatrix);
				routeTimes[max2LenI] = countAntTime(routes[max2LenI],customers,distanceMatrix);
			}

			bool swapped = false;
			long double antTime = distanceMatrix[0][routes[maxLenI][0]] + customers[routes[maxLenI][0]].serviceTime;
			for(int j = 1; j < routes[maxLenI].size();j++){
				antTime = std::max(antTime + distanceMatrix[routes[maxLenI][j-1]][routes[maxLenI][j]],(long double)customers[routes[maxLenI][j]].readyTime);
				
				if(antTime > customers[routes[maxLenI][j]].dueTime){
					long double tmp = antTime - distanceMatrix[routes[maxLenI][j-1]][routes[maxLenI][j]];
					for(int k = j-1; k > 0; k++){
						if(tmp < customers[routes[maxLenI][j]].dueTime && antTime -(distanceMatrix[routes[maxLenI][j-1]][routes[maxLenI][j]] + distanceMatrix[k-1 < 0 ? 0 :routes[max2LenI][k-1]][routes[maxLenI][k]]) + distanceMatrix[k-1 < 0 ? 0 :routes[maxLenI][k-1]][routes[maxLenI][j]] + distanceMatrix[routes[maxLenI][j-1]][routes[maxLenI][k]]){
							int r = routes[maxLenI][j];
							routes[maxLenI].erase(routes[maxLenI].begin() + j);
							routes[maxLenI].insert(routes[maxLenI].begin() + k,r);
							swapped = true;
							break;
						}
					}
					if(!swapped){ 
						std::swap(routes[maxLenI][minChI1], routes[max2LenI][minCHI2 - routes[maxLenI].size()]);
						break;
					}
				}

			}
			
			if(swapped){
				swapped = false;
				antTime =0;
				for(int j = 0; j < routes[max2LenI].size(); j++){
					antTime = std::max(antTime + distanceMatrix[routes[max2LenI][j-1]][routes[max2LenI][j]],(long double)customers[routes[max2LenI][j]].readyTime);
					if(antTime > customers[routes[max2LenI][j]].dueTime){
						long double tmp = antTime - distanceMatrix[routes[max2LenI][j-1]][routes[max2LenI][j]];
						for(int k = j-1; k >= 0; k--){
							std::cout << k << "\t";
							if(tmp < customers[routes[max2LenI][j]].dueTime && antTime -(distanceMatrix[routes[max2LenI][j-1]][routes[max2LenI][j]] + distanceMatrix[k-1 < 0 ? 0 :routes[max2LenI][k-1]][routes[max2LenI][k]]) + distanceMatrix[k-1 < 0 ? 0 :routes[max2LenI][k-1]][routes[max2LenI][j]] + distanceMatrix[routes[max2LenI][j-1]][routes[max2LenI][k]]){
								int r = routes[max2LenI][j];
								routes[max2LenI].erase(routes[max2LenI].begin() + j);
								routes[max2LenI].insert(routes[max2LenI].begin() + k,r);
								swapped = true;
								break;
							}
						}
						if(!swapped){ 
							std::swap(routes[maxLenI][minChI1], routes[max2LenI][minCHI2 - routes[maxLenI].size()]);
							break;
						}
					}

				}
			}

			//LOCAL SEARCH
			bestDistance = countDistance(bestRoute,customers,distanceMatrix);
			//LOCAL UPDATE
			for(int j = 0; j < customers.size(); j++){
				for(int k = j+1; k < customers.size(); k++){
					PheromoneIntensity[j][k] *= Rho;
					PheromoneIntensity[k][j] = PheromoneIntensity[j][k];
				}
			}
			int routeNo = 0;
			for(int j = 1; j < bestRoute.size()-1; j++){
				double dT = Q / (routes.size() * bestDistance) * ((routeTimes[routeNo] - distanceMatrix[bestRoute[j-1]][bestRoute[j]])/(routes[routeNo].size() * routeTimes[routeNo]));
				PheromoneIntensity[bestRoute[j-1]][bestRoute[j]] += dT;
				if(bestRoute[j] == 0) routeNo++;
			}
        }
		
		connectRoute(routes,bestRoute);
		//GLOBAL UPDATING
		for(int j = 0; j < customers.size(); j++){
			for(int k = j+1; k < customers.size(); k++){
				PheromoneIntensity[j][k] *= Rho + Q/bestDistance;
				PheromoneIntensity[k][j] = PheromoneIntensity[j][k];
			}
		}
		auto now = clock::now();
		if(now - start >= std::chrono::seconds(maxseconds-10)){
			break;
		}

	}
	printMatrix(PheromoneIntensity,customers.size());

	saveFile(routes, bestDistance,fileName);
    return 0;

}