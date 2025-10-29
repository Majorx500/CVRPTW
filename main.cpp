#include <regex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sstream>

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
    std::cout << "Print Hello World\n";
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
                        std::cout << line << "\n";
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
//formats data given by the file to standarized output
std::string formatData(const std::string& input){
    return std::regex_replace(input, std::regex(" {2,}"), " ");
    
}

int main(int argc, char* argv[]){
    std::vector<Customer> customers;
    std::cout << argv[1];
    readFile(argv[1], customers);
 
    //display added customers;
    for(int i=0; i<=customers.size(); i++){
        std::cout << customers.at(i).id << " ";
        std::cout << customers.at(i).x << " ";
        std::cout << customers.at(i).y << " ";
        std::cout << "\n";
    }
    return 0;

}