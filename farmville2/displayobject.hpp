#include <iostream>

class DisplayObject {
public:
	int  width;
	int  height;
	int  layer;
	int  current_x;
	int  current_y;
	std::string image;

	void moveto(int, int);

	DisplayObject(const std::string&, const int);
	DisplayObject(const DisplayObject&);
	~DisplayObject();
	void update_contents(const std::string&);
	void draw(int, int);
	static void redisplay();

private:
	static const int LINELEN = 140;
	static const int NLINES = 60;
	static const int NLAYERS = 4;

	static char theFarm[NLINES][LINELEN][NLAYERS];

	void erase();
	void draw(int, int, const char);
};
