#ifndef _SUFFIX_TREE_
#define _SUFFIX_TREE_
#include <string>
//#include <tr1/unordered_map>
#include <limits>
#include <map>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <queue>
#include "datatype.h"

using std::vector;
using std::string;
using std::map;
using std::make_pair;
using std::cout;
using std::endl;
using std::out_of_range;
using std::ostream;
//typedef tr1::unordered_map map;

// TODO: upgrade it to process trace. Rule: char-->elem  string-->elem_list
class SuffixTree
{
public:
	// active point is initialized as (root, None, 0), remainder initialized as 1
	SuffixTree(str_hmap_list& str, str_hmap_list& tm):
		test_str(str), aux(tm), root(test_str, NULL), active_point(&root, 0),
		remainder(0), pos(0), active_e(0), ls(), dfs_to_leaf(str.size()),
		leaf_to_dfs(str.size()) {}
	int construct(void);

	// return -1 if no such sub exist, return the beginning postion of this substring in thr original string if it exist
	int search(str_hmap_list& sub);

	void dfs(void);
	void dfs_num_print(void);
	void output_compressed_log(ostream& os);

	// return the length of the longest prefix of sub which can be matched in suffix tree
	template <class Iterator>
	Iterator inc_search(Iterator sub, int *final_pos)
	{
	//	typedef typename Iterator::value_type T;	// extract real type

		Iterator result = sub;
		Node* node = &root;
		Edge* edge = NULL;
		int pos = 0;	// the iter's pos at edge
		int edge_len = -1;
		bool flag = true;
		*final_pos = -1;


		while (flag) {
			if (edge == NULL) {
				edge = node->find_edge(*result);	
				if (edge == NULL) {
					flag = false;
				}
				else {
					result++;
					pos = 1; // the second element of the edge
					edge_len = edge->length();
				}
			}
			else {
				if (pos >= edge_len) {
					node = edge->endpoint;
					*final_pos = edge->end;
					edge = NULL;
					edge_len = 0;
					pos = 0;

					if (node == NULL || node->isleaf())
						flag = false;
				}
				else {
					if (*result == (*edge)[pos]) {
						result++;
						pos++;
					}
					else {
						flag = false;
						*final_pos = edge->end - edge->length() + pos;	
					}
				}
			}
		}
		
		return result;
	}

	void bfs_find_repetition(void);
	int print_tree(void);
private:
	str_hmap_list& test_str;
	str_hmap_list& aux;

	vector<map<int, int> > repetitions;
	struct Node;

	int output_single_rep(int begin, int end, ostream& os, vector<map <int, int> >& rep_len, bool output);
	int label_non_primitive(int leaf, Node* node);
	int left_rotation(int begin_pos, int length);

	struct Edge{
		// the begin and end pos of this edge, note that INT_MAX stands for #(the changing end pos of this entire string)
		unsigned int begin, end;
		// Is there a better way to find test_str?
		str_hmap_list& test_node_str;

		Node * endpoint;

		Edge(unsigned int b, unsigned int e, str_hmap_list& str):
			test_node_str(str) 
		{
			begin = b;
			end = e;
			endpoint = NULL;
			//std::cout << "Edge initialized" << std::endl;
		}

		void change_edge(unsigned int b, unsigned int e)
		{
			begin = b;
			end = e;
		}

		int length(void) 
		{

			if (end > test_node_str.size())
				return test_node_str.size() - begin;
			else
				return end - begin + 1; 
		}
		
		// needed by map
		friend bool operator<(const Edge& me, const Edge& other)
		{
			return me.begin < other.begin;
		}

		str_hmap operator[](unsigned int i)
		{
			i += begin;
			if (i > end)
				throw out_of_range("Edge [] out of range.");

			return test_node_str[i];
		}

		friend ostream& operator<<(ostream& os, Edge& edge)
		{
			unsigned int end = edge.test_node_str.size()-1;
			if (end >= edge.end)
				end = edge.end;

			os << "Begin " << edge.begin << "End " << edge.end;
			if (end != edge.end)
				os << '#';

			return os;
		}

		bool is_none(void) { return begin == 0 && end == 0; }
	};

	struct Node{
		typedef struct Edge Edge; // the entry accessable type for Node

		int depth;
		int dfs_num_begin, dfs_num_end;
		int largest_sub_begin, largest_sub_end;


		str_hmap_list& test_node_str;
		map<int, int> testmap;
		map<Edge*, bool> edges;
		// find the edge quicky by storing the leading char of this edge
		map<str_hmap, Edge*> findedges;
		Node* suffix_link;
		bool mark;
		Edge* prev;

		friend class LinkState;

		Node(str_hmap_list& str, Edge* prev_edge) : 
			test_node_str(str), suffix_link(NULL), mark(true), prev(prev_edge) { edges.clear(); findedges.clear(); }

