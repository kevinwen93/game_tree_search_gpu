#include <iostream>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <limits.h>
#include <assert.h>
#include <sstream>
#include "game.h"
FILE* fp;
float getTimeLimit() {
    float limit;
    bool validSelection = false;
    do {
        cout << "Pick time limit for computer: (seconds) ";        

        string str;
        cin >> str;
        istringstream iss(str);
        iss >> limit;

        if (iss.eof() == false) {
            cout << "Non-numeric input, please try again." << endl;
        } else if(limit < 0) {
            cout << "Negative numeric input, please try a positive number" << endl;
        } else {
            validSelection = true;
        }
    } while (!validSelection);

    return limit;
}

int checkCPU(int id) {
    char c;
    bool validSelection = false;
    do {
        cout << "What Type of Player You Want Player" 
        << id << " To Be? \n\t1 -> Regular AlphaBeta AI\n\t2 -> CPU Parallism AlphaBeta AI\n\t3 -> GPU Parallism AI\n\t4 -> Human Player\nSelection: ";      

        string str;
        cin >> str;
        istringstream iss(str);
        iss >> c;

        iss.ignore(numeric_limits <streamsize> ::max(), '\n' );

        if (iss.eof() == false) {
            cout << "Non-single character input, please try again.\n" << endl;
        } else 
        if(c != '1' && c != '2' && c != '3' && c != '4') {
            cout << "Did not enter '1', '2', '3' or '4'. Please try again." << endl;
        } else {
            validSelection = true;
        }
    } while (!validSelection);
    return (int)(c-'0');
}

int main (int argc, char* argv[]) {
    othelloBoard board;

   /* int choice;
    cout << "Load a game or start a new one?\n";
    cout << "1 -> Load a saved board state\n";
    cout << "2 -> Start a new game\n";

    bool validSelection = false;

    do {
        cout << "Selection: ";

        string str;
        cin >> str;
        istringstream iss(str);
        iss >> choice;

        if (iss.eof() == false) {
            cout << "Non-integer input, please try again." << endl;
        } else if(choice > 2 || choice < 0) {
            cout << "Integer selection out of range, please try again" << endl;
        } else {
            validSelection = true;
        }
    } while (!validSelection);
*/

    if(argc == 2) {
        string address ("result/");
        address.append(argv[1]);
        const char * add = address.c_str();
        fp = fopen(add,"w");
    }

    othelloGame game (&board);  

    //if (choice == 1)
        game.newGame = true;

    bool whiteMovesFirst = false;
    int cpu1;
    int cpu2;
    float limit;

    if (game.newGame) {

        cpu1 = checkCPU(1);
        cpu2 = checkCPU(2);
        cout<<"cpu1 now is "<< cpu1 << endl;
        if (cpu1 < 4|| cpu2 < 4) {
            limit = getTimeLimit();
        }   

        cout << "New Game\n";
        game.firstMove();

    } else {
        string filename;
        cout << "Give filename of savefile: ";
        cin >> filename;
        game.loadGame(filename, whiteMovesFirst, limit);

        cpu1 = checkCPU(1);
        cpu2 = checkCPU(2);

    }

    heuristicEvaluation h;

    // humanPlayer, playerId, n, symbol 
    player playerOne (cpu1, 1, board.n,-1, h); // black
    player playerTwo (cpu2, 0, board.n,1,  h);  // white

    if (cpu1 < 4|| cpu2 <4) {
        playerOne.limit = limit;
        playerTwo.limit = limit;
        //fprintf(fp,"Time limit is %d\n",limit);
    }

    if (whiteMovesFirst) {
        game.move(playerTwo);
        game.statusUpdate();
    }

    while (!game.complete) {
        game.move(playerOne); // player one moves
        game.move(playerTwo);
        game.statusUpdate(); // updates value of game.complete 
    };  
}
