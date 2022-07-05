#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <iterator>
#include <cctype>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <set>
#include <iomanip>
#include <ctime>

using namespace std;

vector<string> stv(string data, char delimiter){
    stringstream s(data);

    vector<string> _v;
    string w;

    while(getline(s,w,delimiter)){
        _v.push_back(w);
    }

    return _v;
}

string cmd(vector<string> input){
    return boost::algorithm::to_lower_copy(input[0]);
}


void displayVector(vector<string> _vect){
    copy(_vect.begin(), _vect.end(),ostream_iterator<string>(cout, " "));
}

void displayMap(map<string,vector<string>>* list){
    map<string,vector<string> >::iterator it;

    for (it = list->begin(); it != list->end(); it++)
    {
        cout << it->first << "\t\t";
        displayVector(it->second);
        cout << endl;
    }
}


class CSV{
    private:
        const char *filename;

    public:
        CSV(const char *filename);
        map<string, vector<string>> selectAll();
        vector<string> selectById(string id);
        int updateById(string id,string data);
        int updateById(string id,vector<string>* data);
        int deleteById(string id);
        int deleteBy(string value,int colIndex);
        string add(string data);
        vector<string> add(vector<string>* data);
};

class Location {
    public:
        string addLocation(string name);
        bool deleteLocation(string locationName);
        void listLocations();
};


class Disease{
    public:
        vector<string> addDisease(string name,string loation,int cases);
        vector<string> findDiseaseLocation(string diseaseName);
        set<string> listDiseases();
        void summary();
        int cases(string diseaseName,string location);
        int cases(string diseaseName);
};

 //CSV
CSV :: CSV(const char *filename){
    this->filename = filename;
}

map<string, vector<string>> CSV::selectAll(){
    fstream file(this->filename);

    map<string, vector<string>> dataSet;
    vector<string> row;
    string word,line;

    while(getline(file,line)){
        row.clear();
        stringstream s(line);

        while (getline(s, word, ',')) {
            row.push_back(word);
        }

        dataSet.insert({row[0], row});

        cout << endl;
    }

    //close file after reading
    file.close();

    return dataSet;
}



vector<string> CSV::selectById(string id){
    fstream file(this->filename);

    vector<string> foundRow;
    string line,word;
    int foundId;

    while(getline(file,line)){
        foundRow.clear();
        stringstream s(line);

        while(getline(s,word,',')){
            foundRow.push_back(word);
        }

        // assuming that every first element will be interger
        // foundId = stoi(foundRow[0]);

        if( foundRow[0] == id)
            return foundRow;
    }

    file.close();
    throw 404;
}


int CSV::updateById(string id, string data){
    // NOTE: i know this function has stupid logic that could never be done in large enterprise application
    // rewriting whole file 😒, i understands how overwhelming it is  and how poor perfomant it is
    // but you need to understand that i was rushing and this was fast for me, or if you have time open PR
    // dealing with file pointers is kinda pain


    fstream file(this->filename);
    ofstream temp;
    int updatedRows =0;

    string line,identity, tempFileName = "temp.csv";

    temp.open(tempFileName);

    while(getline(file,line)){

        stringstream s(line);
        
        getline(s,identity,',');

        if(id == identity){
            line.replace(line.find(line),line.length(),data);
            updatedRows++;
        }
        temp << line << "\n";


    }

    temp.close();
    file.close();

    remove(this->filename);
    rename(const_cast<char*>(tempFileName.c_str()),this->filename);

    return updatedRows;
}


int CSV::updateById(string id, vector<string> *data){
    string dataString = "";

    for(auto & d : *data){
        if(dataString.length() > 0)
            dataString +=",";
        dataString +=d;
    }

    return this->updateById(id,dataString);
}


