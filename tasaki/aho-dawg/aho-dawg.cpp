#include <iostream>
#include <fstream>
#include <set>
#include <queue>
#include <string>
#include <vector>
#include <stack>
#include <time.h>

using namespace std;

const int SIGMA_SIZE = 100;
int NODE_NUM_AHO = 0;
int NODE_NUM_DAWG = 0;
int DAWGTOAC_NUM = 0;

struct Trie {
	Trie *edges[SIGMA_SIZE];
	Trie *fail;
	set<string> out;
	int nodenum;
	int ine_num;

	Trie() {
		fail = NULL;
		ine_num = 0;
		nodenum = NODE_NUM_AHO;
		NODE_NUM_AHO++;
		for (int i = 0; i < SIGMA_SIZE; i++) {
			edges[i] = NULL;
		}

	}
};
Trie *troot = new Trie();


struct Node {
	Node *edges[SIGMA_SIZE];
	//edge_num[i]  0 = edge�Ȃ� 1 = primary-edge���� 2 = secondary-edge����
	int edge_num[SIGMA_SIZE];
	Node *suff;
	Trie *dtoc;
	vector<Node*> ine;
	//dawg�ɂ�����m�[�h�̔ԍ�
	int nodenum;
	//AC�̃m�[�h��dawg�̃m�[�h�̔ԍ�����v�����邽�߂̕ϐ�
	int dawgtoac;
	//trunk�m�[�h��branch�m�[�h���̔��� 1�Ȃ�trunk�A0�Ȃ�branch �����l��0
	int torb;


	Node() {
		suff = NULL;
		dtoc = NULL;
		torb = 0;
		dawgtoac = 0;
		nodenum = NODE_NUM_DAWG;
		NODE_NUM_DAWG++;
		for (int i = 0; i < SIGMA_SIZE; i++) {
			edges[i] = NULL;
		}
		for (int i = 0; i < SIGMA_SIZE; i++) {
			edge_num[i] = 0;
		}

	}

};
Node *nroot = new Node();


Node *split(Node *parentnode, Node *childnode, char a) {
	Node *newchildnode = new Node();
	Node *currentnode = parentnode;
	int charnum = a - ' ';

	parentnode->edges[charnum] = newchildnode;
	parentnode->edge_num[charnum] = 1;

	//child��edge�����ׂ�newchild��secondary-edge�ɃR�s�[����
	for (int i = 0; i < SIGMA_SIZE; i++) {
		newchildnode->edges[i] = childnode->edges[i];
		if(childnode->edges[i] != NULL)
			newchildnode->edge_num[i] = 2;
	}


	//parentnode->hassecedgechar[childnode->nodenum] = 0;

	newchildnode->suff = childnode->suff;
	newchildnode->suff->ine.push_back(newchildnode);
	auto itr = find(childnode->suff->ine.begin(), childnode->suff->ine.end(), childnode);
	childnode->suff->ine.erase(itr);
	childnode->suff = newchildnode;
	newchildnode->ine.push_back(childnode);


	int m = 0;
	while (currentnode != nroot) {
		currentnode = currentnode->suff;
		for (m = 0; m < SIGMA_SIZE; m++) {
			if ((currentnode->edges[m] == childnode) && (currentnode->edge_num[m] == 2)) {
				currentnode->edges[m] = newchildnode;
				break;
			}

		}


		if (m == (SIGMA_SIZE - 1))
			break;

	}

	return newchildnode;

}



