#include <atomic>
#include <iostream>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unistd.h>
#include "displayobject.hpp"
//
//  "Background" objects are in layer 0: nest, bakery, road
//  Layer 1 objects:  farmer, child
//  Layer 2 objects:  chicken, bags of flour, sugar, box of butter, crate of eggs, cupcakes, cake batter
//  Layer 3 objects:  cow, truck, stuff in the mixer, barn
//

// First chicken nest
DisplayObject nest1[4] = {DisplayObject("\
\\       /#\
 -------", 0),

DisplayObject("\
\\   O   /#\
 -------", 0),

DisplayObject("\
\\  OO   /#\
 -------", 0),

DisplayObject("\
\\  OOO  /#\
 -------", 0),
};

// second chicken next
DisplayObject nest2[4] = {DisplayObject("\
\\       /#\
 -------", 0),

DisplayObject("\
\\   O   /#\
 -------", 0),

DisplayObject("\
\\  OO   /#\
 -------", 0),

DisplayObject("\
\\  OOO  /#\
 -------", 0),
};

// third chicken next
DisplayObject nest3[4] = {DisplayObject("\
\\       /#\
 -------", 0),

DisplayObject("\
\\   O   /#\
 -------", 0),

DisplayObject("\
\\  OO   /#\
 -------", 0),

DisplayObject("\
\\  OOO  /#\
 -------", 0),
};

// first chicken
DisplayObject chicken1("\
     O>#\
   ^( )#\
    = =", 2);

// second chicken
DisplayObject chicken2("\
     O>#\
   ^( )#\
    = =", 2);

DisplayObject cow("\
   ____ \"#\
 /(____)U#\
  !! !!", 3);

DisplayObject farmer("\
   _#\
  O#\
/(~)\\#\
 ! !", 1);

DisplayObject child("\
  *#\
 /()\\#\
  !!", 1);

// barn where butter and eggs are stored
DisplayObject egg_barn("\
Butter/Egg#\
   __ ^#\
  /  /  \\#\
 |  | _  |#\
 |  |[ ] |", 3);

// barn where flour and sugar are stored
DisplayObject flour_barn("\
Flour/Sugar#\
   __ ^#\
  /  /  \\#\
 |  | _  |#\
 |  |[ ] |", 3);

DisplayObject bakery("\
-----------------------------------|#\
|                                  |#\
                                   |#\
                                   |#\
                                   -------------#\
 =================                             |#\
|o   o   o   o   o                             |#\
                      _________                |#\
                       ^^^^^^^      #\
 =================                  #\
|o   o   o   o   o    |_______|    -------------#\
                        oven       |#\
                                   |#\
 =================                 |#\
|o   o   o   o   o                 |#\
                                   |#\
                                   |#\
 =================  (      )       |#\
|o   o   o   o   o   (____)        |#\
|                     mixer        |#\
|                                  |#\
|                                  |#\
-----------------------------------|#\
      Anne's Patisserie", 0);

DisplayObject intersection("\
---------------#\
|      |      |#\
|------|------|#\
|      |      |#\
---------------", 0);

// truck that transports eggs and butter                                       
DisplayObject egg_truck("\
   _______#\
  |  E/B  |__#\
  -OO----OO-O|", 3);

// truck that transports flour and sugar
DisplayObject flour_truck("\
   _______#\
  |  F/S  |__#\
  -OO----OO-O|", 3);

DisplayObject flour1("\
\\~~~~/#\
(flour)", 2);
DisplayObject flour2("\
\\~~~~/#\
(flour)", 2);

DisplayObject sugar1("\
\\~~~~/#\
(sugar)", 2);
DisplayObject sugar2("\
\\~~~~/#\
(sugar)", 2);

DisplayObject butter1("[butter]", 2);
DisplayObject butter2("[butter]", 2);

DisplayObject eggs1("[OOO]", 2);
DisplayObject eggs2("[OOO]", 2);

DisplayObject batter("[ccc]", 2);

