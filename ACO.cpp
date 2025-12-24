#include <cstring>
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
#include <algorithm>
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

bool solutionCorrectnessCheck(std::vector<Customer> &customers, std::vector<std::vector<long double>> &distanceMatrix, int maxCapacity) {
    int depot = 0;

    for(int i = 1; i < customers.size(); i++) {
        float arrival = distanceMatrix[depot][i];
        float startService = std::max(arrival, (float)customers[i].readyTime);
        float backToDepot = startService + customers[i].serviceTime + distanceMatrix[i][depot];

        if(arrival > customers[i].dueTime || backToDepot > customers[depot].dueTime || customers[i].demand > maxCapacity) {
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
	endFile << std::fixed <<  std::setprecision(6);
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
			endFile << a << " ";	}
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
									   std::vector<std::vector<int>> &res, int N, int maxWeight, int A, int B,
                     std::vector<long double> &routeLen){	
	
	std::vector<Customer> notVisited = customers;
	std::vector<Customer> visited;
	std::vector<PheromonePair> pheroList;
	std::vector<PheromonePair> probList;

  routeLen.clear();
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
			if (arrivalTime > el.dueTime || arrivalTime + el.serviceTime + distanceMatrix[el.id][0] > customers[0].dueTime) continue; //jeżeli za późno to skip
			
			PheromonePair pheromone;
			pheromone.index = el.id;
			pheromone.pheromone = calculatePheromoneTrailANT(customers, distanceMatrix,pheromoneMatrix, currentNode,el.id, A, B);

			total += pheromone.pheromone; //potrebujemy do Sum(pi * vis)
			pheroList.push_back(pheromone);
		}
		// tutaj wracamy do depotu 
		if (pheroList.empty()) {
      currentTruckTime += distanceMatrix[currentNode][0];
			routeLen.push_back(currentTruckTime);
      totalTime += currentTruckTime;
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
  currentTruckTime += distanceMatrix[currentNode][0];
  routeLen.push_back(currentTruckTime);
	// dodajemy do res id punktów
  std::vector<int> tmp;
  visited.erase(visited.begin());
  routeLen.erase(routeLen.begin());
  for(Customer node : visited){
    if(node.id == 0){
      res.push_back(tmp);
      tmp.clear(); continue;
    }
		tmp.push_back(node.id);
	}
  res.push_back(tmp);
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

long double countAntTime(const std::vector<int> route,const std::vector<Customer> customers, const std::vector<std::vector<long double>> distanceMatrix){
	long double time = 0;
	const Customer& c = customers[route.front()];
  time = std::max((long double)c.readyTime, distanceMatrix[0][c.id]);
  time += c.serviceTime;
	for(int i = 1; i < route.size(); i++){
    const Customer& c = customers[route[i]];const Customer& c2 = customers[route[i-1]];
		long double arriveTime = time + distanceMatrix[c2.id][c.id];
		time = std::max(arriveTime, (long double)c.readyTime);
		time += c.serviceTime;
	}
	time += distanceMatrix[route.back()][0];
	return time;
}

long double countAntTimes(const std::vector<std::vector<int>> routes, const std::vector<Customer> customers, const std::vector<std::vector<long double>> distanceMatrix, std::vector<long double> &routeTimes){
	routeTimes.clear(); routeTimes.reserve(routes.size());
  long double time, totalTime = 0;;
  for(int j = 0; j < routes.size(); j++){
    const std::vector<int>& route = routes[j];
    time = 0;
	  const Customer& c = customers[route.front()];
    time = std::max((long double)c.readyTime, distanceMatrix[0][c.id]);
    time += c.serviceTime;
	  for(int i = 1; i < route.size(); i++){
      const Customer& c = customers[route[i]];const Customer& c2 = customers[route[i-1]];
	  	long double arriveTime = time + distanceMatrix[c2.id][c.id];
	  	time = std::max(arriveTime, (long double)c.readyTime);
		  time += c.serviceTime;
	  }
  	time += distanceMatrix[route.back()][0];
    totalTime += time;
    routeTimes.push_back(time);
  }
  return totalTime;
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
    if( currentTime + distanceMatrix[route.back()][0] > customers[0].dueTime) return false;
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
    int maxIterations = 100, Q = 5, A= 6, B = 12;
	srand(time(NULL));
	using clock = std::chrono::steady_clock;
	auto start = clock::now();
	std::string fileName = argv[2];
	int maxseconds = atoi(argv[3]);
  int maxWeight;
	double Rho = 0.85;
  std::vector<Customer> customers;
  readFile(argv[1], customers, maxWeight);
  int N = customers.size(), truckCount = 0, bestTruckCount = N, minI, minI2, minJ, minJ2;
  std::vector<int> bestRoute;
  std::vector<std::vector<long double>> distanceMatrix(N,std::vector<long double>(N,0));
  long double bestDistance = 0, bestbestDistance = (unsigned int) -1,L;
  calculateDistance(N,distanceMatrix,customers);
	std::vector<std::vector<int>> routes, bestRoutes; std::vector<long double> routeTimes;
	bool ok = solutionCorrectnessCheck(customers, distanceMatrix,maxWeight);
	if(!ok){
		saveFile(routes, -1,fileName);
	}
	std::vector<std::vector<double>>PheromoneIntensity(N,std::vector<double>(N,1));
  
	while(true){
		
		truckCount = 0;
		routes.clear();
		N = customers.size();
		bestRoute.clear();
		//std::cout << "generating new solution\n";
		generateInitialSolutionANT(customers,distanceMatrix,PheromoneIntensity,routes,N,maxWeight,A,B,routeTimes);
	//	printVector(routeTimes);
    //routeTimes.clear();
    for(int i = 0; i < routes.size(); i++){
    //routeTimes.push_back(countAntTime(routes[i],customers,distanceMatrix));
    }
    //printVector(routeTimes);
   // printMatrix(routes);
    //bestDistance = countDistance(bestRoute,customers,distanceMatrix);
		N = routes.size();
		double Pmin = Q/(double)customers.size();
    double Pmax = Q/(double)routes.size();
		for(int i = 0; i < maxIterations;i++){

			auto now = clock::now();
			if(now - start >= std::chrono::seconds(maxseconds-5)){
				//printVector(routeTimes);
        bestDistance = countAntTimes(routes,customers,distanceMatrix,routeTimes);

				if(bestDistance < bestbestDistance){
          bestRoutes = std::move(routes);
          bestbestDistance = bestDistance;
        }
        saveFile(bestRoutes, bestbestDistance,fileName);
				return 0;
			}
		
			//MUTATION
     // std::cout << "mutete\n";
      
      double Pm = Pmin + std::pow(Pmax - Pmin,1-(i)/(double)maxIterations);
			//std::cout << Pm << "\n";
			minI = -1;minI2 = -1;
			minJ = -1; minJ2 = -1;
			long double minRand = 1, tmp;
      
     // std::cout << "m1\n";

			for(int j = 0; j < routes.size(); j++){
				for(int k = 0; k < routes[j].size(); k++){
          tmp = rand() / (long double) RAND_MAX;
          if(tmp >= minRand || tmp > Pm) continue;
						minI = j; minJ = k; minRand = tmp;
				}
			}

      //std::cout << "m2\n";
      if(minI != -1){
      minRand = 1;
			  for(int j = 1;j < routes.size(); j++){
				  if (j == minI) continue;
				  for(int k = 0; k < routes[j].size();k++){
            tmp = rand() / (long double) RAND_MAX;
					  if(tmp > Pm || tmp >= minRand) continue;
					  	minI2 = j; minJ2 = k; 
				  }
			  }
      }
			if(minJ != -1 && minJ2 != -1){
				std::swap(routes[minI][minJ], routes[minI2][minJ2]);
			}
			
      //std::cout << "m3\n";
      //std::cout <<"A: " <<  minI << " " << minJ << "\t" << minI2 << " " << minJ2 << "\n";

			if(minJ != -1 && minJ2 != -1 && (!isRouteValid(routes[minI],customers,distanceMatrix,maxWeight) || !isRouteValid(routes[minI2],customers,distanceMatrix,maxWeight)) ){
      	std::swap(routes[minI][minJ], routes[minI2][minJ2]);
			}else if (minJ != -1 && minJ2 != -1){
        routeTimes[minI] = countAntTime(routes[minI],customers,distanceMatrix);
				routeTimes[minI2] = countAntTime(routes[minI2],customers,distanceMatrix);
			}

			//LOCAL SEARCH
      //std::cout << "merge\n";
			for(int j = 0; j < routes.size()-1; j++){
				for(int k = j+1; k < routes.size(); k++){
					long double savings = distanceMatrix[routes[j].back()][0] + distanceMatrix[routes[k].front()][0] - distanceMatrix[routes[j].back()][routes[k].front()];
					if(savings > 0){
						std::vector<int> r = routes[j];
            //printVector(r);
						r.insert(r.end(),routes[k].begin(),routes[k].end());
				
						if(isRouteValid(r,customers,distanceMatrix,maxWeight)){
              routes[j] = std::move(r);
              routes.erase(routes.begin() + k);
              routeTimes[j] = routeTimes[j] - savings + routeTimes[k];
							routeTimes.erase(routeTimes.begin() + k);
							k--;
						}
					}
				}
			}
			now = clock::now();
			if(now - start >= std::chrono::seconds(maxseconds-5)){
        //printVector(routeTimes);				
				bestDistance = countAntTimes(routes,customers,distanceMatrix,routeTimes);

				if(bestDistance < bestbestDistance){
					bestRoutes = routes;
					bestbestDistance = bestDistance;
				}
				saveFile(bestRoutes, bestbestDistance,fileName);
				return 0;
			}
		
			//2opt
      //std::cout << "2opt\n";
			for(int j = 0; j < routes.size(); j++){
				int N = routes[j].size();
				for(int k = 0; k < N-2;k++){
					for(int l = k + 3; l < N-1;l++){
						long double dL = -distanceMatrix[routes[j][k]][routes[j][k+1]] - distanceMatrix[routes[j][l]][routes[j][l+1]] + distanceMatrix[routes[j][k]][routes[j][l]] + distanceMatrix[routes[j][l+1]][routes[j][k+1]];
						if(dL < 0){
							std::vector<int> tmp = routes[j];
              int m = k+1, n = l;
                while(n > m){
                 std::swap(tmp[m],tmp[n]);
                  m++;n--;
               } 

							  if(isRouteValid(tmp,customers,distanceMatrix,maxWeight)){
                  //std::cout << k << " " << l << " " << dL << " " << routeTimes[j] <<  "\n";
                  //printVector(routes[j]);
                  //printVector(tmp);
                  routes[j] = std::move(tmp);                 
                  routeTimes[j] += dL;
							 }
              
						}
					}
				}
			}
      
			L = 0;
			for(int j = 0; j < routeTimes.size(); j++){
				L += routeTimes[j];
        //std::cout << routeTimes[j] << " ";
			}
			bestDistance = L;

			//LOCAL UPDATE
      //std::cout << "local Update\n";
			for(int j = 0; j < customers.size(); j++){
				for(int k = j+1; k < customers.size(); k++){
					PheromoneIntensity[j][k] *= Rho;
					PheromoneIntensity[k][j] = PheromoneIntensity[j][k];
				}
			}

     // std::cout << "Local Update\n";
      double dT;
			for(int l = 0; l < routes.size(); l++){
			  dT = Q / (routes.size() * bestDistance) * ((routeTimes[l] - distanceMatrix[0][routes[l].front()])/(routes[l].size() * routeTimes[l]));
        PheromoneIntensity[0][routes[l].front()] += dT;
        for(int j = 1; j < routes[l].size(); j++){
				  dT = Q / (routes.size() * bestDistance) * ((routeTimes[l] - distanceMatrix[routes[l][j-1]][routes[l][j]])/(routes[l].size() * routeTimes[l]));
				  PheromoneIntensity[routes[l][j-1]][routes[l][j]] += dT;
			  }
        dT = Q / (routes.size() * bestDistance) * ((routeTimes[l] - distanceMatrix[0][routes[l].back()])/(routes[l].size() * routeTimes[l]));
        PheromoneIntensity[0][routes[l].back()] += dT;

      }
       now = clock::now();
       if(now - start > std::chrono::seconds(maxseconds-5)) break;
    }
     //std::cout << "Global Update\n";
		for(int j = 0; j < customers.size(); j++){
			for(int k = j+1; k < customers.size(); k++){
				PheromoneIntensity[j][k] *= Rho; PheromoneIntensity[j][k] += Q/bestDistance;
				PheromoneIntensity[k][j] = PheromoneIntensity[j][k];
			}
		}

		auto now = clock::now();
		if(now - start >= std::chrono::seconds(maxseconds-10)){
			break;
		}
		
  	}
    //printVector(routeTimes);
	  bestDistance = countAntTimes(routes,customers,distanceMatrix,routeTimes);

		if(bestDistance < bestbestDistance){
      bestRoutes = std::move(routes);
			bestbestDistance = bestDistance;
		}
	  saveFile(bestRoutes, bestbestDistance,fileName);
    return 0;
}
