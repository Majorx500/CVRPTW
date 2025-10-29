#include <regex>
#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>

//declaration of structures
struct Customer{
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
                //get only clients' coordinates;
                if(lineCount >= 7){
                    std::cout << line << "\n";
                }
                lineCount++;
            }    
    }else{
        std::cout << "File wasn't open";
    }
       file.close();

}

std::string formatData(const std::string& input){
    return std::regex_replace(input, std::regex(" {2,}"), " ");
    
}

int main(int argc, char* argv[]){
    std::vector<Customer> customers;
    std::cout << argv[1];
    readFile(argv[1], customers);


    return 0;

}