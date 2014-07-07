#include "suffix_tree_edit.h"
#include <iostream>
#include <fstream>
#include <queue>

int SuffixTree::search(str_hmap_list& sub)
{
	Node* node = &root;
	bool in_edge = false; // Are we searching in middle of an edge?
	Edge* edge = NULL;
	int edge_pos = 0, edge_len = 0;

	int result = -1;

	for (unsigned int i=0; i<sub.size(); i++) {
		str_hmap cur = sub[i];

		if (in_edge == false) {
			in_edge = true;
			edge = node->find_edge(cur);	// find an edge and search it

			if (edge == NULL)
				return -1;
			
			// record match pos
			if (i == 0)
				result = edge->begin;

			edge_pos = 0;
			edge_len = edge->length();
		}

		
		if (cur != (*edge)[edge_pos])
			return -1;

		edge_pos++;
		// reached the end of this edge, jump to next node
		if (edge_pos >= edge_len) {
			in_edge = false;
			node = edge->endpoint;
			edge = NULL;
			edge_pos = 0;
		}
	}

	return result;
}


int SuffixTree::construct(void)
{
	// test_str shouldn't have '#' until now
	using std::numeric_limits;
	
	str_hmap end;
	end.insert(make_pair("TheLastElement", "END"));
	test_str.push_back(end);


	while (pos < test_str.size()) {
		ls.clear();
		remainder++;
		//cout << "Char:  "  << test_str[pos] << endl;

		while (remainder) {
			int length = get_active_length();
			if (length == 0)
				active_e = pos;

			Node* node = active_point.active_node;
			str_hmap a_char = get_active_edge();
			Edge* a_edge = node->find_edge(a_char);


			if (a_edge == NULL) {
				Edge* newedge = new Edge(pos, numeric_limits<unsigned int>::max(), test_str);
				node->add_edge(newedge);
				ls.ins_link(node);
			}	
			else {
				if (check_active_node())
					continue;

				str_hmap expected_ele = (*a_edge)[get_active_length()];
				if (expected_ele == get_ele(pos)) {
					inc_active_len();
					ls.ins_link(node);
					break;
				}
				Node *newnode = seperate_edge(node, a_edge);
				Edge* newedge = new Edge(pos, numeric_limits<unsigned int>::max(), test_str);
				newnode->add_edge(newedge);
				ls.ins_link(newnode);
			}
			remainder--;
			if (node == &root && get_active_length() > 0) {
				dec_active_len();
				active_e = pos - remainder + 1;
			}
			else if (node->suffix_link) {
				set_active_node(node->suffix_link);
			}
			else
				set_active_node(&root);
		}

		pos++;
	}
	return 0;
}


void SuffixTree::dfs(void)
{
	dfs_rec(&root, 0);
}

void SuffixTree::dfs_num_print(void)
{
	dfs_num_print_rec(&root, 0);
}

int SuffixTree::dfs_rec(Node* node, unsigned depth)
{
	static int dfs_num = 0;

	node->depth = depth;

	int largest_sub_size = -1;

	bool first = true;
	for (typename map<Edge*, bool>::iterator edge_iter = node->edges.begin(); 
		edge_iter!=node->edges.end(); ++edge_iter) {
		Node *nextnode = edge_iter->first->endpoint;

		dfs_rec(nextnode, depth+edge_iter->first->length());
		
		int nndb = nextnode->dfs_num_begin;
		int nnde = nextnode->dfs_num_end;

		if (first)
			node->dfs_num_begin = nndb;
		node->dfs_num_end = nnde;

		if (node->isleaf() == false && (nnde - nndb > largest_sub_size)) {
			node->largest_sub_begin = nndb;
			node->largest_sub_end = nnde;
			largest_sub_size = nnde - nndb;
		}

		first = false;
			
	}
	if (node->isleaf()) {
		node->dfs_num_begin = dfs_num;
		node->dfs_num_end = dfs_num;
		int leaf_value = test_str.size() - node->depth;
		dfs_to_leaf[dfs_num] = leaf_value;
		leaf_to_dfs[leaf_value] = dfs_num;
		dfs_num++;
		node->largest_sub_begin = -1;
		node->largest_sub_end = -1;
	}
	return 0;
}