int CSV::deleteById(string id){

    fstream file(this->filename);
    ofstream temp;
    int deleted = 0;

    string line,identity, tempFileName = "temp.csv";

    temp.open(tempFileName);
    
    // write headers
    getline(file,line);
    temp << line << "\n" ;

    while(getline(file,line)){

        stringstream s(line);

        getline(s,identity,',');

        if(id != identity){
            temp << line << "\n" ;
        }else {
            deleted += 1;
        }


    }

    temp.close();
    file.close();

    remove(this->filename);
    rename(const_cast<char*>(tempFileName.c_str()),this->filename);

    return deleted;
}


int CSV::deleteBy(string value,int colIndex){


    fstream file(this->filename);
    ofstream temp;
    int deleted = 0;

    string line,col, tempFileName = "temp.csv";
    vector<string> row;

    temp.open(tempFileName);
    // write headers
    getline(file,line);
    temp << line << "\n" ;

    while(getline(file,line)){
        row.clear();

        stringstream s(line);

        while(getline(s,col,',')){
            row.push_back(col);
        }

        if(row[colIndex] != value){
            temp << line << "\n" ;
        }else {
            deleted += 1;
        }


    }

    temp.close();
    file.close();

    remove(this->filename);
    rename(const_cast<char*>(tempFileName.c_str()),this->filename);

    return deleted;
}


string CSV::add(string data){
    fstream file(this->filename);
    map<string,string> ids;
    string line,id;

    
    string newId = stv(data,',')[0];

    while(getline(file,line)){
        stringstream lineStream(line);

        getline(lineStream,id,',');
        ids.insert({id, id});
    }

    //know wheter id exist in database
    if(ids.find(newId) != ids.end()){
        throw 409;
    }

    file.close();

    ofstream fileAppend(this->filename,ios::app);

    fileAppend << data << endl;

    fileAppend.close();

    return data;
}

vector<string> CSV::add(vector<string>* data){
    string dataString = "";

    for(auto & d : *data){
        if(dataString.length() > 0)
            dataString +=",";
        dataString += d;
    }

    string newData = this->add(dataString),singleCol;
    vector<string> newDataVector;
    stringstream newDataStream(newData);

    while(getline(newDataStream,singleCol,',')){
        newDataVector.push_back(singleCol);
    }

    return newDataVector;
}

// Location

string Location::addLocation(string name){
    CSV locationFile("database/locations.csv");
    try{
        return locationFile.add(name);
    }catch(int e){
        if(e == 409)
            cout << "\t" << "Location already exist" << endl;
        return "";
    }
}

void Location::listLocations(){
    fstream locationFile("database/locations.csv");

    string line;

    getline(locationFile,line);
    set<string> locations;


    while(getline(locationFile,line)){
        boost::algorithm::to_upper(line);
        locations.insert(line);
    }
    locationFile.close();


    for(auto location : locations){
        cout << "\t" << location << endl;
    }
}

bool Location::deleteLocation(string locationName){
    CSV locationFile("database/locations.csv");
    CSV diseaseFile("database/diseases.csv");

    try{
        
        int deletedDiseaseLocations = diseaseFile.deleteBy(locationName,1); 

        if(deletedDiseaseLocations > 0){
            cout << "\t" << "Deleted " << deletedDiseaseLocations << " diseases in '" << boost::algorithm::to_upper_copy(locationName) << "'" << endl;
        }else{
            cout << "\t" << "No diseases in '" << boost::algorithm::to_upper_copy(locationName) << "' to delete" << endl;
        }

        int deletedLocations =  locationFile.deleteById(locationName);

        if(deletedLocations > 0){
            cout << "\t" << "Deleted '" << boost::algorithm::to_upper_copy(locationName) << "' location" << endl;
        }else {
            cout << "\t" << "No location named '"<< boost::algorithm::to_upper_copy(locationName) <<"' to delete" << endl;
        }

        return true;
    }catch(int e){
        if(e == 404)
            cout << "\t" << "Location not found" << endl;
        return false;
    }
}


// Diseases

