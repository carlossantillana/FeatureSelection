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
    vector<node*> children;
    vector<int> feature;
    double correct =0;
    int height = 0;
    node(node *ch, node *sib, int ht)
        : sibling{ sib }, child{ ch }, height{ ht } {}
    node()
        : sibling{ NULL }, child{ NULL }, height{ 0 } {}
    node(node *ch)
        : sibling{ NULL }, child{ NULL }, height{ ch->height +1 }, feature{ ch->feature} {}
};
//Function Prototypes
void getInput(vector<feature> &features);
void printFeatures(const vector<feature> &features);
void normalize(vector<feature> &features);
feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types, int miss);
float distance(vector<float> lhs, vector<float> rhs, vector<int> types);
node makeTree(const vector<feature> &features);
node forwardSelection(const vector<feature> &features, node best, node searchTree);
double leaveOneOut(const vector<feature> &features, vector<int> types);
void treeHelper(const vector<feature> &features, node &parent, int type);
void printTree(node origin);

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
    vector<int> all (features.at(0).feature.size());
    cout << "starting tree\n";
    node * searchTree =  new node(makeTree(features));
    cout << "finished Tree\n";
    for(int i=1; i <= features.at(0).feature.size(); i++){//fills vector with all features;
        all.at(i-1) = i;
    }
    correct = leaveOneOut(features, all);
    cout << "Running nearest neighbor with all 4 features, using “leaving-one-out” evaluation, I get an accuracy of ";
    cout << setprecision(2) << fixed << correct* 100 << "% \n";
    if (menuInput == 1){
        best = forwardSelection(features, best, *searchTree);
    }
    else if (menuInput == 2){

    }
    else if (menuInput == 3){
    //Idea!
    //Depth limited monte carlo (fork) search
    //Depth is limited to four (reference graph dicusssed in class, maybe allow     for user input)
    //number of "snakes" is the square root of number of features
    }
    cout << "The best feature is "  << best.feature.at(0) << " with an accuracy of "<< best.correct << endl;
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
//Nearest neighbor
// feature nearestNeighbor(const vector<feature> &features, feature start, vector<int> types){
//     feature nearest;
//     float dist = numeric_limits<float>::max(), currentDist =0;
//     for(int i =0; i < (features.at(0).feature.size()); i++){
//         currentDist = distance(features.at(i).feature, start.feature,types);
//         if ( currentDist < dist){
//             dist = currentDist;
//             nearest = features.at(i);
//         }
//     }
//     return nearest;
// }
//Runs forward selection
node forwardSelection(const vector<feature> &features, node best, node searchTree){
    node current;// local best node
    double max = best.correct;
    if (searchTree.height >= features.at(0).feature.size()){
        return best;
    }
    for(int i=0; i < searchTree.children.size(); i++){//find the best feature then call function on it
        searchTree.children.at(i)->correct = leaveOneOut(features, searchTree.children.at(i)->feature);
        if (searchTree.children.at(i)->correct > current.correct){//Checks for best local solution
            current = *searchTree.children.at(i);
            if (current.correct > max){//Checks for best solution since root
                max = current.correct;
                best = *searchTree.children.at(i);
            }
        }
    }
    best = forwardSelection(features, best, current);
    return best;
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

node makeTree(const vector<feature> &features){
    node *origin = new node();
    for (int i=1; i <= features.at(0).feature.size(); i++)
        treeHelper(features, *origin, i);
    return *origin;
}
void treeHelper(const vector<feature> &features, node &parent, int type){
    int size =0;
    int sizecheck = type;
    node * child = new node(&parent);
    parent.children.push_back(child);
    bool repeat = false;
    if (parent.height >= features.at(0).feature.size()){// quits if at end of tree
        return;
    }
    //adds input to child
    child->feature.push_back(type);
    for (int j= 1/*child->feature*/; j <= features.at(0).feature.size(); j++){// compares from 1 to num features to input
        repeat = false;
        for (int i=0; i < child->height; i++){
            if (j == child->feature.at(i)){
                repeat = true;
            }
        }
        if (!repeat){//recursively calls itself
            treeHelper(features, *child, j);//  adds new value
        }
    }
}

void printTree(node origin){

    cout << "\nnew node at height " << origin.height << endl;
    if (origin.feature.size() > 0)
        cout << "node value is " << origin.feature.at(0) << endl;
    // cout << "new node at child " << origin.child << endl;
    cout << "children are " << endl;
    for(int i =0; i < origin.children.size(); i++) {
        cout << "child " << i << " " << origin.children.at(i) << endl;
    }
    // cout << "new node at sibling " << origin.sibling << endl;
    for(int i =0; i < origin.feature.size(); i++){// prints feature
        cout << origin.feature.at(i) << ", ";
    }
    for(int i =0; i < origin.children.size(); i++){
        printTree(*origin.children.at(i));
    }
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
