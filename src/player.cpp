#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <numeric>
#include <limits>
#include <algorithm>  
#include <ratio>
#include <chrono>

#include "player.h"
#include "sstream"

using namespace std;


vector<pair<int, list<int>>> kv_buffer; 
int timeout = 0;

int bigNo = 2000000000;

chrono::time_point<std::chrono::system_clock> tic() {
    return chrono::system_clock::now();
}

chrono::duration<double> toc(chrono::time_point<std::chrono::system_clock> start) {
    chrono::time_point<std::chrono::system_clock> end = chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    return elapsed_seconds;
}


void player::GPU_search(othelloBoard board, int maxDepth, int depth, bool maximizingPlayer, 
	int & nodesVisited, vector<pair<int,list<int>>> moveList, pair<int, list<int>> last_kv, chrono::time_point<std::chrono::system_clock> start) {
    //pair<int,pair<int,list<int>>> scoreMove;
    chrono::duration<double> elapsed_seconds = toc(start);
    moveList.push_back(last_kv);
    

    if (elapsed_seconds.count() > 0.99*limit) {
        timeout = 1;
        return;
    }

    unordered_map<int, list<int>> validMoves;
    int bestValue;
    int currSymbol;
    if (maximizingPlayer) {
        currSymbol = symbol;
    } else {
        currSymbol = -symbol;
    }

    //comment out all return
    if (depth < 1) { 
        heuristic.heuristic_gpu(board,60 - board.nMoves,currSymbol);
        kv_buffer.push_back(moveList[0]);
        return;
    }

    board.validMoves(validMoves,currSymbol);

    if (validMoves.size() == 0) {
        unordered_map<int, list<int>> otherValidMoves;
        if (!maximizingPlayer) {
            board.validMoves(otherValidMoves,symbol);
        } else {
            board.validMoves(otherValidMoves,-symbol);
        }
        if (otherValidMoves.size() > 0) {
            GPU_search(board, maxDepth, depth -1, !maximizingPlayer, nodesVisited, moveList, start);
            return;
        } else {
            heuristic.heuristic_gpu(board,60 - board.nMoves,currSymbol);

            kv_buffer.push_back(moveList[0]);
            return;
        }
    }
    for (auto kv : validMoves) {
            othelloBoard scratchBoard = board;
            if (maximizingPlayer) 
                scratchBoard.updatePositions(kv,symbol);
            else
                scratchBoard.updatePositions(kv,-symbol);
            nodesVisited++; 
            GPU_search(scratchBoard, maxDepth, depth -1,  !maximizingPlayer, nodesVisited, moveList, kv,start);
      
    }
    return;
}


void player::GPU_search(othelloBoard board, int maxDepth, int depth, bool maximizingPlayer, 
	int & nodesVisited, vector<pair<int,list<int>>> moveList, chrono::time_point<std::chrono::system_clock> start) {
    chrono::duration<double> elapsed_seconds = toc(start);

    if (elapsed_seconds.count() > 0.99*limit) {

        timeout = 1;
        return;
    }

    unordered_map<int, list<int>> validMoves;
    int bestValue;
    int currSymbol;
    if (maximizingPlayer) {
        currSymbol = symbol;
    } else {
        currSymbol = -symbol;
    }

    //comment out all return
    if (depth < 1) {        

        heuristic.heuristic_gpu(board,60 - board.nMoves,currSymbol);
        kv_buffer.push_back(moveList[0]);

        return;
    }

    board.validMoves(validMoves,currSymbol);

    if (validMoves.size() == 0) {
        unordered_map<int, list<int>> otherValidMoves;
        if (!maximizingPlayer) {
            board.validMoves(otherValidMoves,symbol);
        } else {
            board.validMoves(otherValidMoves,-symbol);
        }
        if (otherValidMoves.size() > 0) {
            GPU_search(board, maxDepth, depth -1, !maximizingPlayer, nodesVisited, moveList, start);
            return;
        } else { 
            heuristic.heuristic_gpu(board,60 - board.nMoves,currSymbol);

            kv_buffer.push_back(moveList[0]);
            return;
        }
    }
    for (auto kv : validMoves) {
            othelloBoard scratchBoard = board;
            if (maximizingPlayer) 
                scratchBoard.updatePositions(kv,symbol);
            else
                scratchBoard.updatePositions(kv,-symbol);
            nodesVisited++; 
            GPU_search(scratchBoard, maxDepth, depth -1,  !maximizingPlayer, nodesVisited, moveList, kv,start);
      
    }
    return;
}


 pair<int, list<int>> test(){
    int size = mobility_buffer.size();
    int maxind = 0;
    int max = -2000000;
    vector<int> value_list;
    for (int i=0;i<size;i++){

            value_list[i]= 5*mobility_buffer[i] + 2*potMobility_buffer[i] + makeup_buffer[i]
             + 5*naivety_buffer[i] + 500*parity_buffer[i] + 1000*corners_buffer[i] + 1000*stability_buffer[i];
        if(value_list[i] > max){ 
            maxind = i;
            max = value_list[i];
        }
    }
    return kv_buffer[maxind];
    
 }

 pair<int, list<int>> test(vector<int> value_list){
    int size = mobility_buffer.size();
    int maxind = 0;
    int max = -2000000;
    for (int i=0;i<size;i++){
        if(value_list[i] > max){
            maxind = i;
            max = value_list[i];
        }
    }
    return kv_buffer[maxind];
    
 }


