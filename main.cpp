#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <limits>
#include <algorithm>//To erase element from vector by value
#include <queue>
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
    node()
        : correct {0}, accuracy {0} {}
    node(vector<int> feats, double corr, double acc)
        : feature { feats }, correct { corr }, accuracy { acc } {}
    bool operator<(const node& rhs) const//Used for priority_queue compares accuracy of nodes
 {
     return accuracy < rhs.accuracy;
 }
};

//Function Prototypes
void getInput(vector<feature> &features);
void normalize(vector<feature> &features);
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types, int miss);
float distance(vector<float> lhs, vector<float> rhs, vector<int> types);
node forwardSelection(const vector<feature> &features);
node backwardSelection(const vector<feature> &features);
node carlosSelection(const vector<feature> &features,int place);
double leaveOneOut(const vector<feature> &features, vector<int>  types, int k);
bool contains(vector<int> thisLevel, int feature);
double leaveOneOut2(const vector<feature> &features, vector<int>  types, int j);

int main(){
    int menuInput = 2;
    vector<feature> features (2048);
    node best, tempBest;
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
     correct = leaveOneOut(features, all2, features.at(0).feature.size() );
    cout << "Running nearest neighbor with all 4 features, using “leaving-one-out” evaluation, I get an accuracy of ";
    cout << setprecision(2) << fixed << correct* 100 << "% \n";
    cout << "Beginning search\n";
    if (menuInput == 1){
        best = forwardSelection(features);
    }
    else if (menuInput == 2){
        best = backwardSelection(features);
    }
    else if (menuInput == 3){
    //Idea!
    //Depth limited monte carlo (fork) search
    //Depth is limited to four (reference graph dicusssed in class, maybe allow    for user input)
    //number of "snakes" is the square root of number of features
        for(int l= 0; l < sqrt(features.at(0).feature.size()); l++){
            tempBest = carlosSelection(features, l);
            if ( tempBest.accuracy > best.accuracy ){
                best = tempBest;
            }
        }
    }
    cout << "The best feature is ";
    for (int i=0; i < best.feature.size(); i++){
        cout << best.feature.at(i) << " ";
    }
    cout << " with an accuracy of " <<best.accuracy* 100  << "%" << endl;
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
    inFile.open(fname.c_str());
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

//Normalizes input
void normalize(vector<feature> &features){
    float mean= 0, stdev=0, count=0, middle =0;
    cout << "This data set has " << features.at(1).feature.size() << " features";
    cout << "(not including the class attribute), with " << features.size() << " instances.\n";
    for(int i=0; i < features.at(0).feature.size(); i++){
        for(int j=0; j < features.size(); j++){// finds mean
                mean+= features.at(j).feature.at(i);
                count++;
        }
        mean = mean/count;

        for(int j=0; j < features.size(); j++){// finds std
                middle+= pow((features.at(j).feature.at(i)- mean),2);
        }
        stdev = middle/count;
        stdev = sqrt(stdev);
        for(int j=0; j < features.size(); j++){// normalizes
                features.at(j).feature.at(i)= (features.at(j).feature.at(i) - mean)/stdev;
        }
    }
}

//Runs forward selection
node forwardSelection(const vector<feature> &features){
    vector<int> setOfFeatures;
    vector<int> thisLevel (1);
    bool localMax = false;
    double bestSoFar = 0, accuracy =0;
    node bestest;
    for (int i =1; i <= features.at(0).feature.size(); i++){
        bestSoFar = 0;
        for (int k =1; k <= features.at(0).feature.size(); k++){
            accuracy =0;
            if (!contains(setOfFeatures, k)){
                accuracy = leaveOneOut(features, setOfFeatures, k);
                cout << "   Using feature(s) { ";
                for (int m =0; m < setOfFeatures.size(); m++){
                    cout << setOfFeatures.at(m) << ", ";
                }
                cout << k << " } accuracy is " << accuracy * 100 << "%" <<endl;
                if (accuracy > bestSoFar){
                    bestSoFar = accuracy;
                    thisLevel.at(thisLevel.size()-1) = k;
                    if (bestSoFar > bestest.accuracy){
                         if (i > 1)//So it doesn't crash when setOfFeatures is empty
                            bestest.feature = thisLevel;
                        else
                            bestest.feature.push_back(k);
                        bestest.accuracy = bestSoFar;
                    }
                }
            }
        }
        if (bestSoFar < bestest.accuracy && !localMax){
            localMax = true;
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n";
        }
        setOfFeatures =  (thisLevel);
        cout << "\nFeature set { ";
        for (int m =0; m < setOfFeatures.size()-1; m++){
            cout << setOfFeatures.at(m) << ", ";
        }
        cout << " " <<  setOfFeatures.at(setOfFeatures.size()-1) << " } was best, accuracy is ";
        cout  << bestSoFar * 100 << "%" << endl << endl;
        thisLevel.resize(thisLevel.size()+1);
    }

    return bestest;
}
//Runs backwardSelection
node backwardSelection(const vector<feature> &features){
    vector<int> setOfFeatures;
    vector<int> thisLevel;
    double bestSoFar = 0;
    double accuracy =0;
    bool localMax = false;
    for(int i=1; i <= features.at(0).feature.size(); i++){
        setOfFeatures.push_back(i);
    }
    node bestest;
    for (int i =1; i < features.at(0).feature.size(); i++){
        bestSoFar = 0;
        for (int k =1; k <= features.at(0).feature.size(); k++){
            accuracy =0;
            if (contains(setOfFeatures, k)){
                accuracy = leaveOneOut2(features, setOfFeatures, k);

                cout << "   Using feature(s) { ";
                for (int m =0; m < setOfFeatures.size(); m++){
                    if (setOfFeatures.at(m) != k)
                        cout << setOfFeatures.at(m) << ", ";
                }
                cout <<  "} accuracy is " << accuracy * 100 << "%" <<endl;
                if (accuracy >= bestSoFar){
                    bestSoFar = accuracy;
                    thisLevel = setOfFeatures;
                    thisLevel.erase(std::remove(thisLevel.begin(), thisLevel.end(), k), thisLevel.end());// erases k from vect
                    if (bestSoFar > bestest.accuracy){
                        bestest.feature = thisLevel;
                        bestest.accuracy = bestSoFar;
                    }
                }
            }
        }

        if (bestSoFar < bestest.accuracy && !localMax){
            localMax = true;
            cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n";
        }
        setOfFeatures =  (thisLevel);
        cout << "\nFeature set { ";
        for (int m =0; m < setOfFeatures.size()-1; m++){
            cout << setOfFeatures.at(m) << ", ";
        }
        cout << " " <<  setOfFeatures.at(setOfFeatures.size()-1) << " } was best, accuracy is ";
        cout  << bestSoFar * 100 << "%" << endl << endl;
        thisLevel.resize(thisLevel.size()-1);
    }

    return bestest;
}
bool contains(vector<int> thisLevel, int feature){
    bool cont = false;
    for(int i =0; i < thisLevel.size(); i++){
            if (feature == thisLevel.at(i)){
                cont = true;
                return cont;
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
double leaveOneOut(const vector<feature> &features, vector<int>  types, int j){
    double percentCorrect =0;
    double correct = 0;
    types.push_back(j);
    feature guess;
        for(int i=0; i < features.size(); i++){//I represents the value to leave out
            guess = nearestNeighbor(features, features.at(i), types, i);
            if (guess.type == features.at(i).type){
                correct++;
            }
        }
    percentCorrect = correct/(features.size());
    return percentCorrect;
}
//Runs leave one out evalution for backwardSelection
double leaveOneOut2(const vector<feature> &features, vector<int>  types, int j){
    double percentCorrect =0;
    double correct = 0;
    feature guess;
    types.erase(std::remove(types.begin(), types.end(), j), types.end());// erases k from vect
    for (int k =0; k < types.size(); k++){//checks previous values
        for(int i=0; i < features.size(); i++){//I represents the value to leave out
            guess = nearestNeighbor(features, features.at(i), types, i);
            if (guess.type == features.at(i).type){
                correct++;
            }
        }
    }
    percentCorrect = correct/(features.size()*(types.size()));
    return percentCorrect;
}
//Runs a depth limited search by the sqrt of the number of features but guarantees
// to run to a depth of at least four if there are four or more features.
node carlosSelection(const vector<feature> &features, int place){
    vector<int> setOfFeatures;
    vector<int> thisLevel (1);
    int depth = sqrt(features.at(0).feature.size());
    double bestSoFar = 0, accuracy =0;
    priority_queue<node> accuracyOrder;
    node bestest;
    bool localMax = false;
    if (depth < 4 && 4 < features.at(0).feature.size())
        depth = 4;
    for (int i =1; i <= depth; i++){
        bestSoFar = 0;
        for (int k =1; k <= features.at(0).feature.size(); k++){
            accuracy =0;
            if (!contains(setOfFeatures, k)){
                accuracy = leaveOneOut(features, setOfFeatures, k);
                if (i == 1){//When at level one  place accuracy in priority_queue
                    vector<int> stuff {k};
                    node temp(stuff, 0, accuracy);
                    accuracyOrder.push(temp);
                }
                cout << "   Using feature(s) { ";
                for (int m =0; m < setOfFeatures.size(); m++){
                    cout << setOfFeatures.at(m) << ", ";
                }
                cout << k << " } accuracy is " << accuracy * 100 << "%" <<endl;
                if (accuracy > bestSoFar){
                    bestSoFar = accuracy;
                    thisLevel.at(thisLevel.size()-1) = k;
                    if (bestSoFar > bestest.accuracy){
                         if (i > 1)//So it doesn't crash when setOfFeatures is empty
                            bestest.feature = thisLevel;
                        else
                            bestest.feature.push_back(k);
                        bestest.accuracy = bestSoFar;
                    }
                }
            }
        }
        if (i == 1){//picks the "place" best feature
            for(int k = 0; k < place; k++){
                accuracyOrder.pop();
            }
            thisLevel = accuracyOrder.top().feature;
            bestSoFar = accuracyOrder.top().accuracy;
            setOfFeatures =  (thisLevel);
        }
        if (bestSoFar < bestest.accuracy && !localMax){
            localMax = true;
            cout << "(Warning, Accuracy has decreased! Continuing search in case of other maxima)\n";
        }
        setOfFeatures =  (thisLevel);

        cout << "\nFeature set { ";
        for (int m =0; m < setOfFeatures.size()-1; m++){
            cout << setOfFeatures.at(m) << ", ";
        }
        cout << " " <<  setOfFeatures.at(setOfFeatures.size()-1) << " } was best, accuracy is ";
        cout  << bestSoFar * 100 << "%" << endl << endl;
        thisLevel.resize(thisLevel.size()+1);
    }

    return bestest;
}
