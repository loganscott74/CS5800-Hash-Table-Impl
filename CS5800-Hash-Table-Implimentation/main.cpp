//
//  main.cpp
//  CS5800-Hash-Table-Implimentation
//
//  Created by Logan Gill on 11/9/24.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <string>
#include <cmath>
#include <fstream>
#include <numeric>

using namespace std;

class Node {
public:
    string key;
    int value;
    Node* right;
    Node* left;
    
    Node(string k, int val): key(k), value(val)
    {}
};

class HashTable {
private:
    static const int SOMEPRIME = 31;
    const int MAXHASH;
    vector<Node*> hTable;
    
    /// Function used to get the hash value of the given key
    int hashFunc(const string &key) const {
        int hash = 0;
        for (int i = 0; i < key.size(); ++i) {
            int c = key[i];
            // Multiply existing hash by prime then add char?
            hash = ((hash * static_cast<int>(SOMEPRIME)) + c) % MAXHASH; // Prime 71: 48.1437, 5.60843, 1.70048 | Prime 31: 40.4195, 5.81577, 1.80258
            //hash += (c * static_cast<int>(SOMEPRIME)) % MAXHASH; // Prime 71: 40.4195, 11.3743, 5.03381 | Prime 31: 40.4195, 11.3743, 5.03381
        }
        return hash % MAXHASH;
    }
    
    /// Returns the pair the has the matching key, otherwise returns the end of the subGroup
    Node* findInList(const string &key, Node* subGroup) const {
        Node* currNode = subGroup;
        while (currNode) {
            if (currNode->key == key) {
                return currNode;
            }
            currNode = currNode->right;
        }
        
        return nullptr;
    }
    
    /// Cleans up the string to only include letters
    string cleanString(string str) {
        for (int i = 0; i < str.size(); ++i) {
            if ((str[i] < 'A' || str[i] > 'Z') && (str[i] < 'a' || str[i] > 'z') && (str[i] < '0' || str[i] > '9')) {
                str.erase(i, 1);
                --i;
            }
        }
        return str;
    }
    
    /// Outputs a mulimap with the line number as the key, and the length of the line as the value
    vector<pair<int, int>> getLengths() {
        vector<pair<int, int>> lineLengths;
        for (int i = 0; i < hTable.size(); ++i) {
            int listLen = 0;
            Node* currNode = hTable[i];
            while (currNode) {
                ++listLen;
                currNode = currNode->right;
            }
            lineLengths.push_back(pair<int, int>(i, listLen));
        }
        return lineLengths;
    }
    
    double calculateVariance() {
        vector<pair<int, int>> lineLengths = getLengths();
        //double mean = 0.0;
        double mean2 = accumulate(lineLengths.begin(), lineLengths.end(), 0.0, [](const double & d, const pair<int, int> & p){
            return d + p.second;
        }) / MAXHASH;
        
        //cout << mean2 << endl;
        
        double variance = accumulate(lineLengths.begin(), lineLengths.end(), 0.0, [&mean2, this](const double & d, const pair<int, int> & p){
            return d + (pow(p.second - mean2, 2) / (MAXHASH - 1));
        });
        
        return variance;
        
        //for (pair<int, int> line : lineLengths) {
            //mean += line.second;
        //}
        
        //mean = mean / static_cast<double>(MAXHASH);
    }
    
public:
    /// Constructor for a hash table, given the max size of the table
    HashTable(const int mh): MAXHASH(mh), hTable(MAXHASH)
    {}
    
    /// Inserts the given key with the given value
    void insert(const string &sKey, int value) {
        string key = cleanString(sKey);
        int hash = hashFunc(key);
        Node* subGroup = hTable[hash];
        Node* p = findInList(key, subGroup);
        
        if (!p) {
            Node* newNode = new Node(key, value);
            if (subGroup) {
                subGroup->left = newNode;
                newNode->right = subGroup;
            }
            hTable[hash] = newNode;
        } else {
            // If there is a value already in the list, then update it's value to this value
            p->value = value;
        }
    }
    
    /// Removes the given item from the hash table
    void remove(const string &sKey) {
        string key = cleanString(sKey);
        int hash = hashFunc(key);
        Node* & subGroup = hTable[hash];
        Node* p = findInList(key, subGroup);
        
        if (!p) {
            // If there is no matching key do nothing
            // TODO: Throw error?
        } else {
            // If there is a value, remove it
            if (p->left)
                p->left->right = p->right;
            if (p->right)
                p->right->left = p->left;
            delete p;
        }
    }
    