int SuffixTree::dfs_num_print_rec(Node* node, int level=0)
{
	cout << "Level " << level << "\t(" << node->dfs_num_begin << ", " << node->dfs_num_end << ")";
	cout << "\t(" << node->largest_sub_begin << ", " << node->largest_sub_end << ")\tDepth " << node->depth << endl;

	for (typename map<Edge*, bool>::iterator edge_iter = node->edges.begin(); 
		edge_iter!=node->edges.end(); ++edge_iter) {
		Node *newnode = edge_iter->first->endpoint;

		dfs_num_print_rec(newnode, level+1);
	}

	if (level == 0) {
		cout << "dfs_to_leaf" << endl;
		for (int i=0; i<dfs_to_leaf.size(); i++)
			cout << dfs_to_leaf[i] << '\t';

		cout << endl;
		cout << "leaf_to_dfs" << endl;
		for (int i=0; i<leaf_to_dfs.size(); i++)
			cout << leaf_to_dfs[i] << '\t';
		cout << endl;
	}


	return 0;
}

int SuffixTree::dfs_num_search(int leaf, int distance)
{
	//int leaf = dfs_to_leaf[dfs_number];
	int search_value = leaf + distance;
	
	if (search_value>=0 && search_value<test_str.size())
		return leaf_to_dfs[search_value];
	else
		return -1;
}

int SuffixTree::label_non_primitive(int leaf, Node* node)
{
	int k = 0;	// possible repetition numbers
	
	int dns;
	// find repetition by searching backward
	do {
		dns = dfs_num_search(leaf, -node->depth);
		k++;
		leaf -= node->depth;
	} while(node->in_range(dns));
	k--;
	leaf += node->depth;

	
	Edge* prev = node->prev;
	int end = prev->end;
	int begin = end - node->depth;
/*
	for (int i=begin+1; i<=end; i++)
		cout << test_str[i] << '\t';
	cout << endl;
*/
	if (end == -1)
		cout << "repetition at leaf, impossible" << endl;

	Edge* middle = NULL;
	// each repetition will end at a node except the last repetition
	for (int i=0; i<k-1; i++)
	{
		int base = begin+1;
		for (int j=begin+1; j<=end; j++) {
		//	cout << j << test_str[j] << endl;
			if (middle == NULL)
				middle = node->find_edge(test_str[j]);
			else {
				if (j-base >= middle->length()) {
					node = middle->endpoint;
					//node->mark = false;
					base = j;
					middle = NULL;
					j--;
				}
				else {
					if (test_str[j] == (*middle)[j-base])
						;
					else
						cout << "Error searching repetition" << endl;
				}
			}
		}
		if (end+1-base != middle->length())
			cout << "Error! Repetition do not end at a node" << endl;
		else {
			node = middle->endpoint;
			node->mark = false;
			middle = NULL;
	/*
	Edge* prev = node->prev;
	int end = prev->end;
	int begin = end - node->depth;

	cout << "__MARKED FALSE____________________" << endl;
	for (int i=begin+1; i<=end; i++)
		cout << test_str[i] << '\t';
	cout << endl;
	*/
		}
	}

	return k;
}

int SuffixTree::left_rotation(int begin_pos, int length)
{
	int end_pos = begin_pos + length - 1;
	int offset = 1;
	
	while (end_pos <= begin_pos) {
		if (test_str[begin_pos-offset] != test_str[end_pos])
			break;
		offset++;
		end_pos--;
	}

	return offset-1; }

void SuffixTree::bfs_push_all_edge_in_q(Node * node, queue<Edge*>& q)
{
	queue<Edge*> temp;

	Edge *edge;
	typename map<Edge*, bool>::iterator e_iter;
	for (e_iter = node->edges.begin();
		e_iter!=node->edges.end(); ++e_iter)
	{
		q.push(e_iter->first);
		temp.push(e_iter->first);
	}
	while (!temp.empty()) {
		edge = temp.front();
		temp.pop();
		node = edge->endpoint;

		for (e_iter = node->edges.begin(); e_iter!=node->edges.end(); ++e_iter) {
			q.push(e_iter->first);
			temp.push(e_iter->first);
		}
	}
}


