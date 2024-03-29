// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
//
//  poke.cpp
//  project4
//
//  Created by Rishi Barad on 12/2/19.
//  Copyright © 2019 Rishi Barad. All rights reserved.
//

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include "xcode_redirect.hpp"
#include "getopt.h"

using namespace std;


//From Instructor Piazza Post
struct Pokemon {
    double dist = std::numeric_limits<double>::infinity(); //dv
    int xCoord;
    int yCoord;
    int parent = -1; //pv
    bool visited = false; //kv
    char terrain = 'l';
};

class mstDistance {
public:
    double operator()(const Pokemon* p1, const Pokemon* p2) {
        if ((p1->terrain == 'l' && p2->terrain == 's') ||
            (p1->terrain == 's' && p2->terrain == 'l')) {
            return std::numeric_limits<double>::infinity();
        }
        double firstDif = p1->xCoord - p2->xCoord;
        double secondDif = p1->yCoord - p2->yCoord;
        return sqrt((firstDif*firstDif) + (secondDif*secondDif));
    }
};

class tspCost {
public:
    double operator()(const Pokemon* p1, const Pokemon* p2) {
        double firstDif = p1->xCoord - p2->xCoord;
        double secondDif = p1->yCoord - p2->yCoord;
        return sqrt((firstDif*firstDif) + (secondDif*secondDif));
    }
};


class mainPokeDex {
public:
    void getOptions(int argc, char** argv);
    
    void readData();
    
    void constructMST();
    
    void printMST();

    void fastTSP();
    
    void printTSP();
        
    void genPerms(size_t permLength);
    
    bool promising(size_t permLength);
    
    ~mainPokeDex() {
        for (auto p : pokeDex) {
            delete p;
        }
        pokeDex.clear();
    }
    
private:
    vector<Pokemon*> pokeDex;
    vector<int> tspCycle;
    vector<int> bestTspCycle;
    char mode;
    double weight = 0;
    double bestWeight = std::numeric_limits<double>::infinity();
    //double lowerBound = std::numeric_limits<double>::infinity();
    bool sea = false;
    bool coast = false;
    bool land = false;
};


/* ****************************** MAIN ************************************** */

int main(int argc, char** argv) {
    //speed optimization commands
    xcode_redirect(argc, argv);
    // This should be in all of your projects, speeds up I/O
    ios_base::sync_with_stdio(false);
    
    cout << std::setprecision(2); //Always show 2 decimal places
    cout << std::fixed; //Disable scientific notation for large numbers
    
    mainPokeDex startGame;
    startGame.getOptions(argc, argv);
    startGame.readData();
    
    
    return 0;
}

/* ****************************** MAIN ************************************** */

void mainPokeDex::getOptions(int argc, char** argv) {
    int option_index = 0, option = 0;
    
    // Don't display getopt error messages about options
    opterr = false;
    
    struct option longOpts[] = {{ "help", no_argument, nullptr, 'h' },
                                { "mode", required_argument, nullptr, 'm' },
                                { nullptr, 0, nullptr, '\0' }};
    
    while ((option = getopt_long(argc, argv, "hm:",
                             longOpts, &option_index)) != -1) {
    
        switch (option) {
            case 'h':
                cout << "### Supported Options\n"
                << "* --help, -h: Print a useful help message and exit, ignores all other options\n"
                << "Valid examples of how to execute the program:\n"
                << "poke --mode MST          (OK, but you must type the input by hand)\n"
                << "poke -h < inputFile.txt  (OK, -h happens before we realize there’s no -m)\n"
                << "poke -m OPTTSP < inputFile.txt  (OK, reads from a file on disk)\n"
                << "poke -m BLAH   (BAD mode following -m)\n";
                exit(0);
            case 'm':
                if (!strcmp(optarg, "MST")) {
                    mode = 'a';
                } else if (!strcmp(optarg, "FASTTSP")) {
                    mode = 'b';
                } else if (!strcmp(optarg, "OPTTSP")) {
                    mode = 'c';
                } else {
                    cerr << "faulty MODE\n";
                    exit(1);
                }
                break;
            default:
                cerr << "faulty command line\n";
                exit(1);
        }
    }
}

