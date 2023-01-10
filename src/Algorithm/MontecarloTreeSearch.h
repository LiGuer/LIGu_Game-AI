#ifndef MONTECARLO_TREE_SEARCH
#define MONTECARLO_TREE_SEARCH

#include <vector>

using namespace std;

/******************************************************************************
*					Monte-Carlo Tree Search 
* 		[1] Select
* 		[2] Expand
* 		[3] Simulate
* 		[4] Backpropage
******************************************************************************/

namespace MonteCarloTreeSearch { 
	/*
	 *  struct of State
	 */
	struct State {
		int action;
	};

	/*
	 *  Node of Monte-Carlo Tree
	 */
	struct Node {
		// evalution of node
		int value = 0, 
			visit = 0;

		State* state  = NULL;
		Node* parent = NULL;

		// actions & next states
		vector<int> actionSet;
		unordered_map<int, Node*> child;

		// Constructor
		Node(State* _state, Node* _parent) {
			state = _state;
			parent = _parent;
			generateActionSet();
		}

		// Destructor, when root is destructed, the entire tree will be destructed automatically 
		~Node() {
			//delete state
			delete state;

			// delete all child node except the best node
			for (auto e = child.begin(); e != child.end(); e++) {
				if (e->second == NULL)
					continue;
				delete e->second;
			}
		}

		// generate action set randomly
		void generateActionSet() {
			{
				// TO generate and push_back ActionSet
			}

			random_device rd;
			mt19937 rng(rd());
			std::shuffle(actionSet.begin(), actionSet.end(), rng);
		}
	};

	/*
	 *  Initial 
	 */
	int isTerminate(State& s) {
		// TO DO;
	}

	State* nextStateRand_Expand(State* s) {
		// TO DO;
	}

	bool nextStateRand_Simulate(State* s) {
		// TO DO;
	}

	/*
	 *  Montecarlo Search Tree
	 */
	inline State* MonteCarloTreeSearch(State* s) {
		Node* root = new Node(s, NULL),
			* current;
		 
		while (true) {
			current = root;

			// [1] Select
			while (current->child.size() == current->actionSet.size()) {
				current = Select(current, true);
			}

			// [2] Expand
			if (isTerminate(*(current->state)) == 0) {
				Node* newNode;

				if (!Expand(current, newNode)) {
					printf("error!\n");
					continue;
				}
				current = newNode;
			}

			// [3] Simulate
			int reward = Simulate(current->state);

			// [4] Backpropagate
			while (current != NULL) {
				current->value += reward;
				current->visit += 1;
				current = current->parent;
			}
		}

		return Select(root, false)->state;
	}

	/*
	 *  [1] Select
	 */
	inline Node* Select(Node* node, bool isExplore) {
		double maxValue = -DBL_MAX;
		Node* bestNode = NULL;

		for (auto e = node->child.begin(); e != node->child.end(); e++) {
			if (e->second == NULL)
				continue;

			// Upper Confidence Bound 
			double
				a = (double)e->second->value / e->second->visit,
				b = (double)log(node->visit) / e->second->visit,
				t = a + (isExplore == true ? sqrt(2) : 0) * sqrt(b);

			maxValue = maxValue < t ? (bestNode = e->second, t) : maxValue;
		}
		return bestNode;
	}

	/* 
	 *  [2] Expand
	 */
	bool Expand(Node* node, Node*& newNode) {
		//New State
		State* s_;

		if ((s_ = nextStateRand_Expand(node->state)) == NULL)
			return false;

		//New Node
		newNode = new Node(s_, node);
		node->child[s_->action] = newNode;

		return true;
	}

	/*
	 *  [3]  Simulate 
	 */
	int Simulate(State* s) {
		State s_ = *s;
		int reward = 0;

		while ((reward = isTerminate(s_)) == 0)
			nextStateRand_Simulate(&s_);

		return reward;
	} 

}
#endif