vector<string> Disease::addDisease(string name,string location,int cases){
    string newLIne = name + "," + location + "," + to_string(cases);
    ofstream diseaseFile("database/diseases.csv",ios::app);
    CSV locationFile("database/locations.csv");

    try{
        locationFile.selectById(location);

        diseaseFile << newLIne << endl;

        return stv(newLIne,',');
    }catch(int e){
        if(e == 404)
            cout << "\t" << "Location "<< location <<" not found" << endl;
        return vector<string>();
    }
    
}

vector<string> Disease::findDiseaseLocation(string diseaseName){
    fstream diseaseFile("database/diseases.csv");
    vector<string> foundDisease,locations;
    string line,word;
    stringstream s(line);

    while(getline(diseaseFile,line)){
        foundDisease.clear();
        stringstream s(line);

        while(getline(s,word,',')){
            foundDisease.push_back(word);
        }
        if(foundDisease[0] == diseaseName){
            locations.push_back(foundDisease[1]);
        }
    }
    diseaseFile.close();

    if(locations.size() == 0)
        throw 404;

    return locations;
}

set<string> Disease::listDiseases(){
    fstream diseaseFile("database/diseases.csv");
    set<string> diseases;
    string line,word;
    getline(diseaseFile,line);

    while(getline(diseaseFile,line)){
        stringstream s(line);
        getline(s,word,',');
        diseases.insert(word);
    }
    diseaseFile.close();
    return diseases;
}

int Disease::cases(string diseaseName,string location){
    fstream diseaseFile("database/diseases.csv");
    string line,word;
    int cases = 0;
    bool foundSomething = false;
    
    vector<string> foundDisease;

    while(getline(diseaseFile,line)){
        foundDisease.clear();
        stringstream s(line);

        while(getline(s,word,',')){
            foundDisease.push_back(word);
        }
        if(foundDisease[0] == diseaseName && foundDisease[1] == location){
            cases += stoi(foundDisease[2]);
            foundSomething = true;
        }
    }
    diseaseFile.close();

    if(!foundSomething)
        throw 404;

    return cases;
}

int Disease::cases(string diseaseName){
    fstream diseaseFile("database/diseases.csv");
    string line,word;
    int cases = 0;
    bool foundSomething = false;
    
    vector<string> foundDisease;

    while(getline(diseaseFile,line)){
        foundDisease.clear();
        stringstream s(line);

        while(getline(s,word,',')){
            foundDisease.push_back(word);
        }
        if(foundDisease[0] == diseaseName){
            cases += stoi(foundDisease[2]);
            foundSomething = true;
        }
    }
    diseaseFile.close();

    if(!foundSomething)
        throw 404;
    return cases;
}

void Disease::summary(){
    fstream diseaseFile("database/diseases.csv");
    string line,word;
    int cases = 0;
    bool foundSomething = false;
    
    vector<string> foundDisease;
    map<string,int> diseaseMap;
    map<string,int >::iterator it;

    getline(diseaseFile,line); // skip header

    while(getline(diseaseFile,line)){
        foundDisease.clear();
        stringstream s(line);

        while(getline(s,word,',')){
            foundDisease.push_back(word);
        }
        cases += stoi(foundDisease[2]);
        foundSomething = true;
        diseaseMap[foundDisease[1]] += stoi(foundDisease[2]);
    }
    diseaseFile.close();

    if(!foundSomething)
        throw 404;
    
    cout << endl;
    cout << "\t" << "Locations Summary" << endl;
    cout << "\t" << "--------------------------------------------" << endl;
    cout << "\t" << setfill(' ') << left << setw(30) << "Location" << "Total cases" << endl;
    for (it = diseaseMap.begin(); it != diseaseMap.end(); it++)
    {
        cout << "\t" << setfill(' ') << left << setw(30) << it->first << it->second;
        cout << endl;
    }
    
    cout << endl;
    cout << "\t" << "Total cases: " << cases << endl;
    cout << endl;
}