pair<int, list<int>> player::computerMove_GPU(othelloBoard board, unordered_map<int, list<int>> validMoves) {
        chrono::time_point<std::chrono::system_clock> start = tic();

        int nodesVisited = 0;
        pair<int, list<int>> move;

        if (validMoves.size() == 1) {
            move = *validMoves.begin();
            cout << "Only one valid move." << endl;
                        fprintf(fp,"G nan\n");
            return move;
        }

        /*if (board.nMoves < 18) {
            pair<bool,pair<int,list<int>>> bookLookup = openingDatabase.getMove(validMoves, board.pastMoves);
            if (bookLookup.first) {
                cout << "Move found in opening database." << endl;
                return bookLookup.second;
            } 
        }*/

        pair<int, list<int>> tmpmove;

        unordered_map<int,int> moveOrder;
        pair<int,list<int>> v = *validMoves.begin();

        bool fullSearch = false;    
        int d;

        for (d = 1; d < 60 - board.nMoves + 1; d++) {
            int bestVal = -bigNo;

           /* if (60 - board.nMoves < 10) {
                d = 60 - board.nMoves + 1;
                fullSearch = true;
            }*/
			cout << "Searching at depth " << d << endl;
        	vector<pair<int,list<int>>> moveList;
            GPU_search(board,d, d, true, nodesVisited, moveList,start);
            vector<int> value_list = GPUcaller(mobility_buffer.size());
            try{
                tmpmove = test(value_list);
            }catch(cl::Error e){
                tmpmove=test();
            }
            if(d == 1){
            	move = tmpmove;
            }
            else if(tmpmove.first>move.first){
            	move = tmpmove;
            }
            mobility_buffer.clear();
            potMobility_buffer.clear();
            makeup_buffer.clear();
            naivety_buffer.clear();
            parity_buffer.clear();
            corners_buffer.clear();
            stability_buffer.clear();
            kv_buffer.clear();
            if(timeout){
            	timeout = 0;
            	break;
            }
        }
        
        chrono::duration<double> elapsed_seconds = toc(start);
        if (!fullSearch) {
            cout << "\nCompleted search to depth " << d - 1 << endl;
        }
                    fprintf(fp,"G %d\n",d-1);
        cout << "\nNodes visited: " << nodesVisited << endl; 
        cout << "Time to move: " << elapsed_seconds.count() << " seconds" << endl << endl;
        
        return move;
    }


pair<int, list<int>> player::interactiveMove(unordered_map<int, list<int>> validMoves) {
        pair<int, list<int>> move;
        int ind;
        //interactive selection from list.

        bool validSelection = false;

        do {
            cout << "Pick Move: ";

            string str;
            cin >> str;
            istringstream iss(str);
            iss >> ind;

            if (iss.eof() == false) {
                cout << "Non-integer input, please try again." << endl;
            } else if(ind > validMoves.size() || ind < 0) {
                cout << "Integer selection out of range, please try again" << endl;
            } else {
                validSelection = true;
            }
        } while (!validSelection);

        int i = 0;
        for (auto kv : validMoves) {
            move = kv;
            i++;
            if (i == ind) {
                break;
            }
        }

        return move;
    }