void mainPokeDex::readData() {
    int numPokemon;
    int xCoord;
    int yCoord;
    
    cin >> numPokemon;

    while (cin >> xCoord >> yCoord) {
        Pokemon* wildPokemon = new Pokemon;
        //land is default
        wildPokemon->xCoord = xCoord;
        wildPokemon->yCoord = yCoord;
        
        if (yCoord < 0 && xCoord < 0) {
            //sea
            wildPokemon->terrain = 's';
            if (!sea) {
                sea = true;
            }
        } else if ((yCoord < 0 && xCoord == 0) || (xCoord < 0 && yCoord == 0)) {
            //on coast
            wildPokemon->terrain = 'c';
            if (!coast) {
                coast = true;
            }
        } else {
            if (!land) {
                land = true;
            }
        }
        //add to pokeDex
        pokeDex.push_back(wildPokemon);
    }
    
    if (mode == 'a') {
        if (land && sea && !coast) {
            cerr << "Cannot construct MST\n";
            exit(1);
        }
        constructMST();
    } else if (mode == 'b') {
        fastTSP();
        printTSP();
    } else {
        fastTSP();
        weight = 0;
        genPerms(1);
        printTSP();
    }
    

    return;
}

void mainPokeDex::constructMST() {
    //PRIM'S ALGORITHM - MST with linear pass. More efficient for dense graphs
    
    //assign root
    pokeDex[0]->dist = 0;
    //loop through this V times
    for (unsigned int i = 0; i < pokeDex.size(); i++) {
        unsigned int j = 0;
        int targetIndex = -1;
        double lowestDist = std::numeric_limits<double>::infinity();
        /* 1. From the set of vertices for which kv is false,
         select the vertex v having the smallest tentative distance dv. */
        while (j < pokeDex.size()) {
            if (!pokeDex[j]->visited && pokeDex[j]->dist < lowestDist) {
                lowestDist = pokeDex[j]->dist;
                targetIndex = j;
            }//if
            j++;
        }//while
        
        /* 2. Set kv to true. */
        pokeDex[targetIndex]->visited = true;
        weight += pokeDex[targetIndex]->dist;

        /* 3. For each vertex w adjacent to v for which kw is false,
         test whether dw is greater than distance(v,w).
         If it is, set dw to distance(v,w) and set pw to v. */
        j = 0;
        while (j < pokeDex.size()) {
            if (!pokeDex[j]->visited) {
                double tempDist = mstDistance()(pokeDex[targetIndex],pokeDex[j]);
                if (pokeDex[j]->dist > tempDist) {
                    pokeDex[j]->parent = targetIndex;
                    pokeDex[j]->dist = tempDist;
                }//if
            }//if
            j++;
        }//while
    }//for
    printMST();
    return;
}

void mainPokeDex::fastTSP() {
    //Fast TSP with Arbitrary Insertion Heuristic
    for (unsigned int i = 0; i < pokeDex.size(); i++) {
        if (i < 3) {
            //Initialization
            tspCycle.push_back(i);
            if (i == 2) {
                //calculate cost
                weight = tspCost()(pokeDex[tspCycle[0]],pokeDex[tspCycle[1]]) +
                tspCost()(pokeDex[tspCycle[1]],pokeDex[tspCycle[2]]) +
                tspCost()(pokeDex[tspCycle[0]],pokeDex[tspCycle[2]]);
            }
        } else {
            int insertIndex = -1;
            double lowestDist = std::numeric_limits<double>::infinity();
            double tempDist = std::numeric_limits<double>::infinity();
            
            for (unsigned int j = 0; j < tspCycle.size(); j++) {
                //sqrt(Cost(i,k)) + sqrt(Cost(j,k)) - sqrt(Cost(I,j))
                if (j != tspCycle.size()-1) {
                    tempDist = tspCost()(pokeDex[tspCycle[j]],pokeDex[i]) +
                    tspCost()(pokeDex[tspCycle[j+1]],pokeDex[i]) -
                    tspCost()(pokeDex[tspCycle[j]],pokeDex[tspCycle[j+1]]);
                } else {
                    tempDist = tspCost()(pokeDex[tspCycle[j]],pokeDex[i]) +
                    tspCost()(pokeDex[tspCycle[0]],pokeDex[i]) -
                    tspCost()(pokeDex[tspCycle[j]],pokeDex[tspCycle[0]]);
                }
                if (tempDist < lowestDist) {
                    insertIndex = j;
                    lowestDist = tempDist;
                }
            }//for
            //add after index
            if (insertIndex == int(tspCycle.size()-1)) {
                tspCycle.push_back(i);
            } else {
                //insert
                tspCycle.insert(tspCycle.begin()+insertIndex+1, i);
            }//else
            //adjust cost
            weight += lowestDist;
        }//else
    }
    return;
}