string console(){
    string command;

    cout << endl << "Console > ";
    getline(cin,command);
    boost::algorithm::to_lower(command);
    return command;
}

string currentTime(){
    time_t t = time(0);   // get time now
    tm* now = localtime(&t);
    string timeString = asctime(now);

    stringstream s(timeString);
    getline(s,timeString);

    return timeString;
}

void help(){
    cout << "\t" << "==============================================================================================================" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "*                                          HELP MENU                                                         *" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "**************************************************************************************************************" << endl;
    cout << "\t" << endl;
    cout << "\t" << setfill(' ') << left << setw(50) << "add <location> " << ":Add new location" << endl;
    cout << "\t" << setfill(' ') << left << setw(50) << "delete <location> " << ":Delete an existing location" << endl;
    cout << "\t" << setfill(' ') << left << setw(50) << "record <location> <disease> <cases> " << ":Record a disease and its cases" << endl;
    cout << "\t" << setfill(' ') << left << setw(50) << "list locations " << ":List all existing locations" << endl;
    cout << "\t" << setfill(' ') << left << setw(50) << "list diseases " << ":List all existing diseases" << endl;
    cout << "\t" << setfill(' ') << left << setw(50) << "where <disease> " << ":Find where disease exists" << endl; 
    cout << "\t" << setfill(' ') << left << setw(50) << "cases <location> <disease> " << ":Find where disease exists" << endl; 
    cout << "\t" << setfill(' ') << left << setw(50) << "cases <disease> " << ":Find total cases of a given disease" << endl; 
    cout << "\t" << setfill(' ') << left << setw(50) << "help  " << ":Print user manual" << endl; 
    cout << "\t" << setfill(' ') << left << setw(50) << "exit  " << ":Exit the program" << endl; }

void startMenu(){

    cout << "\t" << "==============================================================================================================" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "*                                     Welcome to Disease Casese Reporting System!                            *" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "**************************************************************************************************************" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "* It is developed by 'Ntwari Clarance Liberiste` as practical                                                *" << endl;
    cout << "\t" << "* Evaluation for end of Year 3                                                                               *" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "==============================================================================================================" << endl;
    cout << "\t" << "*                                                                                                            *" << endl;
    cout << "\t" << "* Starting time: " << setfill(' ') << left << setw(92) << currentTime() <<"*" << endl;
    cout << "\t" << "* need a help? type 'help' then place Enter key                                                              *" << endl;
    cout << "\t" << endl;
}