DisplayObject cupcakes[7] = {
DisplayObject(\
"   ________#\
   /       /#\
  /       /#\
  _______", 2),
DisplayObject(\
"   ________#\
   /       /#\
  /     c /#\
  _______", 2),
DisplayObject(\
"   ________#\
   /       /#\
  /   c c /#\
  _______", 2),
DisplayObject(\
"   ________#\
   /       /#\
  / c c c /#\
  _______", 2),
DisplayObject(\
"   ________#\
   /     c /#\
  / c c c /#\
  _______", 2),
DisplayObject(\
"   ________#\
   /   c c /#\
  / c c c /#\
  _______", 2),
DisplayObject(\
"   ________#\
   / c c c /#\
  / c c c /#\
  _______", 2)
};

// Currently these statistics will all be 0.  But your code should increment eggs_laid for each egg a chicken lays,
// and eggs_used for each egg mixed into batter.  Similarly, increment butter_produced when a farmer sells a box of
// bars of butter, and so forth.  Because we know the cake batter recipe (see hw3 handout), we should be able to 
// track that all products are accounted for: They are either visible on the screen, or became "sold cakes"

int eggs_laid, eggs_used;
int butter_produced, butter_used;
int sugar_produced, sugar_used;
int flour_produced, flour_used;
int cakes_produced, cakes_sold;

int wait = 1000000;
int delay = 200;

// Position of bakery, so that we can keep track of where to put items inside the bakery
int bakery_y = 35;
int bakery_x = 75;
int flourbarn_y = 1;
int flourbarn_x = 25;
int eggbarn_y = 10;
int eggbarn_x = 1;
int int_ymin = eggbarn_y+26;
int int_ymax = eggbarn_y+26+4;
int int_xmin = eggbarn_x+36;
int int_xmax = eggbarn_x+36+15;

std::condition_variable cv; // condition variable controlling drawing and redisplaying
std::mutex cv_mutex;
std::atomic<bool> go(false); // set to 1 while we are redisplaying

std::condition_variable int_cv; // controls access to truck intersection
std::mutex int_mutex;
std::atomic<bool> int_full(false); // set to true if a truck is in the intersection

void redisplay()
{
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay*2));
		{
			std::lock_guard<std::mutex> lock(cv_mutex);
			DisplayObject::redisplay();
			std::cout << "Eggs: Laid=" << eggs_laid << ", Used=" << eggs_used << 
							" Butter: Sold=" << butter_produced << ", Used=" << butter_used <<
							" Sugar: Sold=" << sugar_produced << ", Used=" << sugar_used <<
							" Flour: Sold=" << flour_produced << ", Used=" << flour_used << 
							" Cakes: Baked=" << cakes_produced << ", Sold=" << cakes_sold << std::endl;
			go = true;
		}
		cv.notify_all();
	}
}


// int count = 0;
std::atomic<int> count(0);

