#include "table.h"
using namespace std;

Table::Table(){
		//creating edgelists and keys
		std::vector<vec2>* cases = new std::vector<vec2>[14];
		bool** keys = new bool*[14];

		//case 1
		std::vector<vec2> one;
		one.push_back(vec2(0,1));
		one.push_back(vec2(0,4));
		one.push_back(vec2(0,3));
		bool key_one [] = {true, false, false, false, false, false, false, false};
		cases[0] = one;
		keys[0] = key_one;
		

		//case 2
		std::vector<vec2> two;
		two.push_back(vec2(0,4));
		two.push_back(vec2(0,3));
		two.push_back(vec2(1,2));
		two.push_back(vec2(1,2));
		two.push_back(vec2(0,3));
		two.push_back(vec2(1,5));
		bool key_two [] = {true, true, false, false, false, false, false, false};
		cases[1] = two;
		keys[1] = key_two;

		//case 3
		std::vector<vec2> three;
		three.push_back(vec2(0,1));
		three.push_back(vec2(0,4));
		three.push_back(vec2(0,3));
		three.push_back(vec2(2,1));
		three.push_back(vec2(2,6));
		three.push_back(vec2(2,3));
		bool key_three  [] = {true, false, true, false, false, false, false, false};
		cases[2] = three;
		keys[2] = key_three;


		//case 4
		std::vector<vec2> four;
		four.push_back(vec2(0,1));
		four.push_back(vec2(0,4));
		four.push_back(vec2(0,3));
		four.push_back(vec2(6,2));
		four.push_back(vec2(6,5));
		four.push_back(vec2(6,7));
		bool key_four  [] = {true, true, false, false, false, false, true, false};
		cases[3] = four;
		keys[3] = key_four;

		//case 5
		std::vector<vec2> five;
		five.push_back(vec2(1,0));
		five.push_back(vec2(1,2));
		five.push_back(vec2(4,0));
		five.push_back(vec2(4,0));
		five.push_back(vec2(1,2));
		five.push_back(vec2(4,7));
		five.push_back(vec2(4,7));
		five.push_back(vec2(1,2));
		five.push_back(vec2(5,6));
		bool key_five [] = {false, true, false, false, true, true, false, false};
		cases[4] = five;
		keys[4] = key_five;

		//case 6
		std::vector<vec2> six;
		six.push_back(vec2(0,4));
		six.push_back(vec2(0,3));
		six.push_back(vec2(1,2));
		six.push_back(vec2(1,2));
		six.push_back(vec2(0,3));
		six.push_back(vec2(1,5));
		six.push_back(vec2(6,2));
		six.push_back(vec2(6,5));
		six.push_back(vec2(6,7));
		bool key_six [] = {true, true, false, false, false, false, true, false};
		cases[5] = six;
		keys[5] = key_six;


		//case 7
		std::vector<vec2> seven;
		seven.push_back(vec2(1,0));
		seven.push_back(vec2(1,2));
		seven.push_back(vec2(1,5));
		seven.push_back(vec2(3,0));
		seven.push_back(vec2(3,2));
		seven.push_back(vec2(3,7));
		seven.push_back(vec2(6,2));
		seven.push_back(vec2(6,5));
		seven.push_back(vec2(6,7));
		bool key_seven [] = {false, true, false, true, false, false, true, false};
		cases[6] = seven;
		keys[6] = key_seven;
	
		//case 8 
		std::vector<vec2> eight;
		eight.push_back(vec2(1,2));
		eight.push_back(vec2(0,3));
		eight.push_back(vec2(5,6));
		eight.push_back(vec2(0,3));
		eight.push_back(vec2(5,6));
		eight.push_back(vec2(4,7));
		bool key_eight [] = {true, true, false, false, true, true, false, false};
		cases[7] = eight;
		keys[7] = key_eight;

		//case 9 
		std::vector<vec2> nine;
		nine.push_back(vec2(7,3));
		nine.push_back(vec2(0,3));
		nine.push_back(vec2(7,6));
		nine.push_back(vec2(0,3));
		nine.push_back(vec2(7,6));
		nine.push_back(vec2(0,1));
		nine.push_back(vec2(7,6));
		nine.push_back(vec2(0,1));
		nine.push_back(vec2(5,6));
		nine.push_back(vec2(0,1));
		nine.push_back(vec2(5,6));
		nine.push_back(vec2(5,1));
		bool key_nine [] = {true, false, false, false, true, true, false, true};
		cases[8] = nine;
		keys[8] = key_nine;
		
		//case 10
		std::vector<vec2> ten;
		ten.push_back(vec2(3,7));
		ten.push_back(vec2(0,4));
		ten.push_back(vec2(3,2));
		ten.push_back(vec2(0,4));
		ten.push_back(vec2(3,2));
		ten.push_back(vec2(0,1));
		ten.push_back(vec2(6,7));
		ten.push_back(vec2(6,2));
		ten.push_back(vec2(5,4));
		ten.push_back(vec2(6,2));
		ten.push_back(vec2(5,4));
		ten.push_back(vec2(5,1));
		bool key_ten [] = {true, false, false, true, false, true, true, false};
		cases[9] = ten;
		keys[9] = key_ten;

		//case 11
		std::vector<vec2> eleven;
		eleven.push_back(vec2(0,3));
		eleven.push_back(vec2(4,7));
		eleven.push_back(vec2(0,1));
		eleven.push_back(vec2(4,7));
		eleven.push_back(vec2(0,1));
		eleven.push_back(vec2(6,2));
		eleven.push_back(vec2(0,1));
		eleven.push_back(vec2(6,2));
		eleven.push_back(vec2(5,1));
		eleven.push_back(vec2(6,2));
		eleven.push_back(vec2(4,7));
		eleven.push_back(vec2(6,7));
		bool key_eleven [] = {true, false, false, false, true, true, true, false};
		cases[10] = eleven;
		keys[10] = key_eleven;

		//case 12
		std::vector<vec2> twelve;
		twelve.push_back(vec2(3,0));
		twelve.push_back(vec2(3,7));
		twelve.push_back(vec2(3,2));
		twelve.push_back(vec2(1,0));
		twelve.push_back(vec2(1,2));
		twelve.push_back(vec2(4,0));
		twelve.push_back(vec2(4,0));
		twelve.push_back(vec2(1,2));
		twelve.push_back(vec2(4,7));
		twelve.push_back(vec2(1,2));
		twelve.push_back(vec2(4,7));
		twelve.push_back(vec2(5,6));
		bool key_twelve [] = {false, true, false, true, true, true, false, false};
		cases[11] = twelve;
		keys[11] = key_twelve;

		//case 13
		std::vector<vec2> thirteen;
		thirteen.push_back(vec2(0,1));
		thirteen.push_back(vec2(0,3));
		thirteen.push_back(vec2(0,4));
		thirteen.push_back(vec2(5,1));
		thirteen.push_back(vec2(5,6));
		thirteen.push_back(vec2(5,4));
		thirteen.push_back(vec2(7,3));
		thirteen.push_back(vec2(7,6));
		thirteen.push_back(vec2(7,4));
		thirteen.push_back(vec2(2,1));
		thirteen.push_back(vec2(2,3));
		thirteen.push_back(vec2(2,6));
		bool key_thirteen [] = {true, false, true, false, false, true, false, true};
		cases[12] = thirteen;
		keys[12] = key_thirteen;

		//case 14
		std::vector<vec2> fourteen;
		fourteen.push_back(vec2(4,0));
		fourteen.push_back(vec2(1,0));
		fourteen.push_back(vec2(7,3));

		fourteen.push_back(vec2(1,0));
		fourteen.push_back(vec2(7,3));
		fourteen.push_back(vec2(5,6));

		fourteen.push_back(vec2(7,3));
		fourteen.push_back(vec2(5,6));
		fourteen.push_back(vec2(7,6));

		fourteen.push_back(vec2(1,0));
		fourteen.push_back(vec2(5,6));
		fourteen.push_back(vec2(1,2));
		bool key_fourteen [] = {false, true, false, false, true, true, false, true};
		cases[13] = fourteen;
		keys[13] = key_fourteen;
		
		printf("Cases defined \n");

		//rotate all cases and build table
		char w = 'w';
		char c = 'c';
		char l = 'l';
		char u = 'u';
		char directions [] ={w,c,w,c,w,c,w,c,l,w,u,w,u,w,u,w,u,l,w,c,w,c,w,c,w,c,l,w,u,w,u,w,u,w,u,l,u,w,l,w,l,w,l,w,l,u,u,w,l,w,l,w,l,w};

		for (unsigned char i = 0; (i < 14); i++){
			for (unsigned char j = 0; (j < 54); j++){
				char direction = directions[j];
				if (direction == 'w') table[resolve(keys[i])] = cases[i];
				if (direction == 'c') rotate_clock(cases[i], keys[i]);
				if (direction == 'l') rotate_left(cases[i], keys[i]);
				if (direction == 'u') rotate_up(cases[i], keys[i]);
			}
		}
	}
		
	Table::~Table(){
		delete table;
	}

		
	std::vector<vec2> Table::lookup(mcCase index) const {
		//Inverting index if 5 or more Vertices are "inside"
		unsigned char sum = 0;
		for (unsigned char i = 0; (i < 8); i++){
			if (index[i]) sum++;
		}
		if (sum > 4){
			for (unsigned char i = 0; (i < 8); i++){
				index[i] = !(index[i]);
			}
		}	
		//looking up case
		bool array_index[8];
		for (int i = 0; (i < 8); i++){
			array_index[i] = index[i];
		}
		return table[resolve(array_index)];		
	}

	unsigned char Table::resolve(bool* key) const {
		unsigned char result = 0;
		if (key[0]) result++;
		if (key[1]) result += 2;
		if (key[2]) result += 4;
		if (key[3]) result += 8;
		if (key[4]) result += 16;
		if (key[5]) result += 32;
		if (key[6]) result += 64;
		if (key[7]) result += 128;
		//printf("resolved to %i\n", result);
		return result;
	}

	int Table::clock(float target){
		switch ((int)target)
		{
			case 0 : return 3;
			case 1 : return 0;
			case 2 : return 1;
			case 3 : return 2;
			case 4 : return 7;
			case 5 : return 4;
			case 6 : return 5;
			case 7 : return 6;
		}
	}

	int Table::left(float target){
		switch ((int)target)
		{
			case 0 : return 4;
			case 1 : return 0;
			case 2 : return 3;
			case 3 : return 7;
			case 4 : return 5;
			case 5 : return 1;
			case 6 : return 2;
			case 7 : return 6;
		}	
	}

	int Table::up(float target){
		switch ((int)target)
		{
			case 0 : return 3;
			case 1 : return 2;
			case 2 : return 6;
			case 3 : return 7;
			case 4 : return 0;
			case 5 : return 1;
			case 6 : return 5;
			case 7 : return 4;
		}
	}

	void Table::rotate_clock(std::vector<vec2>& target, bool* key){
		bool zero = key[1];
		bool one = key[2];
		bool two = key[3];
		bool three = key[0];
		bool four = key[5];
		bool five = key[6];
		bool six = key[7];
		bool seven = key[4];
		key[0] = zero;
		key[1] = one;
		key[2] = two;
		key[3] = three;
		key[4] = four;
		key[5] = five;
		key[6] = six;
		key[7] = seven;
		for (vec2& current : target){
			current.x = clock(current.x);
			current.y = clock(current.y);
		}
	}
	
	void Table::rotate_left(std::vector<vec2>& target, bool* key){
		for (vec2& current : target){
			current.x = left(current.x);
			current.y = left(current.y);
		}
		bool* tmp_key = new bool[8];
		tmp_key[0] = key[1];
		tmp_key[1] = key[5];
		tmp_key[2] = key[6];
		tmp_key[3] = key[2];
		tmp_key[4] = key[0];
		tmp_key[5] = key[4];
		tmp_key[6] = key[7];
		tmp_key[7] = key[3];
		for (unsigned char i = 0; (i < 8); i++){
			key[i] = tmp_key[i];
		}
		delete tmp_key;
	}

	void Table::rotate_up(std::vector<vec2>& target, bool* key){
		for (vec2& current : target){
			current.x = up(current.x);
			current.y = up(current.y);
		}
		bool* tmp_key = new bool[8];
		tmp_key[0] = key[4];
		tmp_key[1] = key[5];
		tmp_key[2] = key[1];
		tmp_key[3] = key[0];
		tmp_key[4] = key[7];
		tmp_key[5] = key[6];
		tmp_key[6] = key[2];
		tmp_key[7] = key[3];
		for (unsigned char i = 0; (i < 8); i++){
			key[i] = tmp_key[i];
		}
		delete tmp_key;
	}
