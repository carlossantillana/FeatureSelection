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
    double accuracy = 0;
};
struct node{
    vector<int> feature;
    double correct =0;
    double accuracy = 0;
};
//Function Prototypes
void getInput(vector<feature> &features);
void printFeatures(const vector<feature> &features);
void normalize(vector<feature> &features);
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types, int miss);
float distance(vector<float> lhs, vector<float> rhs, vector<int> types);
node forwardSelection(const vector<feature> &features);
node backwardSelection(const vector<feature> &features);
double leaveOneOut(const vector<feature> &features, vector<vector <int>>  types, int k);
bool contains(vector<vector<int>> thisLevel, int feature);

int main(){
    int menuInput = 2;
    vector<feature> features (2048);
    node best;
        double correct =0;

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
    vector<int> all2 (features.at(0).feature.size()-1);
    for(int i=1; i < features.at(0).feature.size(); i++){//fills vector with all features;
        all2.at(i-1) = i;
    }
    vector<vector<int>> all;
    all.push_back(all2);
     correct = leaveOneOut(features, all, features.at(0).feature.size() );
    cout << "Running nearest neighbor with all 4 features, using “leaving-one-out” evaluation, I get an accuracy of ";
    cout << setprecision(2) << fixed << correct* 100 << "% \n";
    if (menuInput == 1){
        best = forwardSelection(features);
    }
    else if (menuInput == 2){
        best = backwardSelection(features);
    }
    else if (menuInput == 3){
    //Idea!
    //Depth limited monte carlo (fork) search
    //Depth is limited to four (reference graph dicusssed in class, maybe allow     for user input)
    //number of "snakes" is the square root of number of features
    }
    cout << "The best feature is ";
    for (int i=0; i < best.feature.size(); i++){
        cout << best.feature.at(i) << " ";
    }
    cout << " with an accuracy of "<< setprecision(7) <<best.accuracy << endl;
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

            if (first){//Enters class into feature list
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
//Normalizes input
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
    for(int i=0; i < features.size(); i++){// finds std
        for(int j=0; j < features.at(i).feature.size(); j++){
                middle+= pow((features.at(i).feature.at(j)- mean),2);
        }
    }
    stdev = middle/count;
    stdev = sqrt(stdev);
    for(int i=0; i < features.size(); i++){// normalizes
        for(int j=0; j < features.at(i).feature.size(); j++){
                features.at(i).feature.at(j)= (features.at(i).feature.at(j) - mean)/stdev;
        }
    }
}

//Runs forward selection
node forwardSelection(const vector<feature> &features){
    vector<vector<int>> setOfFeatures ;
    vector<int> thisLevel (1);
    double bestSoFar = 0;
    double accuracy =0;
    node bestest;
    for (int i =1; i <= features.at(0).feature.size(); i++){
        cout << "On the " << i  << "th level of the search tree\n";
        bestSoFar = 0;
        for (int k =1; k <= features.at(0).feature.size(); k++){
            accuracy =0;
            if (!contains(setOfFeatures, k)){
                cout << "Considering adding the " << k  << "th feature";
                accuracy = leaveOneOut(features, setOfFeatures, k);
                cout << " with accuracy " << accuracy << endl;
                if (accuracy > bestSoFar){
                    bestSoFar = accuracy;
                    thisLevel.at(thisLevel.size()-1) = k;
                    if (bestSoFar > bestest.accuracy){
                        if (i > 1)
                            bestest.feature = setOfFeatures.at(i);
                         bestest.feature.push_back(k);
                         bestest.accuracy = bestSoFar;
                    }
                }
            }
        }

        setOfFeatures.push_back (thisLevel);
        cout << "On level, " << i << " I added feature ";
        for (int m = 0; m < thisLevel.size(); m++ ){
            cout << thisLevel.at(m) << " ";
        }
        cout << "to current set\n";
        thisLevel.resize(thisLevel.size()+1);
    }

    return bestest;
}
//Runs backwardSelection
node backwardSelection(const vector<feature> &features){
    vector<vector<int>> setOfFeatures ;
    vector<int> thisLevel (1);
    double bestSoFar = 0;
    double accuracy =0;
    node bestest;
    for (int i =1; i <= features.at(0).feature.size(); i++){
        cout << "On the " << i  << "th level of the search tree\n";
        bestSoFar = 0;
        for (int k =1; k <= features.at(0).feature.size(); k++){
            accuracy =0;
            if (!contains(setOfFeatures, k)){
                cout << "Considering adding the " << k  << "th feature";
                accuracy = leaveOneOut(features, setOfFeatures, k);
                cout << " with accuracy " << accuracy << endl;
                if (accuracy > bestSoFar){
                    bestSoFar = accuracy;
                    thisLevel.at(thisLevel.size()-1) = k;
                    if (bestSoFar > bestest.accuracy){
                        if (i > 1)
                            bestest.feature = setOfFeatures.at(i);
                         bestest.feature.push_back(k);
                         bestest.accuracy = bestSoFar;
                    }
                }
            }
        }

        setOfFeatures.push_back (thisLevel);
        cout << "On level, " << i << " I added feature ";
        for (int m = 0; m < thisLevel.size(); m++ ){
            cout << thisLevel.at(m) << " ";
        }
        cout << "to current set\n";
        thisLevel.resize(thisLevel.size()+1);
    }

    return bestest;
}
bool contains(vector<vector<int>> thisLevel, int feature){
    bool cont = false;
    for(int i =0; i < thisLevel.size(); i++){
        for(int j =0; j < thisLevel.at(i).size(); j++){
            if (feature == thisLevel.at(i).at(j)){
                cont = true;
                return cont;
            }
        }
    }
    return cont;
}

feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types, int miss){
    feature nearest;
    float dist = numeric_limits<float>::max(), currentDist =0;
    for(int i =0; i < features.size(); i++){
        if (i != miss){//does not compare left out feature with itself
            currentDist = distance(features.at(i).feature, start.feature,types);
            if ( currentDist < dist){
                dist = currentDist;
                nearest = features.at(i);
            }
        }
    }
    return nearest;
}
//Finds distance between two features.
float distance(vector<float> lhs, vector<float> rhs, vector<int> types){
    float dist =0;
    for(int i=0; i < types.size(); i++){//finds distance of only features passed in
        dist+= abs(lhs.at(types.at(i)-1) - rhs.at(types.at(i)-1));
    }
    return dist;
}

//Runs leave one out evalution
double leaveOneOut(const vector<feature> &features, vector<vector <int>>  types, int j){
    double percentCorrect =0;
    double correct = 0;
    vector<int> passedIn;
    passedIn.push_back(j);
    feature guess;
    cout << "\n Types size: " << types.size() << endl;
    if (!types.empty()){
        for(int c= 0; c < types.at(types.size()-1).size(); c++){
            cout << "feature set: " << types.at(types.size()-1).at(c) << ", ";
        }
    }
    cout << "considering " << j << endl;
    for (int k =0; k < types.size(); k++){//checks previous values
        for(int i=0; i < features.size(); i++){//I represents the value to leave out
            guess = nearestNeighbor(features, features.at(i), types.at(k), i);
            if (guess.type == features.at(i).type){
                correct++;
            }
        }
    }
    for(int i=0; i < features.size(); i++){//checks new value
        guess = nearestNeighbor(features, features.at(i), passedIn, i);
        if (guess.type == features.at(i).type){
            correct++;
        }
    }
    percentCorrect = correct/(features.size()*(types.size()+1));
    return percentCorrect;
}
