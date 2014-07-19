using namespace std;
#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <stdexcept>
#include <climits>
#include <vector>
#include <queue>
#include <stdlib.h>

#define tr(container, it) for(typeof(container.begin()) it = container.begin(); it != container.end(); it++)
#define all(c) c.begin(), c.end() 
#define present(container, element) (container.find(element) != container.end()) 
#define cpresent(container, element) (find(all(container),element) != container.end()) 
#define sz(a) int((a).size()) 

struct Node {
	// Kept public to allow for O(1) access
	int data;       //The data in this node
	int rank;       // Number of children
	bool isMarked;  // Whether the node is marked 

	Node *l_sib; // The sibling pointers
	Node *r_sib;
	
	Node *parent;   // Parent of the node if any	
	Node *child;    // One of the children 	
		
	//Constructor
	 Node (int val) {
		data 		= val;
		rank 		= 0;
		isMarked 	= false;
		l_sib 		= this;
		r_sib 		= this;
		parent 	= NULL;
		child  	= NULL;
	}
};

class FibHeap {
	//pointer to the node storing min. O(1) lookup.
	private:
		Node * min_;
		unsigned int size_;
		unsigned int num_roots_;
		
	public:
	FibHeap() : min_ (NULL) , size_() {};
	
	//Insert a new node with the given value	
	void insertNode(int val){
		Node * newNode = new Node(val);
		
		//if no nodes created yet
		if (!min_){
			min_ = newNode;
		}
		else {
			
			//Merge the new node with the old root list and update min
			min_ = mergeLists(min_, newNode);	
		}
		
		size_++;
		num_roots_++;
	}

	//Merge two lists pointed to by the input parameters. Returns the smaller node
	Node * mergeLists(Node * a, Node * b){
		//When atleast one of the input lists is null
		if (!a && !b) return NULL;
		else if (!a)  return b;
		else if (!b)  return a;
			
		//General merge
		Node * tmp;
		tmp = a->r_sib;
		
		a->r_sib = b->r_sib;
		b->r_sib = tmp;
		
		a->r_sib->l_sib = a;
		b->r_sib->l_sib = b;
		
		return (a->data < b->data) ? a : b;
	}
	
	int getMin(){
		if (min_) return min_->data;
		else {
			cout << "\nCannot get min from Empty heap ";
			return INT_MIN;
		}
	}
	
	int getSize(){
		return size_;
	}	
	
	//Removes the node n from the doubly linked-list it is a part of.
	//Returns the right sibling of the node.
	Node * removeNode(Node * n){
		Node * ret = n->r_sib;
		
		//If its the last node
		if (ret == n){
			return NULL;
		}
		
		n->r_sib->l_sib = n->l_sib;
		n->l_sib->r_sib = n->r_sib;
		n->r_sib = n;
		n->l_sib = n;
		
		return ret;
	}
	
	//Deletes and returns the minimum value in the heap. 
	int deleteMin(){
		
		if (!min_) {
			cout << "\nCannot delete min from Empty heap \n";
			return INT_MIN;
			//throw invalid_argument("\n Cannot delete from empty heap");
		}
		
		Node * deletedNode = min_; 
		size_--;
		
		//Get a handle to the remaining list;
		min_ = removeNode(min_);
		
		//Fix the parent pointers of the children, if any //TO TEST
		if (deletedNode->child){
			Node * curr = deletedNode->child;
			Node * start = curr;
			do{
				curr->parent = NULL;
				curr = curr->r_sib;
			} while(curr != start);
			
			//Merge children of deleted node with the root list
			min_ = mergeLists(deletedNode->child, min_);
		}
		//If we deleted the last node, the heap is empty
		if (!min_) return deletedNode->data;
		
		//Perform consolidation on the new root list
		consolidateList();
		
		return deletedNode->data;
	}
	