    /// Increases the word cout of this given word
    void increase(const string &sKey) {
        string key = cleanString(sKey);
        int hash = hashFunc(key);
        Node* & subGroup = hTable[hash];
        Node* p = findInList(key, subGroup);
        
        if (!p) {
            // If there is no matching key throw an error
            throw invalid_argument("Key not in hash");
        } else {
            // If there is a value, add 1
            p->value += 1;
        }
    }
    
    /// Finds the given word in the table
    Node* find(const string &key) {
        int hash = hashFunc(key);
        Node* subGroup = hTable[hash];
        Node* p = findInList(key, subGroup);
        
        return p;
    }
    
    /// Lists all the keys
    vector<string> listAllKeys() {
        vector<string> keys;
        for (int i = 0; i < MAXHASH; ++i) {
            Node* eleList = hTable[i];
            while (eleList) {
                keys.push_back(eleList->key);
                eleList = eleList->right;
            }
        }
        return keys;
    }
    
    /// Displays a histogram of the values in the hash table
    void displayHistogram() {
        cout << "---------- Histogram for size " << MAXHASH << " ----------" << endl;
        vector<pair<int, int>> lineLengths = getLengths();
        for (pair<int, int> line : lineLengths) {
            cout << "Line " << line.first << ": ";
            for (int i = 0; i < line.second; ++i) {
                cout << "*";
            }
            cout << " " << line.second << endl;
        }
        cout << "Variance: " << calculateVariance() << endl;
        cout << "-------------------------------------------" << endl;
    }
    
    /// Displays the 10 largest lines in the hash table
    void displayLongest() {
        vector<pair<int, int>> lineLengths = getLengths();
        sort(lineLengths.begin(), lineLengths.end(), [](const pair<int, int> &f, const pair<int, int> &s){ return f.second > s.second; });
        
        cout << "----- Longest 10 for size " << MAXHASH << " -----" << endl;
        for (int i = 0; i < 10 && i < lineLengths.size(); ++i) {
            cout << "Line " << lineLengths[i].first << ": " << lineLengths[i].second << endl;
        }
        cout << "----------------------------------" << endl;
    }
};


int main(int argc, const char * argv[]) {
    HashTable wordHash30(30);
    HashTable wordHash300(300);
    HashTable wordHash1000(1000);
    
    
    string fileName = "Alice In Wonderland Text";
    ifstream inputFile;
    inputFile.open(fileName + ".txt");
    string line;
    
    if (!inputFile) {
        cout << "Cannot find file" << endl;
        return -1;
    }
    
    // For each line in the file
    while (getline(inputFile, line)) {
        istringstream words(line);
        string word;
        
        // For each word in the line
        while (words >> word) {
            try {
                // Try to increment
                wordHash30.increase(word);
            } catch(const invalid_argument & e) {
                // If it cannot increment, then add the word
                wordHash30.insert(word, 1);
            }
            try {
                // Try to increment
                wordHash300.increase(word);
            } catch(const invalid_argument & e) {
                // If it cannot increment, then add the word
                wordHash300.insert(word, 1);
            }
            try {
                // Try to increment
                wordHash1000.increase(word);
            } catch(const invalid_argument & e) {
                // If it cannot increment, then add the word
                wordHash1000.insert(word, 1);
            }
        }
    }
    
    cout << "########## MAXHASH 30 ##########" << endl;
    vector<string> keys = wordHash30.listAllKeys();
    
    // Output file
    ofstream outputFile("OUTPUT-" + fileName + ".txt");
    outputFile << left << setw(17) << "Word" << setw(5) << "Count" << endl;
    for (string s : keys) {
        Node* p = wordHash30.find(s);
        string str = p->key;
        outputFile << left << setw(17) << p->key << setw(5) << p->value << endl;
    }
    outputFile.close();
    
    wordHash30.displayHistogram();
    wordHash30.displayLongest();
    
    cout << endl << "########## MAXHASH 300 ##########" << endl;
    wordHash300.displayHistogram();
    wordHash300.displayLongest();
    
    cout << endl << "########## MAXHASH 1000 ##########" << endl;
    wordHash1000.displayHistogram();
    wordHash1000.displayLongest();
    
    return 0;
}