void mainPokeDex::genPerms(size_t permLength) {
  if (tspCycle.size() == permLength) {
    // Do something with the path
      weight += tspCost()(pokeDex[tspCycle[0]],pokeDex[tspCycle[tspCycle.size()-1]]);
      if (weight < bestWeight) {
          bestTspCycle = tspCycle;
          bestWeight = weight;
      }
      weight -= tspCost()(pokeDex[tspCycle[0]],pokeDex[tspCycle[tspCycle.size()-1]]);
    return;
  } // if
  if (!promising(permLength))
    return;
  for (size_t i = permLength; i < tspCycle.size(); ++i) {
    swap(tspCycle[permLength], tspCycle[i]);

      weight += tspCost()(pokeDex[tspCycle[permLength]],pokeDex[tspCycle[permLength-1]]);
      
    genPerms(permLength + 1);

      weight -= tspCost()(pokeDex[tspCycle[permLength]],pokeDex[tspCycle[permLength-1]]);

      
    swap(tspCycle[permLength], tspCycle[i]);
  } // for
} // genPerms()

bool mainPokeDex::promising(size_t permLength) {
    if (tspCycle.size() - permLength < 5) {
        return true;
    }
    
    double mstCost = 0;
        
    //re-initialize everything to false/infinity
    for (size_t i = permLength; i < tspCycle.size(); i++) {
        pokeDex[tspCycle[i]]->dist = std::numeric_limits<double>::infinity();
        pokeDex[tspCycle[i]]->parent = -1;
        pokeDex[tspCycle[i]]->visited = false;
    }

    //assign root
    pokeDex[tspCycle[permLength]]->dist = 0;

    //loop through this V times
    for (size_t i = permLength; i < tspCycle.size(); i++) {
        size_t j = permLength;
        int targetIndex = -1;
        double lowestDist = std::numeric_limits<double>::infinity();
        /* 1. From the set of vertices for which kv is false,
         select the vertex v having the smallest tentative distance dv. */
        while (j < tspCycle.size()) {
            if (!pokeDex[tspCycle[j]]->visited &&
                pokeDex[tspCycle[j]]->dist < lowestDist) {
                lowestDist = pokeDex[tspCycle[j]]->dist;
                targetIndex = int(j);
            }//if
            j++;
        }//while
        
        /* 2. Set kv to true. */
        pokeDex[tspCycle[targetIndex]]->visited = true;
        mstCost += pokeDex[tspCycle[targetIndex]]->dist;

        /* 3. For each vertex w adjacent to v for which kw is false,
         test whether dw is greater than distance(v,w).
         If it is, set dw to distance(v,w) and set pw to v. */
        j = permLength;
        while (j < tspCycle.size()) {
            if (!pokeDex[tspCycle[j]]->visited) {
                double tempDist = tspCost()(pokeDex[tspCycle[targetIndex]],pokeDex[tspCycle[j]]);
                if (pokeDex[tspCycle[j]]->dist > tempDist) {
                    pokeDex[tspCycle[j]]->parent = targetIndex;
                    pokeDex[tspCycle[j]]->dist = tempDist;
                }//if
            }//if
            j++;
        }//while
    }//for

    double leastWeightA = std::numeric_limits<double>::infinity();
    double leastWeightB = std::numeric_limits<double>::infinity();

    for (size_t i = permLength; i < tspCycle.size(); i++) {
        double tempWeightA = tspCost()(pokeDex[tspCycle[i]],pokeDex[tspCycle[0]]);
        if (tempWeightA < leastWeightA) {
            leastWeightA = tempWeightA;
        }
        
        double tempWeightB = tspCost()(pokeDex[tspCycle[i]],pokeDex[tspCycle[permLength-1]]);
        if (tempWeightB < leastWeightB) {
            leastWeightB = tempWeightB;
        }
    }
    mstCost += weight + leastWeightA + leastWeightB;

    if (mstCost < bestWeight) {
        return true;
    }
    return false;
}


void mainPokeDex::printMST() {
    //print weight
    cout << weight << '\n';
    //print edges, with smallest integer first
    for (int i = 0; i < int(pokeDex.size()); i++) {
        if (pokeDex[i]->parent >= 0) {
            if (i < pokeDex[i]->parent) {
                cout << i << ' ' << pokeDex[i]->parent << '\n';
            } else {
                cout << pokeDex[i]->parent << ' ' << i << '\n';
            }
        }
    }
}

void mainPokeDex::printTSP() {
    if (mode == 'b') {
        cout << weight << '\n';
    
        for (int i = 0; i < int(tspCycle.size()); i++) {
            cout << tspCycle[i] << ' ';
        }
    } else {
        cout << bestWeight << '\n';
        
        for (int i = 0; i < int (bestTspCycle.size()); i++) {
            cout << bestTspCycle[i] << ' ';
        }
    }
}