//-----------------------CPU parallelization

pair<int,pair<int,list<int>>> player::alphaBeta_CPU(othelloBoard board, int maxDepth, int depth, unordered_map<int,int> & moveOrder, int alpha, int beta, bool maximizingPlayer, int & nodesVisited, chrono::time_point<std::chrono::system_clock> start) {
    pair<int,pair<int,list<int>>> scoreMove;
    chrono::duration<double> elapsed_seconds = toc(start);
    if (elapsed_seconds.count() > 0.99*limit) {
        if (!maximizingPlayer) {
            scoreMove.first = numeric_limits<int>::max() -1;
            return scoreMove;
        } else if(maximizingPlayer) {
            scoreMove.first = numeric_limits<int>::min() +1;
            return scoreMove;
        }
    }

    unordered_map<int, list<int>> validMoves;
    int bestValue;
    int currSymbol;

    if (maximizingPlayer) {
        currSymbol = symbol;
    } else {
        currSymbol = -symbol;
    }

    if (depth < 1) {        
        // call heuristic - in this case most piece
        bestValue = heuristic.heuristic(board,60 - board.nMoves,currSymbol);
        if (symbol == -1)
            bestValue = -1*bestValue;
        scoreMove.first = bestValue;
        return scoreMove;
    }

    board.validMoves(validMoves,currSymbol);

    if (validMoves.size() == 0) {
        unordered_map<int, list<int>> otherValidMoves;
        if (!maximizingPlayer) {
            board.validMoves(otherValidMoves,symbol);
        } else {
            board.validMoves(otherValidMoves,-symbol);
        }
        if (otherValidMoves.size() > 0) {
            scoreMove = alphaBeta_CPU(board, maxDepth, depth -1, moveOrder, alpha, beta, !maximizingPlayer, nodesVisited, start);
            return scoreMove;
        } else {
            bestValue = heuristic.heuristic(board,60 - board.nMoves,currSymbol);
            if (symbol == -1)
                bestValue = -1*bestValue;
            scoreMove.first = bestValue;
            return scoreMove;
        }
    }
    pair<int,pair<int,list<int>>> tmpScoreMove;
    if (maximizingPlayer) {
        scoreMove.first = -bigNo;
            int k = moveOrder[maxDepth - depth];
        if(validMoves.find(k) != validMoves.end() ) {
                pair<int,list<int>> kv = *validMoves.find(k);
                othelloBoard scratchBoard = board;
                scratchBoard.updatePositions(kv,symbol);
                nodesVisited++;
                tmpScoreMove = alphaBeta_CPU(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, false, nodesVisited, start);
                if (tmpScoreMove.first > scoreMove.first) {
                    scoreMove.first = tmpScoreMove.first;
                    scoreMove.second = kv;
                }
                alpha = max(alpha,scoreMove.first);
                validMoves.erase(k);
            }
 
    }else{
        scoreMove.first = bigNo;
            int k = moveOrder[maxDepth - depth]; 
            if (validMoves.find(k) != validMoves.end()) {
                pair<int,list<int>> kv = *validMoves.find(k);
                othelloBoard scratchBoard = board;
                scratchBoard.updatePositions(kv,-symbol);
                nodesVisited++;
                tmpScoreMove = alphaBeta_CPU(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, true, nodesVisited, start);         
                if (tmpScoreMove.first < scoreMove.first) {
                    scoreMove.first = tmpScoreMove.first;
                    scoreMove.second = kv;
                }
                beta = min(beta,scoreMove.first);
                validMoves.erase(k);
            } 
    }
    bool pruned = false;
    #pragma omp parallel private(tmpScoreMove) \
            shared(alpha, beta, maxDepth, depth, board, pruned)\
            num_threads(4)
    {  
    #pragma omp single
    {
    for (auto kv : validMoves) {
        #pragma omp flush (pruned)
        if (!pruned){
            if (maximizingPlayer) {

                // do for potential best move first, then remove it from valid moves list

                    othelloBoard scratchBoard = board;
                    scratchBoard.updatePositions(kv,symbol);
                    nodesVisited++;
                    tmpScoreMove = alphaBeta_CPU(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, false, nodesVisited, start);
                    if (tmpScoreMove.first > scoreMove.first) {
                        #pragma omp critical
                        scoreMove.first = tmpScoreMove.first;
                        scoreMove.second = kv;
                    }
                    #pragma omp flush(alpha)
                    if(alpha > scoreMove.first){
                        #pragma omp critical
                        pruned = true;
                        alpha = max(alpha,scoreMove.first);
                    }
                    if (beta <= alpha) {
                        break;
                    }
                    
            } else {
                // do for potential best move first, then remove it from valid moves list
               
                    othelloBoard scratchBoard = board;
                    scratchBoard.updatePositions(kv,-symbol);
                    nodesVisited++;
                    tmpScoreMove = alphaBeta_CPU(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, true, nodesVisited, start);        
                    if (tmpScoreMove.first < scoreMove.first) {
                        #pragma omp critical
                        scoreMove.first = tmpScoreMove.first;
                        scoreMove.second = kv;
                    }
                    #pragma omp flush(beta)
                    if(beta > scoreMove.first){ 
                        #pragma omp critical
                        pruned = true;
                        beta = min(beta,scoreMove.first);
                    }
                    if (beta <= alpha) {
                        break;
                    }
                    
            }
        }
    }
    }
    }
    moveOrder[maxDepth - depth] = scoreMove.second.first;
    return scoreMove;
    
}


