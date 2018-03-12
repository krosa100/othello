#include "player.hpp"

const int dim = 8;
const float inf = numeric_limits<float>::infinity();
const float corner_worth = 3;
const float edge_worth = 2;
const float default_worth = 1;  

// NOTE:
// AI is named "aIgo".
// Player variable "mode" determines the functionality of the AI. It is set in the Player constructor.
// 	mode = 0 is a "working AI" "good enough to beat SimplePlayer" the majority of the time.
// 	mode = 1 is an AI with a "minimax algorithm" suitable for testminimax (2-ply)
// 	mode = 2 (or anything else) is my highly tentative "custom" bot
// testminimax sets mode to 1.


/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish
 * within 30 seconds.
 */
Player::Player(Side side) {
    //NOTE: mode is set
    mode = 2; 
    
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;
    s = side;
    
    b = new Board;


    /*
     * TODO: Do any initialization you need to do here (setting up the board,
     * precalculating things, etc.) However, remember that you will only have
     * 30 seconds.
     */
}

/*
 * Destructor for the player.
 */
Player::~Player() {
}

bool Player::corner(int x, int y){
	return (x==0 || x== dim-1) && (y==0 || y== dim-1);
}
bool Player::edge(int x, int y){
	return (x==0 || x== dim-1) || (y==0 || y== dim-1);
}

float Player::heur0(Board * some_b){
	float score = 0;
	for(int x = 0; x < dim; x++){
		for(int y = 0; y < dim; y++){
			float weight = 0;
			if(some_b->get(s,x,y)){
				weight = 1;
			}else if(some_b->get(opp(s),x,y)){
				weight = -1;
			}
			if(weight != 0){
				float v;
				if(corner(x,y)){
					v = corner_worth;
				}else if(edge(x,y)){
					v = edge_worth;
				}else{
					v = default_worth;
				}
				v *= weight;
				score += v;
			}		
		}
	}
	return score;
}

float Player::heur1(Board * some_b){
	return b->count(s) - b->count(opp(s));
}

