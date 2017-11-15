#ifndef PLAYER_H
#define PLAYER_H

#include <list>
#include <chrono>
#include "openings.h"
#include "heuristicEvaluation.h"
#include "gpu.hpp"
#include "external.h"
extern FILE * fp;

using namespace std;

class player {
    int playerType;

    int n;

    openings openingDatabase;
    heuristicEvaluation heuristic;
    void GPU_search(othelloBoard board, int maxDepth, int depth, bool maximizingPlayer, 
        int & nodesVisited, vector<pair<int,list<int>>> moveList, chrono::time_point<std::chrono::system_clock> start);
    void GPU_search(othelloBoard board, int maxDepth, int depth, bool maximizingPlayer, 
        int & nodesVisited, vector<pair<int,list<int>>> moveList, pair<int, list<int>> last_kv,chrono::time_point<std::chrono::system_clock> start);
  
    pair<int, list<int>> computerMove_CPU(othelloBoard board, unordered_map<int, list<int>> validMoves);
    pair<int,pair<int,list<int>>> alphaBeta_CPU(othelloBoard board, int maxDepth, int depth, unordered_map<int,int> & moveOrder, int alpha, int beta, bool maximizingPlayer, int & nodesVisited, chrono::time_point<std::chrono::system_clock> start);


    pair<int,pair<int,list<int>>> alphaBeta(othelloBoard board, int maxDepth, int depth, unordered_map<int,int> & moveOrder, int alpha, int beta, bool maximizingPlayer, int & nodesVisited, chrono::time_point<std::chrono::system_clock> start);
    pair<int, list<int>> computerMove(othelloBoard board, unordered_map<int, list<int>> validMoves);


    int miniMax(othelloBoard board, int depth, bool maximizingPlayer, int & nodesVisited,chrono::time_point<std::chrono::system_clock> start);
    pair<int, list<int>> computerMove_GPU(othelloBoard board, unordered_map<int, list<int>> validMoves);

    pair<int, list<int>>  interactiveMove(unordered_map<int, list<int>> validMoves);

public:
    float limit = 5;
    int symbol;
    int playerId;
    player(int a, bool b, int c, int d, heuristicEvaluation e);

    pair<int, list<int>> selectMove(othelloBoard board, unordered_map<int, list<int>> validMoves);
};


#endif 