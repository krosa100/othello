#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.hpp"
#include <limits>
#include "board.hpp"
#include <vector>
#include <time.h>
#include <math.h>

using namespace std;

struct dual_value{
	float vm;
	float vo;
};

/*
struct move_value{
	Move * m;
	float v;
}
*/

class Player {

public:
    Player(Side side);
    ~Player();
    Board* b;

    Move *doMove(Move *opponentsMove, int msLeft);
    int mode;
    

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
private:
	vector<Move*> options(Board * b, Side s);
	float minimax(Board * b, Side s, int levels_left);
	Side s;
	float heur(Board * some_b);
	float heur0(Board * some_b);
	float heur1(Board * some_b);
	Side opp(Side s);
	Move* minimaxmove(int levels_left);
	Move * simplemove(Move *opponentsMove);
	bool corner(int x, int y);
	bool edge(int x, int y);
	dual_value mm2(Board * b, Side s, int l);
	Move* mm2move(int l);
	int moves_left(Board * b);
	float my_heur(Board * b, vector<Move*> op, Side s);
	dual_value dual_heur(Board * b, vector<Move*> op, Side s);
	float opp_heur(Board * b, vector<Move*> op, Side s);
	
	bool finish_up();
	
};

#endif