void SuffixTree::bfs_find_repetition(void)
{
	Node *node = &root;
	queue<Edge*> q;

	repetitions.resize(test_str.size());
	bfs_push_all_edge_in_q(node, q);
	
	node->mark = false;

	Edge* cur_edge;
	Node* cur_node;
	while (!q.empty()) {
		cur_edge = q.front();
		q.pop();
		cur_node = cur_edge->endpoint;

		if (cur_node->mark == false || cur_node->isleaf()) 
			continue;
		int dns, leaf;
	//	for (int i=cur_node->largest_sub_begin; i<=cur_node->largest_sub_end; i++) {
		for (int i=cur_node->dfs_num_begin; i<=cur_node->dfs_num_end; i++) {
			if (i>=cur_node->largest_sub_begin && i<=cur_node->largest_sub_end)
				continue;

			leaf = dfs_to_leaf[i];
			dns = dfs_num_search(leaf, cur_node->depth);
			if (cur_node->in_range(dns)) {
				dns = dfs_num_search(leaf, 2*cur_node->depth);
				if (cur_node->in_range(dns) == false) {
					int r = label_non_primitive(leaf, cur_node);
					// record repetition times and the starting position, +2 is for the right maximal tandem repeats founded before the label_non_primitive function
					int left_maximal = leaf - (r)*cur_node->depth;
					repetitions[left_maximal].insert(make_pair(cur_node->depth, r+2));
					
					// find all non-branching primitive tandem arrays by left-rotating
					int l = left_rotation(left_maximal, cur_node->depth);
					if (r>0 || l>0) {
						for (int j=1; j<=l; j++)
							repetitions[left_maximal-j].insert(make_pair(cur_node->depth, r+2));
						if (r > 0)
							for (int j=l+1; j<=cur_node->depth; j++)
								repetitions[left_maximal+j-1].insert(make_pair(cur_node->depth, r+1));
					}
				}
			}
			dns = dfs_num_search(leaf,-cur_node->depth);
			if (cur_node->in_range(dns)) {
				dns = dfs_num_search(leaf, cur_node->depth);
				if (cur_node->in_range(dns) == false) {
					int r = label_non_primitive(leaf-cur_node->depth, cur_node);		
					int left_maximal = leaf-(r+1)*cur_node->depth;
					repetitions[left_maximal].insert(make_pair(cur_node->depth, r+2));

					int l = left_rotation(left_maximal, cur_node->depth);
					if (r>0 || l>0) {
						for (int j=1; j<=l; j++)
							repetitions[left_maximal-j].insert(make_pair(cur_node->depth, r+2));
						if (r > 0)
							for (int j=l+1; j<=cur_node->depth; j++)
								repetitions[left_maximal+j-1].insert(make_pair(cur_node->depth, r+1));
					}
				}
			}
		}

	}

	/*ofstream fout("markmap");
	for (int i=0; i<test_str.size(); i++) {
		fout << test_str[i];
		for (typename map<int, int>::iterator l_iter = repetitions[i].begin(); l_iter!=repetitions[i].end(); ++l_iter)
			fout << l_iter->first << " ~ " << l_iter->second << "\t";
		fout << endl;
	}*/

}