// moves a chicken around randomly
void move_chicken(DisplayObject chicken, int num, int ymin, int ymax, int xmin, int xmax)
{
	int y = 10*num, oldy = 10*num, x = 10*num, oldx = 10*num;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;
		y = std::min(std::max(ymin, y + std::rand() % 3 - 1), ymax);
		x = std::min(std::max(xmin, y + std::rand() % 5 - 2), xmax);
		chicken.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves a product along the conveyor belt
void move_conveyor(DisplayObject product, int y0, int x0, int dist, int time_to_wait)
{
	int y = y0, oldy = y0, x = x0, oldx = x0;
	int final = x0 + dist;
	int counter = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;
		counter++;
		if (counter >= time_to_wait) {
			if (x < final)
				x = x + 1;
			product.draw(oldy = y, oldx = x);
		}
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves the farmer around randomly
void move_farmer(DisplayObject farmer, int ymin, int ymax, int xmin, int xmax)
{
	int y = 19, oldy = 19, x = 5, oldx = 5;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;
		y = std::min(std::max(ymin, y + std::rand() % 3 - 1), ymax);
		x = std::min(std::max(xmin, y + std::rand() % 7 - 3), xmax);
		farmer.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves a truck along a vertical road (from ymin to ymax), should start from bottom of path, moving upwards
void move_truckv(DisplayObject truck, int ymin, int ymax, int xmin, int xmax)
{
	int y = ymax, oldy = ymax, x = xmin, oldx = xmin;
	bool up = true; // true if truck is currently moving "up" (with respect to the view of the screen)
	bool in_int = false; // true if this truck is in the intersection
	
	while (true) {
		// std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;

		// check if we are about to enter the intersection from either direction
		if ((!up && y+3 == int_ymin) || (up && y == int_ymax)) {
			if (!int_full) {
				int_full = true;
				in_int = true;
			}
			// set in_int to false?			
		}

		// only update location if the intersection is unoccupied, or occupied by this truck
		if (!int_full || (int_full && in_int)) {
			if (y <= ymin)
				up = false;
			else if (y >= ymax)
				up = true;
			if (up)
				y = y - 1;
			else
				y = y + 1;
		}

		// check if we have left intersection so other truck can enter
		if ((!up && y == int_ymax) || (up && y+3 == int_ymin)) {
			int_full = false;
			in_int = false;
		}

		std::unique_lock<std::mutex> lock(cv_mutex);
		// checking if this truck is trying to enter the intersection
		if (up && y == int_ymax) {
			std::cout << "flour truck entering from bottom" << std::endl;
		}
		if (!up && y+3 == int_ymin) {
			std::cout << "flour truck entering from top" << std::endl;
		}
		truck.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves a truck along a horizontal road (from xmin to xmax), should start from left of path, moving to the right
void move_truckh(DisplayObject truck, int ymin, int ymax, int xmin, int xmax)
{
	int y = ymax, oldy = ymax, x = xmin, oldx = xmin;
	bool right = true; // true if truck is currently moving "right"
	bool in_int = false;
	
	while (true) {
		// std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;

		if ((right && x+12 == int_xmin) || (!right && x == int_xmax)) {
			if (!int_full) {
				int_full = true;
				in_int = true;
			}
		}

		if (!int_full || (int_full && in_int)) {
			if (x <= xmin)
				right = true;
			else if (x >= xmax)
				right = false;
			if (right)
				x = x + 1;
			else
				x = x - 1;	
		}

		if ((right && x == int_xmax) || (!right && x+12 == int_xmin)) {
			int_full = false;
			in_int = false;
		}
	
		std::unique_lock<std::mutex> lock(cv_mutex);
		// checking if this truck is trying to enter the intersection
		if (right && x+12 == int_xmin) {
			std::cout << "egg truck entering from left" << std::endl;
		}
		if (!right && x == int_xmax) {
			std::cout << "egg truck entering from right" << std::endl;
		}	
		
		truck.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// adds an egg to the nest each iteration
void fill_nest(DisplayObject nest[4], int y, int x, int num) {
	int n = num, l = 0;
	while (true) {
		if (n % 2 == 0)
			l++;
		std::unique_lock<std::mutex> lock(cv_mutex);
		nest[l % 4].draw(y, x);
		n++;
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

void add_cakes(DisplayObject cakes[7], int y, int x) {
	int k = 0, c = 0;
	while (true) {
		if (k % 2 == 0)
			c++;
		std::unique_lock<std::mutex> lock(cv_mutex);
		cakes[c % 7].draw(y, x);
		k++;
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// adds products to the mixer
void fill_mixer(int y, int x) {
	std::string mixer_string;
	DisplayObject mixer_contents(mixer_string, 3);
	mixer_string = "";
	int mix_count = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;
		if (mix_count <= 1)
			mixer_string = "";
		else if (mix_count == 7)
			mixer_string += "E   ";
		else if (mix_count == 10)
			mixer_string += "B";
		else if (mix_count == 19)
			mixer_string += "# F";
		else if (mix_count == 27)
			mixer_string += "S";
		mixer_contents.update_contents(mixer_string);
		mixer_contents.draw(y, x);
		if (mix_count <= 30)	
			mix_count++;
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

void draw_batter(int y, int x) {
	int stop_for = 28;
	int batter_count = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;
		if (batter_count >= stop_for)
			batter.draw(y, x);
		else
			batter_count++;
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

void move_children(DisplayObject child, int y0, int x0)
{
	// each child will move to their destination and then back to their (x0,y0) at the same time-
	// have to add synchronization later
	int dest_y = bakery_y+6;
	int dest_x = bakery_x+44;
	child.draw(y0, x0);
	bool to_bakery = true; // set to true while we are walking towards the bakery, false o.w.
	int y = y0;
	int x = x0;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;
		if (to_bakery) {
			if (x > dest_x)
				x--;
			else
				x = dest_x;
			if (y > dest_y)
				y--;
			else if (y < dest_y)
				y++;
			else
				y = dest_y;
			if (y == dest_y && x == dest_x)
				to_bakery = false;			
		}
		else {
			if (x < x0)
				x++;
			else
				x = x0;
			if (y > y0)
				y--;
			else if (y < y0)
				y++;
			else
				y = dest_y;
			if (y == y0 && x == x0)
				to_bakery = true;			
		}
		child.draw(y, x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// Below is a totally fake main method that simply puts some of the icons on the screen to illustrate various options
// It moves the chicken around randomly (if it goes "into" the barn, it vanishes because the barn is in a higher layer).
// It also illustrates how you can generate a string and turn it into a display object that will vanish after a little while
// when that object goes out of scope and the deconstructor executes.

int main(int argc, char** argv)
{	
	srand(time(0));
	egg_barn.draw(eggbarn_y, eggbarn_x);
	flour_barn.draw(flourbarn_y, flourbarn_x);
	bakery.draw(bakery_y, bakery_x);
	cow.draw(eggbarn_y+23, eggbarn_x);
	intersection.draw(eggbarn_y+26, eggbarn_x+36);
	
	// TODO: add PATH data structure, use relative locations rather than absolute locations
	std::thread rd(redisplay);
	std::thread c1(move_chicken, chicken1, 1, eggbarn_y+6, eggbarn_y+12, eggbarn_x, eggbarn_x+30);
	std::thread c2(move_chicken, chicken2, 2, eggbarn_y+6, eggbarn_y+12, eggbarn_x+15, eggbarn_x+30);
	std::thread tf(move_truckv, flour_truck, flourbarn_y, bakery_y+10, flourbarn_x+12, flourbarn_x+13);
	std::thread te(move_truckh, egg_truck, eggbarn_y+26, eggbarn_y+27, eggbarn_x, bakery_x-15);
	std::thread n1(fill_nest, nest1, eggbarn_y+15, eggbarn_x, 1);
	std::thread n2(fill_nest, nest2, eggbarn_y+15, eggbarn_x+12, 2);
	std::thread n3(fill_nest, nest3, eggbarn_y+15, eggbarn_x+24, 3);
	std::thread fa(move_farmer, farmer, eggbarn_y+18, eggbarn_y+22, eggbarn_x+8, eggbarn_x+10);
	std::thread e1(move_conveyor, eggs1,   bakery_y+4,  bakery_x-7, 17, 0);
	std::thread e2(move_conveyor, eggs2,   bakery_y+4,  bakery_x-7, 8, 18);
	std::thread f1(move_conveyor, flour1,  bakery_y+8-1,  bakery_x-7, 17, 0);
	std::thread f2(move_conveyor, flour2,  bakery_y+8-1,  bakery_x-7, 8, 18);
	std::thread s1(move_conveyor, sugar1,  bakery_y+12-1, bakery_x-7, 17, 0);
	std::thread s2(move_conveyor, sugar2,  bakery_y+12-1, bakery_x-7, 8, 18);
	std::thread b1(move_conveyor, butter1, bakery_y+16, bakery_x-7, 17, 0);
	std::thread b2(move_conveyor, butter2, bakery_y+16, bakery_x-7, 8, 18);
	std::thread mx(fill_mixer, bakery_y+16, bakery_x+22);
	std::thread bt(draw_batter, bakery_y+9, bakery_x+24);
	std::thread ck(add_cakes, cupcakes, bakery_y+5, bakery_x+30);
	std::thread ch1(move_children, child, bakery_y+4,  bakery_x+80);
	std::thread ch2(move_children, DisplayObject(child), bakery_y+8,  bakery_x+50);
	std::thread ch3(move_children, DisplayObject(child), bakery_y+12, bakery_x+50);
	std::thread ch4(move_children, DisplayObject(child), bakery_y+16, bakery_x+50);
	std::thread ch5(move_children, DisplayObject(child), bakery_y+20, bakery_x+50);

	c2.join();
	c1.join();
	rd.join();
    return 0;
}