Node *update(Node *activenode, char a) {


	int charnum = a - ' ';

	if (activenode->edges[charnum] != NULL) {
		//�ӂ�����ꍇ

		if (activenode->edge_num[charnum] == 1) {
			return activenode->edges[charnum];
		}
		else {
			return split(activenode, activenode->edges[charnum], a);
		}

	}
	else {
		//�ӂ��Ȃ��ꍇ
		Node *newactivenode = new Node();
		activenode->edges[charnum] = newactivenode;
		activenode->edge_num[charnum] = 1;

		Node *currentnode = activenode;


		while ((currentnode != nroot) && (newactivenode->suff == NULL)) {
			currentnode = currentnode->suff;

			if ((currentnode->edges[charnum] != NULL) && (currentnode->edge_num[charnum] == 1)) {
				newactivenode->suff = currentnode->edges[charnum];
				newactivenode->suff->ine.push_back(newactivenode);
			}
			else if ((currentnode->edges[charnum] != NULL) && (currentnode->edge_num[charnum] == 2)) {
				newactivenode->suff = split(currentnode, currentnode->edges[charnum], a);
				newactivenode->suff->ine.push_back(newactivenode);
			}
			else {
				currentnode->edges[charnum] = newactivenode;
				currentnode->edge_num[charnum] = 2;
			}
		}

		if (newactivenode->suff == NULL) {
			newactivenode->suff = nroot;
			nroot->ine.push_back(newactivenode);
		}



		return newactivenode;

	}
}


void Goto(Trie *node, string &curString, int depth = 0) {
	if (depth == curString.size()) {
		node->out.insert(curString);

		return;
	}

	int next = curString[depth] - ' ';

	if (node->edges[next] == NULL || node->edges[next] == troot) {
		node->edges[next] = new Trie();
	}

	Goto(node->edges[next], curString, depth + 1);
}

int Goto2(Trie *node, string &curString, int depth = 0, int depth2 = -1) {
	if (depth == curString.size()) {
		node->out.insert(curString);

		return depth2;
	}


	int next = curString[depth] - ' ';

	if (node->edges[next] == NULL || node->edges[next] == troot) {
		node->edges[next] = new Trie();
		if (depth2 == -1)
			depth2 = depth;
	}

	Goto2(node->edges[next], curString, depth + 1, depth2);
}



vector<Trie *> getoutstates(string &string) {
	vector<Trie *> outstates;
	Node *activenode = nroot;
	Node *node;
	int stringsize = string.size();
	stack <Node*> st;

	for (int i = 0; (i < (stringsize)) && (activenode != NULL); i++) {

		activenode = activenode->edges[string[i] - ' '];
		
	}


	if (activenode != NULL) {
		queue <Node*> queue;
		queue.push(activenode);
		while (!queue.empty()) {
			node = queue.front();
			queue.pop();
			if ((node->torb == 1)) {
				outstates.push_back(node->dtoc);
			}

			for (int i = 0; i < node->ine.size(); i++) {
				queue.push(node->ine[i]);
			}

		}
	}

	return (outstates);
}



vector<Trie *> getfailstates(string &string, int depth) {
	vector< Trie * > failstates;
	vector<int> tmp;
	Node *activenode = nroot;
	Node *node;
	Node *enode;
	int num;
	int stringsize = string.size();
	stack<Node *> st;
	stack<int> st_num;


	for (int i = 0; (i <= stringsize) && (activenode != NULL); i++) {
		if ((i >= (depth)) && (activenode != NULL)) {
			st.push(activenode);
			st_num.push(i);
		}
		if (i == stringsize)
			break;

		activenode = activenode->edges[string[i] - ' '];
	}

	queue<Node*> queue;
	vector<Node*> mark;

	while (!st.empty()) {
		node = st.top();
		st.pop();
		num = st_num.top();
		st_num.pop();
		queue.push(node);

		while (!queue.empty()) {
			enode = queue.front();
			queue.pop();

			auto itr = find(mark.begin(), mark.end(), enode);
			if (itr == mark.end()) {
				mark.push_back(enode);
				if ((enode->torb == 1)) {
					enode->dtoc->ine_num = num;
					failstates.push_back(enode->dtoc);
				}
				else {

					for (int i = 0; i < node->ine.size(); i++) {
						queue.push(node->ine[i]);
					}
				}
			}
		}

	}

	return failstates;
}



