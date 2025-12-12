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


//declaration of functions
std::string formatData(const std::string& input);
std::vector<int> splitVector(std::vector<int> vector,int& index);

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


    int routeIndex = 0, routesNo = 0;
	for(int i = 0; i < routesVector.size()-1; i++)
		if(routesVector[i] == 0) routesNo++;
	//i believe routesLength and count will be passed seperately. ._.


	endFile << routesNo << " " << distance << " \n";

	for(int i = 1; i < routesVector.size(); i++){
		if(routesVector[i] !=0){
			endFile << routesVector[i] << " ";
			continue;
		}
		endFile << "\n";
	}
    endFile.close();
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

long double calculatePheromoneTrailANT(std::vector<Customer> customers,
									std::vector<std::vector<long double>> &distanceMatrix,
									std::vector<std::vector<double>> &pheromoneMatrix,
									int currentNode, int nextNode, int A, int B){
	long double vis = 1/(distanceMatrix[currentNode][nextNode]);
	//std::cout << "Licznik:" << vis << "\n";
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
void printMatrix(std::vector<std::vector<T>> M){
	std::streamsize ss = std::cout.precision();
	std::cout << std::setprecision(4);
	
	for(int i = 0; i < M.size() ;i++){
		for(int j = 0; j < M[i].size(); j++){
			std::cout << M[i][j] << "\t";
		}
		std::cout << "\n";
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
	routes.clear();
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
bool fixRoute(std::vector<int> r, std::vector<Customer> customers, std::vector<std::vector<long double>> distanceMatrix, int maxWeight){
	bool fixed = false;
	int curWeight = 0;
	long double curTime = 0;
	for(int i = 0; i < r.size()+1; i++){
		curTime = std::max(curTime + distanceMatrix[i-1 < 0 ? 0 : r[i-1]][r[i]],(long double)customers[r[i]].readyTime);
		if (curTime > customers[r[i]].dueTime){

		}
	}
	return fixed;
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
	long double time = 0;
	time = std::max(distanceMatrix[0][route.front()],(long double)customers[route.front()].readyTime) + customers[route.front()].serviceTime;
	for(int i = 0; i < route.size()-1; i++){
		long double arriveTime = time + distanceMatrix[route[i]][route[i+1]];
		time = std::max(arriveTime, (long double)customers[route[i+1]].readyTime);
		time += customers[route[i+1]].serviceTime;
	}
	time += distanceMatrix[route.back()][0];
	return time;
}

void swapEdges(std::vector<int> &res,int i, int j){
	i+=1;
	while(i < j){
		std::swap(res[i],res[j]);
		i++; j--;
	}
}

bool isRouteValid(const std::vector<int> &route, const std::vector<Customer> &customers, const std::vector<std::vector<long double>> &distanceMatrix, int truckCapacity)
{
    long double totalTime = 0;
    long double currentTime = std::max(distanceMatrix[0][route.front()], (long double) customers[route.front()].readyTime) + customers[route.front()].serviceTime;
    int currentLoad = customers.at(route[0]).demand;
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
    int maxIterations = 100, Q = 5, A= 7, B = 8;
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
    std::vector<int> bestRoute;
    std::vector<std::vector<long double>> distanceMatrix(N,std::vector<long double>(N,0));
    long double bestDistance = 0, bestbestDistance = (unsigned int) -1;
    calculateDistance(N,distanceMatrix,customers);
	std::vector<std::vector<int>> routes, bestRoutes; std::vector<long double> routeTimes;
	bool ok = solutionCorrectnessCheck(customers, distanceMatrix);
	if(!ok){
		saveFile(routes, -1,fileName);
	}
	std::vector<std::vector<double>>PheromoneIntensity(N,std::vector<double>(N,1));
	int l = 0;

	while(true){
		
		truckCount = 0;
		routes.clear();
		N = customers.size();
		bestRoute.clear();
		//std::cout << "generating new solution\n";
		generateInitialSolutionANT(customers,distanceMatrix,PheromoneIntensity,bestRoute,N,maxWeight,A,B);
		bestDistance = countDistance(bestRoute,customers,distanceMatrix);
		//printVector(bestRoute);
		bool foundImpr = false;
		for(int i = 0; i<N-1;i++){
			if(bestRoute[i] == 0) truckCount++;
		}
		//printVector(bestRoute);
		//tmpRoute = bestRoute;
		N = bestRoute.size();
		double Pmin = Q/(double)customers.size();
        double Pmax = Q/(double)truckCount;
		
		splitRoute(routes,bestRoute);
		//printMatrix(routes);
		for(int j = 0; j < routes.size();j++){
			routeTimes.push_back(countAntTime(routes[j],customers,distanceMatrix));
		}
		//printVector(bestRoute);
		//std::cout << bestDistance;

        for(int i = 0; i < maxIterations;i++){

			auto now = clock::now();
			if(now - start >= std::chrono::seconds(maxseconds-5)){
				//printVector(routeTimes);
				long double L = 0;
				for(int j = 0; j < routeTimes.size(); j++){
					L += routeTimes[j];
				}
				bestDistance = L;
				if(bestDistance < bestbestDistance){
					bestRoutes = routes;
					bestbestDistance = bestDistance;
				}
				connectRoute(bestRoutes,bestRoute);
				bestbestDistance = countDistance(bestRoute,customers,distanceMatrix);
				saveFile(bestRoutes, bestbestDistance,fileName);
				std::cout << bestbestDistance;
				return 0;
			}
		
			//printMatrix(routes);
			//printVector(routeTimes);
			//MUTATION
            double Pm = Pmin + std::pow(Pmax - Pmin,1-(i)/(double)maxIterations);
			//std::cout << Pm << "\n";
			int minI = 0,minI2 = 0;
			int minJ = 0; int minJ2 = 0 ;
			
			//std::cout << "mutate\n";
			std::vector<std::vector<double>> randValues(routes.size());
			randValues.clear();

			for(int j = 0; j < routes.size(); j++){
				for(int k = 0; k < routes[j].size(); k++){
					randValues[j].push_back((rand()%10000)/10000.0);
					if(randValues[j][k] < randValues[minI][minJ]){
						minI = j; minJ = k;
					}
				}
			}
			//std::cout << "mutate2\n";
			for(int j = 1;j < randValues.size(); j++){
				if (j == minI) continue;
				for(int k = 0; k < randValues[j].size();k++){
					if(randValues[j][k] > Pm) continue;
					if(randValues[j][k] < randValues[minI2][minJ2]){
						minI2 = j; minJ2 = k;
					}
				}
			}
			//std::cout << "mutate4 " << l << "\n";
			//std::cout << minI << " " << minI2 << "\n";
			if(randValues[minI][minJ] < Pm && randValues[minI2][minJ2] < Pm){
				std::swap(routes[minI][minJ], routes[minI2][minJ2]);
			}
			

			bool swappedBack = false;
			//std::vector<int> r = routes[maxLenI];
			long double antTime = 0;
			int currWeight = maxWeight;
			//std::cout << "fixing mutation\n" ;
			if(!isRouteValid(routes[minI],customers,distanceMatrix,maxWeight) || !isRouteValid(routes[minI2],customers,distanceMatrix,maxWeight)){
				std::swap(routes[minI][minJ], routes[minI2][minJ2]);
			}else{
				routeTimes[minI] = countAntTime(routes[minI],customers,distanceMatrix);
				routeTimes[minI2] = countAntTime(routes[minI2],customers,distanceMatrix);
			}

		
			//std::cout << "local search1\n";
			//LOCAL SEARCH
			//bestDistance = countDistance(bestRoute,customers,distanceMatrix);
			//merge
			//printMatrix(routes);
			for(int j = 0; j < routes.size()-1; j++){
				for(int k = j+1; k < routes.size(); k++){
					if (k == j) continue;
					long double savings = distanceMatrix[bestRoute[k-1]][0] + distanceMatrix[bestRoute[j+1]][0] - distanceMatrix[bestRoute[k-1]][bestRoute[j+1]];
					if(savings > 0){
						std::vector<int> r = routes[j];
						
						r.insert(r.end(),routes[k].begin(),routes[k].end());
						//printVector(r);
						bool v = isRouteValid(r,customers,distanceMatrix,maxWeight);
						//std::cout << (v ? "true" : "false" )<< "\n";
						if(v){
							routes[j] = r; routes.erase(routes.begin() + k);
							routeTimes.erase(routeTimes.begin() + k);
							k--;
						}
					}
				}
			}
			//printMatrix(routes);
			//std::cout << "local search2\n";
			now = clock::now();
			if(now - start >= std::chrono::seconds(maxseconds-5)){
				long double L = 0;
				for(int j = 0; j < routeTimes.size(); j++){
					L += routeTimes[j];
				}
				bestDistance = L;

				if(bestDistance < bestbestDistance){
					bestRoutes = routes;
					bestbestDistance = bestDistance;
				}
				
				connectRoute(bestRoutes,bestRoute);
				bestbestDistance = countDistance(bestRoute,customers,distanceMatrix);
				saveFile(bestRoutes, bestbestDistance,fileName);
				std::cout << bestbestDistance;
				return 0;
			}
		
			//2opt
			for(int j = 0; j < routes.size(); j++){
				int N = routes[j].size();
				for(int k = 0; k < N-2;k++){
					for(int l = k + 1; l < N-1;l++){
						long double dL = distanceMatrix[routes[j][k]][routes[j][k+1]] + distanceMatrix[routes[j][l]][routes[j][l+1]] - distanceMatrix[routes[j][k]][routes[j][l+1]] - distanceMatrix[routes[j][l]][routes[j][k+1]];
						if(dL > 0){
							std::vector<int> tmp = routes[j];
							std::reverse(tmp.begin() + k + 1, tmp.begin() + l);
							//printVector(tmp);
							if(isRouteValid(tmp,customers,distanceMatrix,maxWeight)){
								routes[j] = tmp;
								routeTimes[j] = countAntTime(routes[j],customers,distanceMatrix);
							}else{
								tmp = routes[j];
							}
						}
					}
				}
			}
			//std::cout << "count times\n";
			long double L = 0;
			for(int j = 0; j < routeTimes.size(); j++){
				L += routeTimes[j];
			}
			bestDistance = L;

			//std::cout << "local update\n";
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

			now = clock::now();
			if(now - start >= std::chrono::seconds(maxseconds-5)){
				if(bestDistance < bestbestDistance){
					bestRoutes = routes;
					bestbestDistance = bestDistance;
				}
				connectRoute(bestRoutes,bestRoute);
				bestbestDistance = countDistance(bestRoute,customers,distanceMatrix);
				saveFile(bestRoutes, bestbestDistance,fileName);
				std::cout << bestbestDistance;
				return 0;
			}

        }
		l++;
		//printVector(routeTimes);
		//connectRoute(routes,bestRoute);
		//GLOBAL UPDATING
		for(int j = 0; j < customers.size(); j++){
			for(int k = j+1; k < customers.size(); k++){
				PheromoneIntensity[j][k] *= Rho; PheromoneIntensity[j][k] + Q/bestDistance;
				PheromoneIntensity[k][j] = PheromoneIntensity[j][k];
			}
		}

					long double L = 0;
			for(int j = 0; j < routeTimes.size(); j++){
				L += routeTimes[j];
			}
			bestDistance = L;

		auto now = clock::now();
		if(now - start >= std::chrono::seconds(maxseconds-10)){
			break;
		}
		
	}
		long double L = 0;
			for(int j = 0; j < routeTimes.size(); j++){
				L += routeTimes[j];
			}
			bestDistance = L;
		if(bestDistance < bestbestDistance){
			bestRoutes = routes;
			bestbestDistance = bestDistance;
		}
		connectRoute(bestRoutes,bestRoute);
				bestbestDistance = countDistance(bestRoute,customers,distanceMatrix);
				
	std::cout << std::setprecision(5) <<  bestbestDistance;
	//printMatrix(PheromoneIntensity,customers.size());
	//std::cout << "end\n";
	saveFile(bestRoutes, bestbestDistance,fileName);
    return 0;
	

}