		bool in_range(int i) { return i>=dfs_num_begin && i<=dfs_num_end; }

		void add_edge(Edge* edge) { 
			if (edge->endpoint == NULL)
				edge->endpoint = new Node(test_node_str, edge);
			make_pair(edge, true);
			edges.insert(make_pair(edge, true)); 
			findedges.insert(make_pair(test_node_str[edge->begin], edge));
			//cout << "edge added. Now we have " << edges.size() << "edges." << endl;
		}

		void del_edge(Edge* edge) {
			map<Edge*, bool>::iterator iter = edges.find(edge);

			if (iter == edges.end())
				throw out_of_range("edge don't exit");
			else {
				// note we should erase the findedges too
				edges.erase(edge);
				//cout << "delete" << (*edge)[0] << endl;
				findedges.erase((*edge)[0]);
				//cout << "edge deleted. Now we have " << edges.size() << "edges." << endl;
			}

		}

		// find edge by the first char
		Edge* find_edge(str_hmap c)
		{
			//cout << "finding edge char " << c;
			map<str_hmap, Edge*>::iterator iter = findedges.find(c);
			//cout << " founded? ";

			if (iter != findedges.end()) {
				//cout << "yes." << endl;
				return iter->second;
			}
			else {
				//cout << "no." << endl;
				return NULL;
			}
		}

		bool isleaf() { return edges.empty(); }

		bool operator==(Node& other)
		{
			return (this) == (&other);
		}

		friend ostream& operator<<(ostream& os, Node& node)
		{
			map<Node::Edge*, bool>::iterator iter;
			map<str_hmap, Edge*>::iterator iter_f;

			for (iter=node.edges.begin(); iter!=node.edges.end(); ++iter)
				os << iter->first << '\t';
			os << endl;

			for (iter_f=node.findedges.begin(); iter_f!=node.findedges.end(); ++iter_f)
				os << iter_f->first << "-->" << iter_f->second << endl;

			return os;
		}

	};

	class ActivePoint{
	public:
		Node* active_node;
		int active_length;

		ActivePoint(Node* node, int length): 
			active_node(node), active_length(length) { std::cout << "ActivePoint initialized" << std::endl; }
	};

	Node root;
	ActivePoint active_point;

	Node* get_active_node(void) { return active_point.active_node; }
	void set_active_node(Node* node) { active_point.active_node = node; 
	//cout << "Active node set as " << node << endl; 
	}
	str_hmap get_active_edge(void) 
	{ 
		return test_str[active_e]; 
	}

	int get_active_length(void) { return active_point.active_length; }
	void set_active_length(int len) { active_point.active_length = len; }
	void inc_active_len() { active_point.active_length++; }
	void dec_active_len() { active_point.active_length--; }
	
	// how many suffixes is to be inserted?
	int remainder;
	// how many characters inserted?
	unsigned int pos;
	unsigned int active_e;	// the beginnig position of suffixes need to be inserted
	str_hmap get_ele(int i) { return test_str[i]; }
	// insert a char from pos to suffix tree
	int insert();
	int insert_rule1();
	int insert_rule3();
	int print_node(Node* node, int level);


	Node* seperate_edge(Node * node, Edge* edge);

	// check if we can change active node
	bool check_active_node(void)
	{
		Node* node = get_active_node();
		str_hmap a_char = get_active_edge();
		Edge* edge = node->find_edge(a_char);

		if (edge == NULL)
			return false;

		unsigned int edge_size = edge->end - edge->begin + 1;
		unsigned int length = get_active_length();
		
		// update
		if (length >= edge_size) {
			set_active_node(edge->endpoint);
			set_active_length(length-edge_size);
			active_e += edge_size;
			
			return true;
		}
		return false;
	}

	// this class indicate when shall we insert a suffix link
	// ls should be a singleton
	class LinkState
	{
		bool first;
		
		Node* prev, *curr;

	public:
		LinkState() : first(true), prev(NULL), curr(NULL) {}

		void ins_link(Node* node)
		{
			prev = curr;
			curr = node;
			
			if (first == false) {
				prev->suffix_link = curr;
				//cout << "Suffix link added from prev " << prev << " to curr " << curr << endl;
			}

			first = false;
		}

		void clear(void)
		{
			first = true;
			prev = curr = NULL;
		}
	};
	LinkState ls;

	vector<int> dfs_to_leaf;
	vector<int> leaf_to_dfs;

	// set the depth and dfs_num in node
	int dfs_rec(Node* node, unsigned int depth);
	// print values set above
	int dfs_num_print_rec(Node* node, int level);
	
	int dfs_num_search(int dfs_number, int distance);
	
	void bfs_push_all_edge_in_q(Node * node, queue<Edge*>& q);

};
#endif
