using namespace std;
#include <iostream>        // This is a key C++ library
#include <cmath>           // The standard C library math.h
#include <string>
#include <sstream>

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
		
	public:
	
	FibHeap() : min_ (NULL) {};
	
	//Insert a new node with the given value	
	void insertNode(int val){
		Node * newNode = new Node(val);
		
		//if no nodes created yet
		if (!min_){
			min_ = newNode;
		}
		else {
			//Merge the new node with the old root list
			min_ = mergeLists(min_, newNode);	
		}
	}

	Node * mergeLists(Node * a, Node * b){
		//When atleast one of the input lists is null
		if (!a && !b)
			return NULL;
		else if (!a)
			return b;
		else if (!b)
			return a;
			
		//General merge
		Node * tmp;
		tmp = a->r_sib;
		
		a->r_sib = b->r_sib;
		b->r_sib = tmp;
		
		a->r_sib->l_sib = a;
		b->r_sib->l_sib = b;
		
		return a;
	}
	
	friend ostream& operator<<(ostream &strm, const FibHeap &f) {
		
		//In case of an empty heap
		if (!f.min_)
			return strm << "Empty Heap \n"; 

		//Traverse the nodes and print their value
		Node *temp = f.min_;
		ostringstream result;
		
		//Print right
		result << "\nRight siblings: ";
		do {
			result << temp->data << " -> "	;
			temp = temp->r_sib;
		} while (temp != f.min_);
		result << temp->data;
		result << '\n';

		//Print left
		result << "Left siblings: ";
		do {
			result << temp->data << " -> "	;
			temp = temp->l_sib;
		} while (temp != f.min_);
		result << temp->data;
		result << '\n';

  		return strm << result.str();
	}
};

int main() {

	FibHeap fib;
	cout << fib;
	fib.insertNode(5);
	cout << fib;
	fib.insertNode(10);
	cout << fib;
	fib.insertNode(15);
	cout << fib;
	return 0;
}