pair<int, list<int>> player::computerMove_CPU(othelloBoard board, unordered_map<int, list<int>> validMoves) {
        chrono::time_point<std::chrono::system_clock> start = tic();

        int nodesVisited = 0;
        pair<int, list<int>> move;

        if (validMoves.size() == 1) {
            move = *validMoves.begin();
            cout << "Only one valid move." << endl;
                        fprintf(fp,"C nan\n");
            return move;
        }

        /*if (board.nMoves < 18) {
            pair<bool,pair<int,list<int>>> bookLookup = openingDatabase.getMove(validMoves, board.pastMoves);
            if (bookLookup.first) {
                cout << "Move found in opening database." << endl;
                return bookLookup.second;
            } 
        }*/

        pair<int, list<int>> tmpmove;

        unordered_map<int,int> moveOrder;
        pair<int,list<int>> v = *validMoves.begin();
        moveOrder[0] = v.first;

        int alpha = -bigNo;
        int beta = bigNo;
        bool fullSearch = false;    
        int d;

        for (d = 1; d < 60 - board.nMoves + 1; d++) {
            int bestVal = -bigNo;

            if (60 - board.nMoves < 10) {
                d = 60 - board.nMoves + 1;
                fullSearch = true;
            }

            cout << "Searching at depth " << d << endl;

            pair<int,pair<int,list<int>>> moveScore = alphaBeta_CPU(board,d, d, moveOrder, alpha, beta, true, nodesVisited, start);
            if (moveScore.first > bestVal && moveScore.first != (numeric_limits<int>::max() -1) && moveScore.first != (numeric_limits<int>::min() -1)) {
                bestVal = moveScore.first;
                tmpmove = moveScore.second;
            } else if (moveScore.first == bestVal && moveScore.first != (numeric_limits<int>::max() -1) && moveScore.first != (numeric_limits<int>::min() -1)) {
                if ((rand() % 100)  > 50) {
                    bestVal = moveScore.first;
                    tmpmove = moveScore.second;
                }
            }
            if (moveScore.first != (numeric_limits<int>::max() -1) && moveScore.first != (numeric_limits<int>::min() +1)) {
                    move = tmpmove;
            } else {
                break;
            }
        }

        chrono::duration<double> elapsed_seconds = toc(start);
        if (!fullSearch) {
            cout << "\nCompleted search to depth " << d - 1 << endl;
        }
                    fprintf(fp,"C %d\n",d-1);
        cout << "\nNodes visited: " << nodesVisited << endl; 
        cout << "Time to move: " << elapsed_seconds.count() << " seconds" << endl << endl;

        return move;
    }

//-------------------------------CPU parallization

//-------------------------------No parallization


