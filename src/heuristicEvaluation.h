#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "board.h"
#include "external.h"
#include <unordered_map>
#include <unordered_set>
#include <numeric>

using namespace std;

class heuristicEvaluation {
	private:
		int heuristic5(othelloBoard board, int nSpacesRemaining,int symbol);
		void heuristic5_gpu(othelloBoard board, int nSpacesRemaining,int symbol);
	public:
		int hIndex = 0;
    	int heuristic(othelloBoard board, int nSpacesRemaining, int symbol);
		void heuristic_gpu(othelloBoard board, int nSpacesRemaining, int symbol);    	
	    heuristicEvaluation();
};

/*extern std:: vector<int> mobility_buffer;
extern std:: vector<int> potMobility_buffer;
extern std:: vector<int> makeup_buffer;
extern std:: vector<int> naivety_buffer;
extern std:: vector<int> parity_buffer;
extern std:: vector<int> corners_buffer;
extern std:: vector<int> stability_buffer;*/

#endif