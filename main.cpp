#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <limits>
using namespace std;

struct feature{
    int type =0, dist = numeric_limits<int>::max();
    vector<float> feature = vector<float> (64);
};
struct node{
    node *parent;
    vector<node>children;
    vector<int> feat;
    int depth = 0;
};
//Function Prototypes
void getInput(vector<feature> &features);
void printFeatures(const vector<feature> &features);
void normalize(vector<feature> &features);
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types);
float distance(vector<float> lhs, vector<float> rhs, vector<int> types);
vector< vector<int> > makeTree(const vector<feature> &features);
feature forwardSelection(const vector<feature> &features);

int main(){
    int menuInput = 2;
    vector<feature> features (2048);

    cout << "Welcome to Carlos Santillana's Feature Selection Algorithm\n";
    getInput(features);
    cout << "Type the number of the algorithim you want to run\n";
    cout << "1. Forward Selection\n";
    cout << "2. Backward Elimination\n";
    cout << "3. Carlos's Special Algorithm\n";
    cin >> menuInput;
    while (menuInput < 1 || menuInput > 3){
        cin >> menuInput;
        if (menuInput <1  || menuInput > 3){
            cout << "Invalid input\n";
            cout << "Type the number of the algorithim you want to run\n";
            cout << "1. Forward Selection\n";
            cout << "2. Backward Elimination\n";
            cout << "3. Carlos's Special Algorithm\n";
        }
    }
    normalize(features);

    cout << "Please wait while I normalize the data... Done!\n";
    if (menuInput == 1){
        forwardSelection(features);
    }
    else if (menuInput == 2){

    }
    else if (menuInput == 3){

    }
    return 0;
}

void getInput(vector<feature> &features){
    string fname;
    ifstream inFile;
    float val;
    string line;
    bool first = true;
    int count =0;
    int rows= 0;

    cout << "Type the name of file to test: ";
    cin >> fname;
    //inFile.open(fname.c_str()); unblock later
    inFile.open("CS170Smalltestdata__83.txt");
    if(!inFile){
        cout << "Error opening file\n";
        exit(1);
    }
    //Taken then adapted from stack overlow from user Kenrogers link in coverpage of report
    while (getline(inFile, line)){
        int count2=0;
        first =true;
        istringstream iss(line);
        while(iss){
            if (first){
                iss >> val;
                features.at(count).type = val;
                first = false;
            }
            iss >> val;
            features.at(count).feature.at(count2) = val;
            count2++;
        }
        features.at(count).feature.resize(count2-1);
        count++;
        count2=0;
        rows++;
    }
    features.resize(rows);
    //End
}
void printFeatures(const vector<feature> &features){
    for(int i=0; i < features.size(); i++){
        cout << features.at(i).type << ": ";
        for(int j=0; j < features.at(i).feature.size(); j++){
                cout << setprecision(8) << features.at(i).feature.at(j) << ", ";
        }
        cout << endl;
    }
}
void normalize(vector<feature> &features){
    float mean= 0, stdev=0, count=0, middle =0;
    for(int i=0; i < features.size(); i++){// finds mean
        for(int j=0; j < features.at(i).feature.size(); j++){
                mean+= features.at(i).feature.at(j);
                count++;
        }
    }
    mean = mean/count;
    cout << "This data set has " << features.at(1).feature.size() << " features";
    cout << "(not including the class attribute), with " << features.size() << " instances.\n";
    cout << "mean: " << mean << endl;
    cout << "size: " << count << endl;
    for(int i=0; i < features.size(); i++){// finds std
        for(int j=0; j < features.at(i).feature.size(); j++){
                middle+= pow((features.at(i).feature.at(j)- mean),2);
        }
    }
    stdev = middle/count;
    stdev = sqrt(stdev);
    cout << "standard deviation:" << stdev << endl;
    for(int i=0; i < features.size(); i++){// normalizes
        for(int j=0; j < features.at(i).feature.size(); j++){
                features.at(i).feature.at(j)= (features.at(i).feature.at(j) - mean)/stdev;
        }
    }
}
//
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types){
    feature nearest;
    float dist = numeric_limits<float>::max(), currentDist =0;

    for(int i =0; i < features.size(); i++){
        currentDist = distance(features.at(i).feature, start.feature, types);
        if ( currentDist < dist){
            dist = currentDist;
            nearest = features.at(i);
        }
    }
    return nearest;
}

float distance(vector<float> lhs, vector<float> rhs, vector<int> types){
    float dist =0;
    for(int i=0; i < types.size(); i++){
        dist+= abs(lhs.at(types.at(i)) - rhs.at(types.at(i)));
    }
    return dist;
}

//adapted from cplusplus.com
vector<vector<int> > makeTree(const vector<feature> &features){
    int size = features.at(0).feature.size();
    vector<double> nodes(size);
    vector<vector<int> > featureComb;
    for (double i =0; i < size ; i++){
        nodes.at(i) = i;
    }
    for( int length = 1; length <= size; length++ ) {
        vector<int> i(length);
        for( int j = 0; j < length; j++ ){
            i.at(j) = length - 1 - j;
        }
        while( i.at(0) < size ) {
            vector<int> comb;
            for( int j = 0; j < length; j++ ){
                comb.push_back( nodes[i[j]] );
            }
            featureComb.push_back(comb);
            for( int j = 0; j< length; j++ ) {
                i[j]++;
                if( i[j] < size )
                    break;
                if( j < length - 1 )
                    i[j] = i.at(j + 1) + 2;
            }
        }
    }

    return featureComb;
}

feature forwardSelection(const vector<feature> &features){
    feature best;
    vector<vector <int> > searchTree = makeTree(features);

    return best;
}