pair<int,pair<int,list<int>>> player::alphaBeta(othelloBoard board, int maxDepth, int depth, unordered_map<int,int> & moveOrder, int alpha, int beta, bool maximizingPlayer, int & nodesVisited, chrono::time_point<std::chrono::system_clock> start) {
    pair<int,pair<int,list<int>>> scoreMove;
    chrono::duration<double> elapsed_seconds = toc(start);
    if (elapsed_seconds.count() > 0.99*limit) {
        if (!maximizingPlayer) {
            scoreMove.first = numeric_limits<int>::max() -1;
            return scoreMove;
        } else if(maximizingPlayer) {
            scoreMove.first = numeric_limits<int>::min() +1;
            return scoreMove;
        }
    }

    unordered_map<int, list<int>> validMoves;
    int bestValue;
    int currSymbol;
    if (maximizingPlayer) {
        currSymbol = symbol;
    } else {
        currSymbol = -symbol;
    }

    if (depth < 1) {        
        // call heuristic - in this case most piece
        bestValue = heuristic.heuristic(board,60 - board.nMoves,currSymbol);
        if (symbol == -1)
            bestValue = -1*bestValue;
        scoreMove.first = bestValue;
        return scoreMove;
    }

    board.validMoves(validMoves,currSymbol);

    if (validMoves.size() == 0) {
        unordered_map<int, list<int>> otherValidMoves;
        if (!maximizingPlayer) {
            board.validMoves(otherValidMoves,symbol);
        } else {
            board.validMoves(otherValidMoves,-symbol);
        }
        if (otherValidMoves.size() > 0) {
            scoreMove = alphaBeta(board, maxDepth, depth -1, moveOrder, alpha, beta, !maximizingPlayer, nodesVisited, start);
            return scoreMove;
        } else {
            bestValue = heuristic.heuristic(board,60 - board.nMoves,currSymbol);
            if (symbol == -1)
                bestValue = -1*bestValue;
            scoreMove.first = bestValue;
            return scoreMove;
        }
    }
    pair<int,pair<int,list<int>>> tmpScoreMove;
    if (maximizingPlayer) {
        scoreMove.first = -bigNo;
        // do for potential best move first, then remove it from valid moves list
        int k = moveOrder[maxDepth - depth];
        if(validMoves.find(k) != validMoves.end() ) {
            pair<int,list<int>> kv = *validMoves.find(k);
            othelloBoard scratchBoard = board;
            scratchBoard.updatePositions(kv,symbol);
            nodesVisited++;
            tmpScoreMove = alphaBeta(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, false, nodesVisited, start);
            if (tmpScoreMove.first > scoreMove.first) {
                scoreMove.first = tmpScoreMove.first;
                scoreMove.second = kv;
            }
            alpha = max(alpha,scoreMove.first);
            validMoves.erase(k);
        }

        for (auto kv : validMoves) {
            othelloBoard scratchBoard = board;
            scratchBoard.updatePositions(kv,symbol);
            nodesVisited++;
            tmpScoreMove = alphaBeta(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, false, nodesVisited, start);
            if (tmpScoreMove.first > scoreMove.first) {
                scoreMove.first = tmpScoreMove.first;
                scoreMove.second = kv;
            }
            alpha = max(alpha,scoreMove.first);
            if (beta <= alpha) {
                break;
            }
        } 
        moveOrder[maxDepth - depth] = scoreMove.second.first;
        return scoreMove;
    } else {
        scoreMove.first = bigNo;
        // do for potential best move first, then remove it from valid moves list
        int k = moveOrder[maxDepth - depth];
        if (validMoves.find(k) != validMoves.end()) {
            pair<int,list<int>> kv = *validMoves.find(k);
            othelloBoard scratchBoard = board;
            scratchBoard.updatePositions(kv,-symbol);
            nodesVisited++;
            tmpScoreMove = alphaBeta(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, true, nodesVisited, start);         
            if (tmpScoreMove.first < scoreMove.first) {
                scoreMove.first = tmpScoreMove.first;
                scoreMove.second = kv;
            }
            beta = min(beta,scoreMove.first);
            validMoves.erase(k);
        }

        for (auto kv : validMoves) {
            othelloBoard scratchBoard = board;
            scratchBoard.updatePositions(kv,-symbol);
            nodesVisited++;
            tmpScoreMove = alphaBeta(scratchBoard, maxDepth, depth -1, moveOrder, alpha, beta, true, nodesVisited, start);         
            if (tmpScoreMove.first < scoreMove.first) {
                scoreMove.first = tmpScoreMove.first;
                scoreMove.second = kv;
            }
            beta = min(beta,scoreMove.first);
            if (beta <= alpha) {
                break;
            }
        }
        moveOrder[maxDepth - depth] = scoreMove.second.first;
        return scoreMove;
    }
}