void dashboard(){
    cout << "\033[2J\033[1;1H"; // clear screen
    startMenu();
    help();

    string command,input;
    Location location;
    Disease disease;

    while(command != "exit"){

        input = console();
        command = cmd(stv(input,' '));

        if(command == "help"){
            help();
        }
        else if (command == "add"){

            string locationName = boost::algorithm::to_lower_copy(stv(input,' ')[1]);
            string newLocation = location.addLocation(locationName);
            if(newLocation != "")
                cout << "Location " <<boost::algorithm::to_upper_copy(newLocation) <<" is successfully addded" << endl;
        }
        else if (command == "list"){
            vector<string> commandVector = stv(input,' ');

            if(commandVector.size() == 2){
                 if(commandVector[1] == "locations" ){
                    location.listLocations();
                }
                else if(commandVector[1] == "diseases"){
                    set<string> listOfDisease =  disease.listDiseases();

                    for(auto disease : listOfDisease){
                        cout << "\t" << disease << endl;
                    }
                    // diseaseFile.list();
                } else {
                    cout << "\t" << "Invalid usage of 'list' command" << endl;
                    cout << "\t" << "Try out these commands" << endl;
                    cout << "\t" << "\t - list locations" << endl;
                    cout << "\t" << "\t - list diseases" << endl;
                }
            } else {
                cout << "\t" << "Invalid usage of 'list' command" << endl;
                cout << "\t" << "Try out these commands" << endl;
                cout << "\t" << "\t - list locations" << endl;
                cout << "\t" << "\t - list diseases" << endl;
            }
            
        }
        else if(command == "record"){
            vector<string> commandVector = stv(input,' ');
            if(commandVector.size() == 4){
                string locationName = boost::algorithm::to_lower_copy(commandVector[1]);
                string diseaseName = boost::algorithm::to_lower_copy(commandVector[2]);
                int cases = stoi(commandVector[3]);
                vector<string> newDisease = disease.addDisease(diseaseName,locationName,cases);
                if(newDisease.size() == 3){
                    cout << "\t" << "✅" << endl;
                }
            } else {
                cout << "\t" << "❌ Invalid usage of 'record' command" << endl;
                cout << "\t" << "Try out these commands" << endl;
                cout << "\t" << "\t - record <location> <disease> <cases>" << endl;
            }
        }
        else if(command == "where"){
            vector<string> commandVector = stv(input,' ');
            if(commandVector.size() == 2){
                string diseaseName = boost::algorithm::to_lower_copy(commandVector[1]);
                
                try{
                    vector<string> locations = disease.findDiseaseLocation(diseaseName);

                    cout << "\t" << "[";
                    for(int i = 0; i < locations.size(); i++){
                        if(i!=0)
                            cout << ",";
                        cout  << locations[i];
                    }
                    cout << "]" << endl;
                }catch(int e){
                    if(e == 404){
                        cout << "\t" << "❌ No location with this disease" << endl;
                    }
                }
            } else {
                cout << "\t" << "❌ Invalid usage of 'where' command" << endl;
                cout << "\t" << "Try out these commands" << endl;
                cout << "\t" << "\t - where <disease>" << endl;
            }
        }
        else if(command == "cases"){
            vector<string> commandVector = stv(input,' ');
            if(commandVector.size() == 3){
                string locationName = boost::algorithm::to_lower_copy(commandVector[1]);
                string diseaseName = boost::algorithm::to_lower_copy(commandVector[2]);
                try{
                    int cases = disease.cases(diseaseName,locationName);
                    boost::algorithm::to_upper(diseaseName);
                    boost::algorithm::to_upper(locationName);
                    cout << "\t" << "Cases of " << diseaseName << " at " << locationName << " are: " << cases << endl;
                }catch(int e){
                    if(e == 404){
                        cout << "\t" << "❌ Disease/Location not found" << endl;
                    }
                }
            }
            else if(commandVector.size() == 2){
                string diseaseName = boost::algorithm::to_lower_copy(commandVector[1]);
                try{
                    int cases = disease.cases(diseaseName);
                    boost::algorithm::to_upper(diseaseName);
                    cout << "\t" << "Cases of '" << diseaseName << "' = " << cases << endl;
                }catch(int e){
                    if(e == 404){
                        cout << "\t" << "❌ Disease not found" << endl;
                    }
                }
            } else {
                cout << "\t" << "❌ Invalid usage of 'cases' command" << endl;
                cout << "\t" << "Try out these commands" << endl;
                cout << "\t" << "\t - cases <location> <disease>" << endl;
                cout << "\t" << "\t - cases <disease>" << endl;
            }
        }
        else if(command == "delete"){
            vector<string> commandVector = stv(input,' ');
            if(commandVector.size() == 2){
                string locationName = boost::algorithm::to_lower_copy(commandVector[1]);

                location.deleteLocation(locationName);
            } else {
                cout << "\t" << "❌ Invalid usage of 'delete' command" << endl;
                cout << "\t" << "Try out these commands" << endl;
                cout << "\t" << "\t - delete <location> " << endl;
            }
        }
        else if(command == "exit"){
            disease.summary();
            cout << "\t" << "Ending time :" << currentTime() << endl;
            cout << "\t" << "Exiting the program" << endl;
        }
        else{
            cout << "Command '"<< command <<"' not found" << endl;
            cout << "Consider 'help' command to see all available commands" << endl;
        }
    }




}

int main(){

    dashboard();

    return  0;
}