int SuffixTree::output_single_rep(int begin, int end, ostream& os, vector<map <int, int> >& rep_len, bool output = false)
{
	int compressed_length_rec = 0;
	int compressed_length_rec_2 = 0;
	int compressed_length_me = 0;

	if (begin > end || begin < 0 || end < 0)
		return compressed_length_rec;

	int new_begin = -1, new_end = -1;
	int rep_end = -1;
	int basic_len;
	bool flag = true;
	string in_loop_time = "INLOOP";
	for (int i=end-begin+1; i>0 && flag; i--) {
		for (typename map<int, int>::iterator l_iter = rep_len[i].begin(); l_iter!=rep_len[i].end() && flag; ++l_iter)
		if (l_iter->first >= begin && l_iter->first + i - 1 <=end)
		{
			new_begin = l_iter->first;
			new_end = l_iter->first + l_iter->second - 1;
			rep_end = l_iter->first + i - 1;

			basic_len = l_iter->second;
			compressed_length_me += i - l_iter->second;

			in_loop_time = aux[new_end+1]["tm1"];
			
			if (output == false) {
			// record times of repetition and the basic length of it
			if (test_str[new_begin].find("repetition") == test_str[new_begin].end())
				test_str[new_begin].insert(make_pair("repetition", to_string(i/l_iter->second) + "-"));
			else
				test_str[new_begin]["repetition"] += to_string(i/l_iter->second) + "-";
			if (test_str[new_begin].find("basic_len") == test_str[new_begin].end())
				test_str[new_begin].insert(make_pair("basic_len", to_string(l_iter->second) + "-"));
			else 
				test_str[new_begin]["basic_len"] += to_string(l_iter->second) + "-";
			if (test_str[new_begin].find("in_loop_time") == test_str[new_begin].end())
				test_str[new_begin].insert(make_pair("in_loop_time", in_loop_time + "-"));
			else 
				test_str[new_begin]["in_loop_time"] += in_loop_time + "-";
			}
			flag = false;
		}
	}
	// no repetition in range, output directly
	if (new_begin == -1 &&  new_end == -1) {
		if (output)
		for (int i=begin; i<=end; i++)
			if (i<test_str.size()-1)
			os << test_str[i] << aux[i] << endl;
			else
			os << test_str[i] << endl;
				

		return compressed_length_rec;
	}

	compressed_length_rec_2 += output_single_rep(begin, new_begin-1, os, rep_len, output);	
	compressed_length_rec += output_single_rep(new_begin, new_end, os, rep_len, output);
	compressed_length_rec_2 += output_single_rep(rep_end+1, end, os, rep_len, output);
	
	if (output == false) {
	if (test_str[new_begin].find("actual_len") == test_str[new_begin].end())
		test_str[new_begin].insert(make_pair("actual_len", to_string(basic_len - compressed_length_rec) + "-"));
	else 
		test_str[new_begin]["actual_len"] = to_string(basic_len - compressed_length_rec) + "-" + test_str[new_begin]["actual_len"];
	}
	return compressed_length_rec + compressed_length_rec_2 + compressed_length_me;
}

void SuffixTree::output_compressed_log(ostream& os)
{
	// store the starting position and the basic length of each rep 
	// sorted by the length of repetition
	vector<map<int, int> > rep_len(test_str.size()+1);

	for (int i=0; i<test_str.size(); i++) {
		for (typename map<int, int>::iterator l_iter = repetitions[i].begin(); l_iter!=repetitions[i].end(); ++l_iter)
		rep_len[l_iter->first * l_iter->second].insert(make_pair(i, l_iter->first));
	}
	
	int actual_len = test_str.size()-1;
	output_single_rep(0, actual_len, os, rep_len);
	output_single_rep(0, actual_len, os, rep_len, true);
}

SuffixTree::Node* SuffixTree::seperate_edge(Node * node, Edge* a_edge)
{
	//cout << "seperate the old edge here: " << (*a_edge) << endl;
	unsigned int new_begin = a_edge->begin + get_active_length();
	unsigned int new_end = a_edge->end;

	unsigned int old_begin = a_edge->begin;
	unsigned int old_end = new_begin - 1;

	//cout << (*node);

	node->del_edge(a_edge);
	a_edge->change_edge(new_begin, new_end);
	Edge* old_edge1 = new Edge(old_begin, old_end, test_str);
	node->add_edge(old_edge1);

	old_edge1->endpoint->add_edge(a_edge);

	//cout << "The old edge split as -- " << (*a_edge) << " and -- " << (*old_edge1) << endl;

	//cout << "root " << (&root) << endl;
	//cout << node << endl;
	Node* new_node = old_edge1->endpoint;
	//cout << node << endl;

	return new_node;
}

int SuffixTree::print_tree()
{
	print_node(&root, 1);

	return 0;
}

int SuffixTree::print_node(Node* node, int level)
{
	using namespace std;
	if (level == 0) 
		cout << "Root " << endl;

	if (node->isleaf()) {
		cout << "Leaf reached." << endl;
		return 0;
	}
	
	map<Edge*, bool>::iterator iter;
	for (iter = node->edges.begin(); iter!=node->edges.end(); iter++) {
		int indent = level;
		while (indent--) 
			cout << '\t';
	
		int begin = iter->first->begin, end = (iter->first->end < test_str.size() - 1) ? iter->first->end : test_str.size() - 1;
		cout << "--> (" << begin << ", " << end << ")  ";
		for (int i=begin; i<=end; i++)
			cout << test_str[i] << " ";
		cout << endl;

		cout << "Node Add " << node << endl;
		cout << "Edge printed " << iter->first << endl << endl;
		print_node((iter->first->endpoint), level+1);
	}

	cout << endl;
	return 0;
}