pair<int, list<int>> player::computerMove(othelloBoard board, unordered_map<int, list<int>> validMoves) {
        chrono::time_point<std::chrono::system_clock> start = tic();

        int nodesVisited = 0;
        pair<int, list<int>> move;

        if (validMoves.size() == 1) {
            move = *validMoves.begin();
            cout << "Only one valid move." << endl;
            fprintf(fp,"O nan\n");
            return move;
        }

        /*if (board.nMoves < 18) {
            pair<bool,pair<int,list<int>>> bookLookup = openingDatabase.getMove(validMoves, board.pastMoves);
            if (bookLookup.first) {
                cout << "Move found in opening database." << endl;
                return bookLookup.second;
            } 
        }*/

        pair<int, list<int>> tmpmove;

        unordered_map<int,int> moveOrder;
        pair<int,list<int>> v = *validMoves.begin();
        moveOrder[0] = v.first;

        int alpha = -bigNo;
        int beta = bigNo;
        bool fullSearch = false;    
        int d;

        for (d = 1; d < 60 - board.nMoves + 1; d++) {
            int bestVal = -bigNo;

            if (60 - board.nMoves < 10) {
                d = 60 - board.nMoves + 1;
                fullSearch = true;
            }

            cout << "Searching at depth " << d << endl;

            pair<int,pair<int,list<int>>> moveScore = alphaBeta(board,d, d, moveOrder, alpha, beta, true, nodesVisited, start);
            if (moveScore.first > bestVal && moveScore.first != (numeric_limits<int>::max() -1) && moveScore.first != (numeric_limits<int>::min() -1)) {
                bestVal = moveScore.first;
                tmpmove = moveScore.second;
            } else if (moveScore.first == bestVal && moveScore.first != (numeric_limits<int>::max() -1) && moveScore.first != (numeric_limits<int>::min() -1)) {
                if ((rand() % 100)  > 50) {
                    bestVal = moveScore.first;
                    tmpmove = moveScore.second;
                }
            }
            if (moveScore.first != (numeric_limits<int>::max() -1) && moveScore.first != (numeric_limits<int>::min() +1)) {
                    move = tmpmove;
            } else {
                break;
            }
        }

        chrono::duration<double> elapsed_seconds = toc(start);
        if (!fullSearch) {
            cout << "\nCompleted search to depth " << d - 1 << endl;
        }
        fprintf(fp,"O %d\n",d-1);
        cout << "\nNodes visited: " << nodesVisited << endl; 
        cout << "Time to move: " << elapsed_seconds.count() << " seconds" << endl << endl;

        return move;
    }


//--------------------------------------no parallization


player::player(int a, bool b, int c, int d, heuristicEvaluation e) {
        playerType = a;
        playerId = b; // implicit conversion from bool to int
        n = c; // number of entries on board
        symbol = d;
        if (playerType < 4) {
            openings db;
            db.sequences = db.generateData(symbol);
            openingDatabase = db;
            heuristic = e;
        }
    }

pair<int, list<int>> player::selectMove(othelloBoard board, unordered_map<int, list<int>> validMoves) { 
        pair<int, list<int>> kv;
        switch (playerType) {
            case 1:
                cout << "Regular AlphaBeta AI Selects Move\n";
                kv = computerMove(board, validMoves);
                break;

            case 2:
                cout << "CPU Parallism AlphaBeta AI Selects Move\n";
                kv = computerMove_CPU(board, validMoves);                
                break;

            case 3:
                cout << "GPU Parallism AI Selects Move\n";
                kv = computerMove_GPU(board, validMoves);                 
                break;

            case 4:                
                cout << "Human Selects Move\n";
                kv = interactiveMove(validMoves);
                break;

        }
        return kv;
    }