int main(void) {

	//�L�[���[�h�i�̏W���j�����͂Ƃ��ė^������
	//�L�[���[�h��p���āA�ʏ�̕��@��AC�}�V����DAWG�����


	//
	//AC�}�V���̍\��
	//

	// root�̏�����
	for (int i = 0; i < SIGMA_SIZE; i++) {
		troot->edges[i] = troot;
		troot->fail = troot;
	}

	int out_size = 0;

	//goto�֐��̍\��

	std::ifstream reading_file;

		string ward_num;
		int ward_num_i;
		string ward_string;
		string file_name;

		//file_name = "word_2000.txt";
		//file_name = "test5.txt";
		//file_name = "word_list3.txt";
		file_name = "word_list2.txt";
		//file_name = "aho_check_in.txt";

		string reading_line;

		reading_file.open(file_name, std::ios::in);
		if (reading_file.fail())
		{
			std::cerr << "���s" << std::endl;
			return -1;
		}
		std::getline(reading_file, reading_line);
		ward_num = reading_line;

		ward_num_i = atoi(ward_num.c_str());

		for (int i = 0; i < ward_num_i; i++) {

			std::getline(reading_file, reading_line);

			//cout << reading_line << " " << i << endl;

			Goto(troot, reading_line);
		}
	


	// failure�֐��̍\��
	queue<Trie*> q;

	// Must to this before, because of the fact that every edge out of the root is
	// not NULL
	for (int i = 0; i < SIGMA_SIZE; i++) {
		if ((troot->edges[i] != NULL) && (troot->edges[i] != troot)) {
			troot->edges[i]->fail = troot;
			q.push(troot->edges[i]);
		}
	}

	//Trie *tnode;

	//cout << "check point1" << endl;

	int j = 1;
	while (!q.empty()) {
		Trie *curNode = q.front();
		q.pop();

		for (int i = 0; i < SIGMA_SIZE; i++) {
			Trie *next = curNode->edges[i];
			if (next != NULL && next != troot) {
				q.push(next);

				Trie *f = curNode->fail;
				for (; f->edges[i] == NULL; f = f->fail);

				next->fail = f->edges[i];

				for (auto s : next->fail->out) {
					next->out.insert(s);
				}
			}
		}
	}

	cout << "aho struct finish" << endl;


	//
	//dawg�̍\��
	//

	nroot->dawgtoac = DAWGTOAC_NUM;
	DAWGTOAC_NUM++;
	nroot->torb = 1;

		//string ward_num;
		//int ward_num_i;
		//string ward_string;
		//string file_name;

		Node *activenode;
		activenode = nroot;
		Node *tnode;

		Trie *trienode;
		Trie *tmptrie;

		//cout << "�t�@�C��������͂��Ă�������" << endl;
		//cin >> file_name;

		//file_name = "word_2000.txt";
		//file_name = "test5.txt";
		//file_name = "word_list3.txt";
		file_name = "word_list2.txt";
		//file_name = "aho_check_in.txt";

		std::ifstream reading_file2;
		//string reading_line;

		reading_file2.open(file_name, std::ios::in);
		if (reading_file2.fail())
		{
			std::cerr << "���s" << std::endl;
			return -1;
		}
		std::getline(reading_file2, reading_line);
		ward_num = reading_line;

		ward_num_i = atoi(ward_num.c_str());


		for (int i = 0; i < ward_num_i; i++) {

			std::getline(reading_file2, reading_line);

			//cout << reading_line << " " << i << endl;

			int stringsize = reading_line.size();

			for (int j = 0; j < stringsize; j++)
				activenode = update(activenode, reading_line[j]);


			activenode = nroot;
			trienode = troot;

			activenode = activenode->edges[reading_line[0] - ' '];
			trienode = trienode->edges[reading_line[0] - ' '];


			for (int j = 1; j < stringsize; j++) {
				if (activenode->dawgtoac == 0) {
				//if (activenode->torb == 1) {
					activenode->dawgtoac = DAWGTOAC_NUM;
					activenode->torb = 1;
					DAWGTOAC_NUM++;
					activenode->dtoc = trienode;
				}


				tnode = activenode->edges[reading_line[j] - ' '];
				tmptrie = trienode->edges[reading_line[j] - ' '];
				activenode = tnode;
				trienode = tmptrie;
			}
			if (activenode->dawgtoac == 0) {
				activenode->dawgtoac = DAWGTOAC_NUM;
				activenode->torb = 1;
				DAWGTOAC_NUM++;
				activenode->dtoc = trienode;
			}

			activenode = nroot;
		}
	

	cout << "dawg struct finish" << endl;


	//�i����m�F�j

	// �e�L�X�g����L�[���[�h�����o
	string bigString;
	//std::ifstream ifs("test4.txt");
	std::ifstream ifs("aho_check_out.txt");
	if (ifs.fail())
	{
		std::cerr << "���s" << std::endl;
		return -1;
	}
	getline(ifs, bigString);
	cout << bigString << endl;

	Trie *node = troot;
	int k = bigString.size();
	for (int i = 0; i < k; i++) {
		int cur = bigString[i] - ' ';


		for (; node->edges[cur] == NULL; node = node->fail)
		{
			cout << "failure now... node number " << node->fail->nodenum << endl;
		}


		node = node->edges[cur];

		cout << "current node number " << node->nodenum << endl;

		if (node->out.size() != 0) {
			cout << "At position " << i << " we found:\n";

			for (auto s : node->out) {
				cout << s << "\n";
			}
		}
	}


	//
	//�L�[���[�h��V���ɗ^����i��ꂸ�j
	//DAWG�𓮓I�ɍ\��
	//DAWG�����Ƃ�AC�}�V�����X�V
	//


	//std::ifstream readline2("word_list3.txt");
	//std::ifstream readline2("test3.txt");
	std::ifstream readline2("aho_check_in_plus.txt");

	//dynamic start

	while (1) { 

		int key_num = 0;

		cout << "�L�[���[�h�������ǉ����܂����H" << endl;
		cin >> key_num;

		//std::ifstream readline2("word_list3.txt");
		//std::ifstream readline2("test3.txt");
		//std::ifstream readline2("aho_check_in_plus.txt");

		clock_t start, end, total_start, total_end;
		double time1 = 0, time2 = 0, time3 = 0, time4 = 0, time5 = 0;

		total_start = clock();

		for (int i7 = 0; i7 < key_num; i7++) {


			Trie *node2 = troot;
			Trie *nodepoint2 = troot;
			Trie * tactivenode;
			Trie *newstates[50];
			int depth = 0;

			string curString2;
			int size2 = 0;
			int cur3 = 0;

			Node *activenode = nroot;
			Node *tnode = nroot;


			//�L�[���[�h�̓ǂݍ���

			//getline(readline2, curString2);
			getline(reading_file, curString2);

			//cout << curString2 << " " << i7 << endl;

			size2 = curString2.size();


			//Algorithm1

			tactivenode = troot;
			int charnum = 0;
			int trie_depth = 10000;

			start = clock();

			for (int i = 0; i < size2; i++) {
				newstates[i] = tactivenode;
				charnum = curString2[i] - ' ';


				if ((tactivenode->edges[charnum] != NULL) && (tactivenode->edges[charnum] != troot)) {
					tactivenode = tactivenode->edges[charnum];
				}
				else {

					if (trie_depth == 10000) {
						trie_depth = i + 1;
					}

					Trie *newstate = new Trie();

					tactivenode->edges[charnum] = newstate;

					tactivenode = tactivenode->edges[charnum];

				}


			}
			newstates[size2] = tactivenode;
			tactivenode->out.insert(curString2);

			end = clock();
			time1 += (end - start);

			start = clock();

			vector< Trie* > failstates;
			failstates = getfailstates(curString2, trie_depth);
			int fstatesize = failstates.size();


			for (int i = fstatesize - 1; i >= 0; i--) {
				failstates[i]->fail = newstates[failstates[i]->ine_num];
			}


			end = clock();
			time2 += (end - start);

			tactivenode = troot;
			Trie *failurenode;

			start = clock();

			int charnum2 = 0;

			for (int i = 0; i < size2; i++) {
				charnum2 = curString2[i] - ' ';

				if ((i + 1) >= trie_depth) {
					if (tactivenode == troot) {
						tactivenode->edges[charnum2]->fail = troot;
						tactivenode = tactivenode->edges[charnum2];
					}
					else {
						failurenode = tactivenode->fail;

						int j = 0;
						while ((failurenode->edges[charnum2] == NULL) || (failurenode->edges[charnum2] == tactivenode) || 
							   (failurenode == troot)) {

							if (j >= 2)
								break;

							failurenode = failurenode->fail;
							if (failurenode == troot)
								j++;

						}

						tactivenode = tactivenode->edges[charnum2];

						tactivenode->fail = failurenode->edges[charnum2];

						
						for (auto s : failurenode->edges[charnum2]->out)
							tactivenode->out.insert(s);
						

					}
					
				}
				else {
					tactivenode = tactivenode->edges[curString2[i] - ' '];
				}
			}

			end = clock();
			time3 += (end - start);

			vector<Trie *> outstates;

			start = clock();

			outstates = getoutstates(curString2);

			int outstatesize = outstates.size();

			out_size += outstatesize;

			
			for (int i = 0; i < outstatesize; i++) {
				outstates[i]->out.insert(curString2);
			}
			
			
			end = clock();
			time4 += (end - start);

			Trie *check_trie = troot;

			activenode = nroot;

			start = clock();

			for (int i = 0; i < size2; i++)
				activenode = update(activenode, curString2[i]);


			activenode = nroot;
			Trie *trienode = troot;
			Trie *tmptrie;


			activenode = activenode->edges[curString2[0] - ' '];
			trienode = trienode->edges[curString2[0] - ' '];


			for (int i = 1; i < size2; i++) {
				if (activenode->dawgtoac == 0) {
					activenode->dawgtoac = DAWGTOAC_NUM;
					activenode->torb = 1;
					DAWGTOAC_NUM++;
					activenode->dtoc = trienode;
				}


				tnode = activenode->edges[curString2[i] - ' '];
				tmptrie = trienode->edges[curString2[i] - ' '];
				activenode = tnode;
				trienode = tmptrie;
			}


			if (activenode->dawgtoac == 0) {
				activenode->dawgtoac = DAWGTOAC_NUM;
				activenode->torb = 1;
				DAWGTOAC_NUM++;
				activenode->dtoc = trienode;
			}

			activenode = nroot;

			end = clock();
			time5 += (end - start);


		}

		total_end = clock();
		cout << "total time: " << (double)(total_end - total_start) / CLOCKS_PER_SEC << "sec.\n";
		cout << "time1: " << (double)time1 / CLOCKS_PER_SEC << "sec.\n";
		cout << "time2: " << (double)time2/ CLOCKS_PER_SEC << "sec.\n";
		cout << "time3: " << (double)time3/ CLOCKS_PER_SEC << "sec.\n";
		cout << "time4: " << (double)time4/ CLOCKS_PER_SEC << "sec.\n";
		cout << "time5: " << (double)time5/ CLOCKS_PER_SEC << "sec.\n";

		cout << "out size " << out_size << endl;

		// Read big string, in which we search for elements
		string bigString2;

		getline(ifs, bigString2);
		cout << bigString2 << endl;

		Trie *node3 = troot;
		int k2 = bigString2.size();
		cout << "string size " << k2 << endl;

		for (int i = 0; i < k2; i++) {

			int cur = bigString2[i] - ' ';

			for (; node3->edges[cur] == NULL; node3 = node3->fail)
			{
				cout << "failure now... node number " << node3->fail->nodenum << endl;
			}


			node3 = node3->edges[cur];

			cout << "current node number " << node3->nodenum << endl;


			if (node3->out.size() != 0) {
				cout << "At position " << i << " we found:\n";

				for (auto s : node3->out) {
					cout << s << "\n";
				}
			}

		}



	} 
	//dynamic finish

	return 0;
}