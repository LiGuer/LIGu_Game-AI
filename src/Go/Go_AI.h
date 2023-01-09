#ifndef GO_AI_H
#define GO_AI_H

#include <random>
#include <unordered_map>
#include <algorithm>
#include "C:/Users/29753/Desktop/Projects/Games/src/Go/Go.h"
#include "C:/Users/29753/Desktop/Projects/Games/src/Algorithm/MontecarloTreeSearch.h"

namespace GoAI { 

	// Node structure for MCTS tree
	struct Node {
		int value = 0,
			visit = 0;

		Go::State* state = NULL;
		Node* parent = NULL;

		vector<int> actionSet;
		unordered_map<int, Node*> child; // map of child nodes, indexed by action

		void generateActionSet() { 
			for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++)
				if (state->mark[i] == -1)
					actionSet.push_back(i);

			std::random_device rd;
			std::mt19937 rng(rd()); 
			std::shuffle(actionSet.begin(), actionSet.end(), rng);
		}  

		~Node() {
			// delete all child node except the best node
			for (auto e = child.begin(); e != child.end(); e++) {
				if (e->second == NULL)
					continue;
				delete e->second;
			}

			//delete state
			delete state;
		}
	};

	Node* Select(Node* node, bool isExplore);
	bool Expand(Node* node, Node*& newNode);
	int Simulate(Go::State* s);
	Go::State* nextStateRand_Expand(Node* node);
	bool nextStateRand_Simulate(Go::State& s);

	inline void move(Node*& root, int a) {
		// move to new root
		Node* t = root;
		root = root->child[a]; 
		root->parent = NULL;

		// delete old root and its all child nodes except the best node
		t->state = NULL;
		t->child[a] = NULL;
		delete t;

		return;
	}

	static int move_pos = -1;
	static int evaluate_fg = 0;
	static vector<double> evaluate_result{ BOARDSIZE * BOARDSIZE };

	// MCTS
	inline void MonteCarloTreeSearch(Go::State* s) {
		Node* root = new Node, 
			* current = root;

		root->state = s;
		root->generateActionSet();

		while (true) {
			if (move_pos != -1) {
				move(root, move_pos);
				move_pos = -1;
			}
			if (evaluate_fg == 1) {
				evaluate_fg = 0;

				std::fill(evaluate_result.begin(), evaluate_result.end(), 0);
				
				for (auto e = root->child.begin(); e != root->child.end(); e++) {
					if (e->second == NULL)
						continue;

					evaluate_result[e->first] = (double)e->second->value / e->second->visit;
				}
			}

			current = root;

			// Select 
			while (current->child.size() == current->actionSet.size()) {
				current = Select(current, true);
			} 

			// Expand the selected node (if it is not a terminal node)
			if (judgeWin(*(current->state)) == 0) {
				Node* newNode;

				if (!Expand(current, newNode)) {
					printf("error!\n");
					continue;
				}
				current = newNode;
			}

			// Simulate
			int reward = Simulate(current->state);

			// Backpropagate
			while (current != NULL) {
				current->value += current->state->player == reward ? 1 : -1;
				current->visit += 1;
				current = current->parent; 
			}
		}
	}

	inline Node* Select(Node* node, bool isExplore) {
		double maxValue = -DBL_MAX;
		Node* node_ = NULL;
		int n = node->child.size();

		for (auto e = node->child.begin(); e != node->child.end(); e++) {
			if (e->second == NULL)
				continue;
			
			// UCB
			double 
				a = (double)e->second->value / e->second->visit,
				b = (double)log(node->visit) / e->second->visit,
				t = a + (isExplore == true ? sqrt(2) : 0) * sqrt(b);

			maxValue = maxValue < t ? (node_ = e->second, t) : maxValue;
		}
		return node_;
	} 

	inline bool Expand(Node* node, Node*& newNode) {
		//New State
		Go::State* s_;

		if ((s_ = nextStateRand_Expand(node)) == NULL)
			return false;

		//New Node
		newNode = new Node;
		newNode->state = s_;
		newNode->parent = node;
		newNode->generateActionSet();
		node->child[s_->action] = newNode; 

		return true;
	}

	inline int Simulate(Go::State* s) {
		Go::State s_ = *s;
		int reward = 0;  

		while ((reward = Go::judgeWin(s_)) == 0)
			nextStateRand_Simulate(s_);

		return reward;
	}

	inline Go::State* nextStateRand_Expand(Node* node) {
		Go::State 
			* s_ = new Go::State(),
			* s  = node->state;
		*s_ = *s;
		s_->parent = s;
		s_->player =-s->player; 

		//[2]
		while (1) { 
			if (node->actionSet.size() == node->child.size()) {
				s_->parent = NULL;
				delete s_;
				return NULL;
			}

			s_->action = node->actionSet[node->child.size()];

			if (Go::downStone(*s_)) break;
			else {
				node->child[s_->action] = NULL;
				s->mark[s_->action] = BANPOINT;
				
				*s_ = *s;
				s_->parent = s;
				s_->player = -s->player;
			} 
		} 
		return s_;
	}

	inline bool nextStateRand_Simulate(Go::State& s) {
		// save old states to help judge Jie
		static int oldSCur = 0;
		static vector<Go::State> oldS(8);

		oldS[oldSCur] = s; 
		oldSCur = (oldSCur + 1) % 8;

		s.player = -s.player;
		s.parent = &oldS[(oldSCur - 1 + 8) % 8];

		// count how many actions we can choose
		int num = 0;
		for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++)
			if (s.mark[i] == -1)
				num++; 

		// randomly choose an action
		while (1) {
			short index = rand() % num + 1;
			
			for (int i = 0; i < BOARDSIZE * BOARDSIZE; i++) {
				if (s.mark[i] == -1)
					index--;

				if (index == 0) {
					s.action = i;
					break;
				}
			}

			if (Go::downStone(s)) { 
				break;
			}
			else {  
				// if this action is invalid
				num--;
				
				oldS[(oldSCur - 1 + 8) % 8].mark[s.action] = BANPOINT;

				s = oldS[(oldSCur - 1 + 8) % 8];
				s.player = -s.player;
				s.parent = &oldS[(oldSCur - 1 + 8) % 8];
			}

			if (num == 0) {
				return false;
			}
		} 

		return true;
	}

}

#endif