	//Performs consolidation of all the nodes in the root list pointed to by min_
	//Fixes min list with no root of the same rank
		void consolidateList(){
		
		// Start with the bound = log_2[size] possible ranks in the tree
		// Expanded if we encounter higher ranks
		vector<Node *> rank_to_nodes (int(log2(size_)) + 1);
	
		//If singleton tree
		if (min_->r_sib == min_) return;
	
		/* We need to ensure we don't go over the same node twice. 
		* Here we choose to go with a high memory overhead vs. a runtime overhead.
		* We go through and store all the nodes in the root list in a fixed array
		* and iterate over this array.
		*/ 
		
		//Setup list of nodes to visit.
		vector<Node *> nodes_to_visit;
		Node *curr = min_;
		Node *start = curr;
		do{
			nodes_to_visit.push_back(curr);
			curr = curr->r_sib;
		} while(curr != start);
		
		//Go over all the nodes in the nodes_to_visit array
		for (int i = 0; i < nodes_to_visit.size(); i++){
			curr = nodes_to_visit[i];
			
			//Ensure the rank list is big enough
			if (curr->rank >= rank_to_nodes.size()) rank_to_nodes.resize(curr->rank+1);
	
			// If it is the first node of its rank, just add it
			if (!rank_to_nodes[curr->rank]){
				rank_to_nodes[curr->rank] = curr;
				if (curr->data <= min_->data) min_ = curr;
			}
			// Otherwise we merge the two trees. 
			else {
				while (rank_to_nodes[curr->rank]){					
					//Assign the new curr after linking. 
					curr = link(rank_to_nodes[curr->rank], curr);
					//Empty the previous rank
					rank_to_nodes[curr->rank - 1] = NULL;
					//Update min, making sure its a root pointer.
					if(curr->data <= min_->data) min_ = curr;
					//If it needs to be resized.
					if (curr->rank >= rank_to_nodes.size()) rank_to_nodes.resize(curr->rank+1);
				}
				//Add the node to this new rank
				rank_to_nodes[curr->rank] = curr;
			}
		}
	}
	
	
	Node * link(Node *a, Node *b){
		
		if (a == b) cout << "\n Warning: linking node to itself \n";
		
		//Remove the larger node from the root list
		Node *min, *max;
		if (a->data < b->data){
			min = a;
			max = b;
		} else {
			min = b;
			max = a;
		}
		removeNode(max);
		
		//Make the larger node a child of the smaller 		
		min->child = mergeLists(min->child, max);
		max->parent = min;
		
		//Increase the rank of the smaller node
		min->rank++;
		
		//Decrease the number nodes in the root note 
		// ASSUMES LINKING IS ONLY EVER DONE FOR ROOT LIST
		num_roots_--;
		
		return min;
	}
	
	void testLink(){
		min_ = link(min_,min_->r_sib);
	}
	
	// Overloaded << operator for pretty printing the state of the heap
	friend ostream& operator<<(ostream &strm, const FibHeap &f) {
		
		//In case of an empty heap
		if (!f.min_) return strm << "\nNothing to print : Empty Heap "; 

		//Traverse the nodes and print their value
		ostringstream result;
		
		queue<const Node *> currentLevel;
		queue<const Node *> nextLevel;
		const Node *temp;
		const Node *cur_min;
		
		currentLevel.push(f.min_);

		//Print right
		result << "\nSize: "<< f.size_ << "\nRight siblings: ";
		
		while (!currentLevel.empty()){
			cur_min = currentLevel.front();
			currentLevel.pop();
			temp = cur_min;
			if (cur_min){
			do {
				result << temp->data; 
				if (temp->child) {
				result << "(" << temp->child->data << ")";
				nextLevel.push(temp->child);
				}
				result << " -> "	;
				temp = temp->r_sib;
			} while (temp != cur_min);
			result << temp->data;
			result << '\t';
			}
			if (currentLevel.empty()){
				swap(currentLevel, nextLevel);
				result << '\n';
			}
		}

		//Print left
		result << "Left siblings: ";
		temp = f.min_;
		do {
			result << temp->data << " -> "	;
			temp = temp->l_sib;
		} while (temp != f.min_);
		result << temp->data;
		result << '\n';

  		return strm << result.str();
	}
};


void test_sequential_random_Insert_DeleteMin(){
	FibHeap fib;
	priority_queue<int> ref_q;
	int NUM_NODES = 100000;
	
	for (int i = 0; i < NUM_NODES; i++){
		int curr = 	rand();
		fib.insertNode(curr);
		ref_q.push(-curr);			
		}

	for (int i = 0; i < NUM_NODES; i++){
		int val= fib.deleteMin();
		int ref = -ref_q.top();
		ref_q.pop();
	
		if (val != ref){
			cout << "\n Our value: " << val << " Ref Value: " << ref;
			return;
		}
	}
	cout << "\n Sequential Insert then Delete Succeeded";

	for (int i = 0; i < NUM_NODES; i++){
		int curr = 	rand();
		fib.insertNode(curr);
		ref_q.push(-curr);			
		if (i%50 == 0){
		int num_deletes = rand() % fib.getSize();
			for (int j = 0; j < num_deletes; j++){
				int val= fib.deleteMin();
				int ref = -ref_q.top();
				ref_q.pop();
			
				if (val != ref){
					cout << "\n Random : Our value: " << val << " Ref Value: " << ref;
					return;
				}
			}
		}
	}
	cout << "\n Random Insert and Delete Succeeded";

}

int main() {
	test_sequential_random_Insert_DeleteMin();
	
	/*
	FibHeap fib;
	fib.insertNode(5);
	fib.deleteMin();
	cout << fib << "Min : " << fib.getMin() << '\n';
	fib.insertNode(5);
	fib.insertNode(10);
	fib.insertNode(2);
	fib.insertNode(1);
	fib.insertNode(8);
	fib.insertNode(7);
	cout << fib << "Min : " << fib.getMin() << '\n';
	fib.deleteMin();
	cout << fib << "Min : " << fib.getMin() << '\n';
	*/
}
