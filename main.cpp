#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <iterator>
#include <cctype>
#include <algorithm>
#include <boost/algorithm/string.hpp>

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

class CSV{
    private:
        const char *filename;

    public:
        CSV(const char *filename);
        map<string, vector<string>> selectAll();
        vector<string> selectById(int id);
        int updateById(int id,string data);
        int updateById(int id,vector<string>* data);
        int deleteById(int id);
        string add(string data);
        vector<string> add(vector<string>* data);
};

class Location {
    public:
        void Locaton();
        string addLocation(string name);
        bool deleteLocation();
        bool listLocations();
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



vector<string> CSV::selectById(int id){
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
        foundId = stoi(foundRow[0]);

        if( foundId == id)
            return foundRow;
    }

    file.close();
    throw 404;
}


int CSV::updateById(int id, string data){
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

        if(id == stoi(identity)){
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


int CSV::updateById(int id, vector<string> *data){
    string dataString = "";

    for(auto & d : *data){
        if(dataString.length() > 0)
            dataString +=",";
        dataString +=d;
    }

    return this->updateById(id,dataString);
}


int CSV::deleteById(int id){
    // NOTE: i know this function has stupid logic that could never be done in large enterprise application
    // rewriting whole file 😒, i understands how overwhelming it is  and how poor perfomant it is
    // but you need to understand that i was rushing and this was fast for me, or if you have time open PR
    // dealing with file pointers is kinda pain


    fstream file(this->filename);
    ofstream temp;
    int deleted = 0;

    string line,identity, tempFileName = "temp.csv";

    temp.open(tempFileName);

    while(getline(file,line)){

        stringstream s(line);

        getline(s,identity,',');

        if(id != stoi(identity)){
            temp << line << "\n" ;
            deleted = 1;
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
            cout << "Location already exist" << endl;
        return "";
    }
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



string console(){
    string command;

    cout << endl << "Console > ";
    getline(cin,command);
    return command;
}

void help(){
    cout << "\t" << "==============================================================" << endl;
    cout << "\t" << "*          HELP MENU                                          *" << endl;
    cout << "\t" << "***************************************************************" << endl;
    cout << "\t" << endl;
    cout << "\t" << "add <location> \t\t\t\t:Add new location" << endl;
    cout << "\t" << "delete <location> \t\t\t\t:Delete an existing location" << endl;
    cout << "\t" << "record <location> <disease> <cases> \t\t\t\t:Record a disease and its cases" << endl;
    cout << "\t" << "list locations \t\t\t\t:List all existing locations" << endl;
    cout << "\t" << "list diseases \t\t\t\t:List all existing diseases" << endl;
    cout << "\t" << "where <disease> \t\t\t\t:Find where disease exists" << endl; 
    cout << "\t" << "cases <location> <disease> \t\t\t\t:Find where disease exists" << endl; 
    cout << "\t" << "cases <disease> \t\t\t\t:Find total cases of a given disease" << endl; 
    cout << "\t" << "help  \t\t\t\t:Print user manual" << endl; 
    cout << "\t" << "exit  \t\t\t\t:Exit the program" << endl; }

void startMenu(){

    cout << "\t" << "==============================================================" << endl;
    cout << "\t" << "*          Welcome to Disease Casese Reporting System!        *" << endl;
    cout << "\t" << "***************************************************************" << endl;
    cout << "\t" << "*                                                             *" << endl;
    cout << "\t" << "*                                                             *" << endl;
    cout << "\t" << "* It is developed by 'Ntwari Clarance Liberiste` as practical *" << endl;
    cout << "\t" << "* Evaluation for end of Year 3                                *" << endl;
    cout << "\t" << "*                                                             *" << endl;
    cout << "\t" << "==============================================================" << endl;
    cout << "\t" << "*                                                             *" << endl;
    // TODO display starting time
    cout << "\t" << "* Starting time: ......                                       *" << endl;
    cout << "\t" << "* need a help? type 'help' then place Enter key               *" << endl;
    cout << "\t" << endl;
}



void dashboard(){

    startMenu();
    help();

    string command,input;
    Location location;

    // c++ switch case

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

        else{
            cout << "Command not found" << endl;
        }
    }




}

int main(){

    // CSV csv("data.csv");

    
    // 1. TEST SELECT ALL FUNCTIONALITY
    // map<string, vector<string>> list = csv.selectAll();
    // displayMap(&list);

    //2.  TEST GET FUNCTIONALITY
    // try{
    //     displayVector(csv.selectById(20));
    // }catch(int e){
    //     if(e == 404)
    //     cout << "\nNot found\n" ; 
    // }


    //3. TEST UPDATE FUNCTIONALITY
    // displayVector(csv.selectById(1));
    
    // vector<string> data= {"1","umwali","11"};

    // cout << "\n\n" << csv.updateById(1,data) << "\n\n";
    // // cout << "\n\n" << csv.updateById(1,"1,ntwari,900") << "\n\n"; // or using just string string;

    // displayVector(csv.selectById(1));

    //4. TEST DELETE
    // csv.deleteById(2);

    //5. ADD DATA TO FILE
    // cout << csv.add("tracy,303");
    // vector<string> data = {"tracy","430"};
    // displayVector(csv.add(&data));



    string command;

    dashboard();

}