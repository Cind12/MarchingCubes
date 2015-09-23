#ifndef TABLEH   
#define TABLEH
#include "../lib/glm/glm.hpp"
#include <vector>
#include <stdio.h>

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

using namespace std;


typedef vector<bool> mcCase;


class Table {
public:
	Table();
	virtual ~Table();
	vector<vec2> lookup(mcCase index) const;
protected:
	vector<vec2>* table = new vector<vec2>[256];

	unsigned char resolve(bool* key) const;

	int clock(float target);
	int left(float target);
	int up(float target);

	void rotate_clock(vector<vec2>& target, bool* key);
	void rotate_left(vector<vec2>& target, bool* key);
	void rotate_up(vector<vec2>& target, bool* key);
};

#endif
