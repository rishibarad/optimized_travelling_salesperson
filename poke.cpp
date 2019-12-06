// Project Identifier: 5949F553E20B650AB0FB2266D3C0822B13D248B0
//
//  main.cpp
//  project4
//
//  Created by Rishi Barad on 12/2/19.
//  Copyright © 2019 Rishi Barad. All rights reserved.
//

#include <iostream>
#include <vector>
#include <deque>
#include <algorithm>
#include "xcode_redirect.hpp"
#include "getopt.h"

using namespace std;




//From genPerms.txt file
template <typename T>
void genPerms(vector<T> &path, size_t permLength) {
  if (path.size() == permLength) {
    // Do something with the path
    return;
  } // if
  if (!promising(path, permLength))
    return;
  for (size_t i = permLength; i < path.size(); ++i) {
    swap(path[permLength], path[i]);
      //TODO: From video (15:17). calculate the weight of the new edge added
      
    genPerms(path, permLength + 1);
      
      //TODO: now subtract that edge weight before the swap cal
      
    swap(path[permLength], path[i]);
  } // for
} // genPerms()


//From Instructor Piazza Post
struct Pokemon {
    double dist = std::numeric_limits<double>::infinity();
    int xCoord;
    int yCoord;
    int parent = -1;
    bool visited = false;
    char terrain = 'l';
};

class mainPokeDex {
public:
    void getOptions(int argc, char** argv);
    void readData();
    void constructMST();
    
private:
    vector<Pokemon*> pokeDex;
    char mChoice;
};

/* ****************************** MAIN ************************************** */

int main(int argc, char** argv) {
    //speed optimization commands
    xcode_redirect(argc, argv);
    // This should be in all of your projects, speeds up I/O
    ios_base::sync_with_stdio(false);
    
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
                    mChoice = 'a';
                } else if (!strcmp(optarg, "FASTTSP")) {
                    mChoice = 'b';
                } else if (!strcmp(optarg, "OPTTSP")) {
                    mChoice = 'c';
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
        //create struct. add struct ptrs  to vector?
        Pokemon* wildPokemon = new Pokemon;
        //land is default
        wildPokemon->xCoord = xCoord;
        wildPokemon->yCoord = yCoord;
        
        if (yCoord < 0 && xCoord < 0) {
            //sea
            wildPokemon->terrain = 's';
        } else if ((yCoord < 0 && xCoord == 0) || (xCoord < 0 &&yCoord == 0)) {
            //on coast
            wildPokemon->terrain = 'c';
        }
        
        //add to pokeDex
        pokeDex.push_back(wildPokemon);
    }
    
    if (mChoice == 'a') {
        constructMST();
    } else if (mChoice == 'b') {
        
    } else {
        
    }

    return;
}

void mainPokeDex::constructMST() {
    //MST with linear pass. More efficient for our project's dense graphs
    
    
    
    
    return;
}