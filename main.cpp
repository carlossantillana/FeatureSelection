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
    node *sibling, *child;
    vector<int> feature = vector<int> (64);
    int height = 0;
    node(node *ch, node *sib, int ht)
        : sibling{ sib }, child{ ch }, height{ ht } {}
    node()
        : sibling{ NULL }, child{ NULL }, height{ 0 } {}
    node(node *ch)
        : sibling{ NULL }, child{ NULL }, height{ ch->height +1 }, feature{ ch->feature } {}
};
//Function Prototypes
void getInput(vector<feature> &features);
void printFeatures(const vector<feature> &features);
void normalize(vector<feature> &features);
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types);
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types, int miss);
float distance(vector<float> lhs, vector<float> rhs, vector<int> types);
vector< vector<int> > makeTree(const vector<feature> &features);
node makeTree2(const vector<feature> &features);
feature forwardSelection(const vector<feature> &features);
double leaveOneOut(const vector<feature> &features, vector<int> types);
void treeHelper(const vector<feature> &features, node &parent);
void printTree(node origin);

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
    vector<int> all (features.at(0).feature.size());
    double correct =0;
    for(int i=0; i < features.at(0).feature.size(); i++){
        all.push_back(i);
    }
    correct = leaveOneOut(features, all);
    cout << "Running nearest neighbor with all 4 features, using “leaving-one-out” evaluation, I get an accuracy of ";
    cout << setprecision(2) << fixed << correct* 100 << "% \n";
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
//General case
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types){
    feature nearest;
    float dist = numeric_limits<float>::max(), currentDist =0;

    for(int i =0; i < features.size(); i++){
        currentDist = distance(features.at(i).feature, start.feature,types);
        if ( currentDist < dist){
            dist = currentDist;
            nearest = features.at(i);
        }
    }
    return nearest;
}
//nearest neighbor for leaving one out evalutation
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
        dist+= abs(lhs.at(types.at(i)) - rhs.at(types.at(i)));
    }
    return dist;
}

//adapted from cplusplus.com creates a vector of all possible combinations without replacement
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

node makeTree2(const vector<feature> &features){
    node origin = new node();
    treeHelper(features, origin);
    printTree(origin);
    return origin;
}
void treeHelper(const vector<feature> &features, node &parent){
    node * child = new node(&parent);
    parent.child = child;
    cout << "In tree helper\n";
    cout << "Parent height " << parent.height << endl;
    cout << "child height " << child->height << endl;
    cout << "Parent feature size " << parent.feature.size() << endl;
    if (parent.height >= features.at(0).feature.size()){
        exit(0);
    }
    for(int i=0; i < features.at(0).feature.size() ; i++){
        child->feature.push_back(i);
    }
        treeHelper(features, *child);
}
void printTree(node origin){
    cout << "new node at height " << origin.height << endl;
    cout << "new node at child " << origin.child << endl;
    cout << "new node at sibling " << origin.sibling << endl;
    for(int i =0; i < origin.feature.size(); i++){
        cout << origin.feature.at(i) << " ";
    }
    if (origin.child != NULL){
        printTree(origin.child);
    }

    cout << "end of node\n";
}
//Runs forward selection
feature forwardSelection(const vector<feature> &features){
    feature best;
    node searchTree = makeTree2(features);
    cout << "end of make tree\n";
    //for(int i=0; i < searchTree.; i++){
        //Darn current solution does not work...
        //need to know parent child dependency.
    //}
    return best;
}
//Runs leave one out evalution
double leaveOneOut(const vector<feature> &features, vector<int> types){
    double percentCorrect =0;
    double correct = 0;
    feature guess;
    vector<feature> featMinus = features;//copies feature list
    for(int i=0; i < features.size(); i++){//I represents the value to leave out
        guess = nearestNeighbor(features, features.at(i), types, i);
        if (guess.type == features.at(i).type){
            correct++;
        }
    }
    percentCorrect = correct/features.size();
    return percentCorrect;
}