float Player::heur(Board * some_b){
	switch(mode){
		case 0: 
			return heur0(some_b);
			break;
		case 1: 
			return heur1(some_b); 
			break;
		default: 
			return heur0(some_b);
	}	
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be
 * nullptr.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return nullptr.
 */
Move * Player::doMove(Move *opponentsMove, int msLeft) {
	b->doMove(opponentsMove, opp(s));
	Move* todo;
	switch(mode){
		case 0: 
			todo = simplemove(opponentsMove);
			break;
		case 1: 
			todo = minimaxmove(2);
			break;
		default: 
			todo = mm2move(4);//mm2move(4);
	}
	b->doMove(todo, s);
	return todo;
}

Move * Player::simplemove(Move *opponentsMove){
	Move * best_option;
	float best_h = -100;
	bool move_found =false;
	for(int x = 0; x < dim; x++){
		for(int y = 0; y < dim; y++){
			Move * m = new Move(x, y);
			
			if(b->checkMove(m, s) ){
				Board * cb = b->copy();
				cb->doMove(m, s);
				float h = heur(cb);
				if(h >= best_h){
					best_h = h;
					best_option = m;
					//cerr << best_option->x << "/" << best_option->y << endl;
					move_found = true;
				}
			}
			//delete m;
		}
	}
	
	if(move_found){
		//cerr << b->checkMove(best_option, s) << endl;
		//cerr << best_option->x << "/" << best_option->y << endl;
		return best_option;
	}else{
	cerr << "null" << endl;
		return nullptr;	
	}
}

Side Player::opp(Side s){
	if(s == WHITE){
		return BLACK;
	}else{
		return WHITE;
	}
}

vector<Move*> Player::options(Board * b, Side s){
	vector<Move*> moves;
	for(int x = 0; x < dim; x++){
		for(int y = 0; y < dim; y++){
			Move * m = new Move(x, y);
			if(b->checkMove(m, s)){
				moves.push_back(m);
			}
		}
	}
	return moves;
}

//redundancy of minimax is for optimization
Move* Player::minimaxmove(int levels_left){
	vector<Move*> op = options(b, s);
	float best;
	if(s == this->s){
		best=-inf;
	}else{
		best=inf;
	}
	
	Move* best_move;//dif
	if(op.size() == 0){
		return nullptr;//dif
	}
	for(int i = 0; i < op.size(); i++){
		float value;
		Board * nb = b->copy();
		nb->doMove(op[i], s);
		if(levels_left == 0){
			value = heur(nb);
		}else{
			value = minimax(nb, opp(s), levels_left - 1);
		}
		bool condition;
		if(s == this->s){
			condition = value > best;
		}else{
			condition = value < best;
		}
		if(condition){
			best = value;
			best_move = op[i];//dif
		}	
	}
	return best_move;//dif
}

float Player::minimax(Board * b, Side s, int levels_left){
	vector<Move*> op = options(b, s);
	float best;
	if(s == this->s){
		best=-inf;
	}else{
		best=inf;
	}
	if(op.size() == 0){
		if(levels_left == 0){
			return heur(b);
		}else{
			return minimax(b, opp(s), levels_left - 1);
		}
	}
	for(int i = 0; i < op.size(); i++){
		float value;
		Board * nb = b->copy();
		nb->doMove(op[i], s);
		if(levels_left == 0){
			value = heur(nb);
		}else{
			value = minimax(nb, opp(s), levels_left - 1);
		}
		bool condition;
		if(s == this->s){
			condition = value > best;
		}else{
			condition = value < best;
		}
		if(condition){
			best = value;
		}	
	}
	return best;
}

time_t start_time;
time_t timer;
float move_sec;
const float time_buffer = 0.5;

bool Player::finish_up(){
	return false;
	/*
	time(&timer);
	float time_elapsed = difftime(start_time, timer);
	return time_elapsed > (move_sec/2 - time_buffer) */
	
}

int Player::moves_left(Board * b){
	return dim * dim - (b->countWhite() + b->countBlack());
}

/*
Move* Player mm2m(float time){
	int min = 3;
	int max = 5;
	move_sec = time;
	time(&start_time);
	time_alloted = move_sec/(ceil(((float)moves_left())/2);
	
	move_value best;
	best.v = -inf;
	
	for(int i = min; i <= max; i++)
		move_value mv = mm2move(min);
		if(mv.v > best.v){
			best = mv;
		}
		if(finish_up()) return best.m;
	}
}
*/

Move* Player::mm2move(int l){
	Move* best;
	float v = -inf;
	vector<Move*> op = options(b, s);

	Move* best_move;
	if(op.size() == 0){
		return nullptr;
	}
	for(int i = 0; i < op.size(); i++){
		float value;
		Board * nb = b->copy();
		nb->doMove(op[i], s);
		if(l==0){
			value = dual_heur(nb, op, s).vm;
		}else{
			value = mm2(nb, opp(s), l - 1).vm;
		}
		bool condition;
		
		condition = value > v;
		
		if(condition){
			v = value;
			best = op[i];
		}	
	}
	return best;
}

dual_value Player::mm2(Board * b, Side s, int l){
	vector <Move*> op = options(b, s);
	dual_value best;
	best.vm = -inf;
	best.vo = -inf;
	if(op.size() == 0){
		if(l == 0 || finish_up()){
			return dual_heur(b, op, s);
		}else{
			return mm2(b, opp(s), l - 1);
		}
	}
	for(int i = 0; i < op.size(); i++){

		dual_value value;
		Board * nb = b->copy();
		nb->doMove(op[i], s);
		if(l == 0 || finish_up()){
			value = dual_heur(nb, op, s);
		}else{
			value = mm2(nb, opp(s), l - 1);
		}
		bool condition;
		if(s == this->s){
			condition = value.vm > best.vm;
		}else{
			condition = value.vo > best.vo;
		}
		if(condition){
			best = value;
		}	
	}
	return best;
}

dual_value Player::dual_heur(Board * b, vector<Move*>op, Side s){
	dual_value dv;
	dv.vm = my_heur(b, op, s);
	dv.vo = opp_heur(b, op, s);
	return dv;
}


float const long_weight = 2;
float const mul = 2;
float Player::my_heur(Board * b, vector<Move*> op, Side s){
	
	vector<Move*> opp_op = options(b, opp(s));
	float v = (1+(log((double)op.size() + 1))/(1+log((double)opp_op.size() + 1)));
	if(s == this->s){
		//v *= 1;
	}else{
		
		v*= -1;
	}
	float a = heur0(b);
	//float d = heur1(b);
	std::cerr << long_weight * a << "/" << v/fabs(a) << std::endl; 
	
	return  long_weight * a + v/fabs(a);
	
	//return heur0(b);
}

float Player::opp_heur(Board * b, vector<Move*> op, Side s){
	return heur0(b);
}



    /*
     * TODO: Implement how moves your AI should play here. You should first
     * process the opponent's opponents move before calculating your own move
     */
