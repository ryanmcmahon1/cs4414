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
   __ ^#\
  /  /  \\#\
 |  | _  |#\
 |  |[ ] |#\
 Butter/Egg", 3);

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

std::atomic<int> eggs_laid, eggs_used;
std::atomic<int> butter_produced, butter_used;
std::atomic<int> sugar_produced, sugar_used;
std::atomic<int> flour_produced, flour_used;
std::atomic<int> cakes_produced, cakes_sold;

int wait = 1000000;
int delay = 150;

// Position of bakery, so that we can keep track of where to put items inside the bakery
int bakery_y = 35;
int bakery_x = 75;
int flourbarn_y = 1;
int flourbarn_x = 25;
int eggbarn_y = 33;//10;
int eggbarn_x = 1;//1;
int int_ymin = eggbarn_y+3;//26;
int int_ymax = eggbarn_y+7;//26+4;
int int_xmin = eggbarn_x+36;
int int_xmax = eggbarn_x+36+15;

std::condition_variable cv; // condition variable controlling drawing and redisplaying
std::mutex cv_mutex;
std::atomic<bool> go(false); // set to 1 while we are redisplaying

std::atomic<bool> int_full(false); // set to true if a truck is in the intersection

// // control number of eggs in each of the three nests
// std::atomic<int> eggs_n1(0);
// std::atomic<int> eggs_n2(0);
// std::atomic<int> eggs_n3(0);
std::atomic<int> nest1_eggs(0);
std::atomic<int> nest2_eggs(0);
std::atomic<int> eggs_collected(0);
std::atomic<int> butter_collected(0);

// control access to nests and the lanes between them (true if occupied)
std::atomic<bool> l1_full(true);
std::atomic<bool> l2_full(false);
std::atomic<bool> n1_full(false);
std::atomic<bool> n2_full(false);

// number of each container currently on conveyor belt
// std::atomic<int> conveyor_eggs(0);
// std::atomic<int> conveyor_butter(0);
// std::atomic<int> conveyor_sugar(0);
// std::atomic<int> conveyor_flour(0);

void redisplay()
{
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay*2));
		{
			std::lock_guard<std::mutex> lock(cv_mutex);
			DisplayObject::redisplay();
			std::cout << "Eggs: Laid=" << eggs_laid << ", Collected=" << eggs_collected << ", Used=" << eggs_used << 
							" Butter: Collected=" << butter_collected << " Sold=" << butter_produced << ", Used=" << butter_used <<
							" Sugar: Sold=" << sugar_produced << ", Used=" << sugar_used <<
							" Flour: Sold=" << flour_produced << ", Used=" << flour_used << 
							" Cakes: Baked=" << cakes_produced << ", Sold=" << cakes_sold << std::endl;
			// std::cout << "n1_full=" << n1_full << ", n2_full=" << n2_full << ", l1_full=" << l1_full << ", l2_full=" << l2_full << std::endl;
			go = true;
		}
		cv.notify_all();
	}
}

// int count = 0;
std::atomic<int> count(0);

// x and y positions of each nest
int n1_x = eggbarn_x-1;
int n2_x = eggbarn_x-1+24;
int n_y  = eggbarn_y-11;

// chicken dimensions
int ch = 3;
int cw = 4;

int l2_y = n_y - (ch + 2);

void move_chicken(DisplayObject chicken, int num, int ymin, int ymax, int xmin, int xmax)
{
	// chicken 1 initially in lane1, chicken 2 currently in lane2
	int y = n_y, oldy = n_y, x = n1_x+cw+2, oldx = n1_x+cw+2;
	bool right = false, left = false, up = false, down = false;
	bool in_l1 = false, in_l2 = false, in_n1 = false, in_n2 = false;
	bool entering = false, leaving = false;
	int egg_count = 0;

	if (num == 1) {
		in_l1 = true;
		right = true;
	}
	else if (num == 2) {
		in_l2 = true;
		left = true;
		y = l2_y;
	}

	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;

		bool in_lane = in_l1 || in_l2; // true if this chicken is occupying a lane
		bool in_nest = in_n1 || in_n2; // true if this chicken is occupying a nest


		// std::cout << "current x,y = " << x << ", " << y << "; final y = " << n_y << std::endl;
		if (in_lane && !in_nest) {

			// check if we are in a critical position or not (about to enter a nest)
			int to_nest = 0;
			bool in_crit = false;
			if (in_l1) {
				if (right && x == n2_x-cw && y >= n_y) {
					in_crit = true;
					to_nest = 2;
				}
				else if (left && x == n1_x+cw && y >= n_y) {
					in_crit = true;
					to_nest = 1;
				}
				else {
					in_crit = false;
				}
			}
			else if (in_l2) {
				if (down && x == n1_x && y == n_y-ch) {
					in_crit = true;
					to_nest = 1;
				}
				else if (down && x == n2_x && y == n_y-ch) {
					in_crit = true;
					to_nest = 2;
				}
				else {
					in_crit = false;
				}
			}

			// if not in critical position, continue moving in same direction
			if (!in_crit) {
				if (in_l2 && y == l2_y) {
					if (x == n1_x) {
						if (up) {
							up = false, left = false, down = false;
							right = true;
						}
						else if (left) {
							left = false, up = false, right = false;
							down = true;
						}
					}
					else if (x == n2_x) {
						if (up) {
							up = false, right = false, down = false;
							left = true;
						}
						else if (right) {
							right = false, up = false, left = false;
							down = true;
						}
					}
				}
				if (up)
					y = y - 1;
				else if (down)
					y = y + 1;
				else if (right)
					x = x + 1;
				else if (left)
					x = x - 1;
			}

			// if in critical position, check if nest is full or empty
			else {
				if (to_nest == 1) {
					if (!n1_full) {
						n1_full = true;
						in_n1 = true;
						entering = true;
					}
				}
				else if (to_nest == 2) {
					if (!n2_full) {
						n2_full = true;
						in_n2 = true;
						entering = true;
					}
				}
			}
		}

		else if ((in_n1 || in_n2) && (in_l1 || in_l2)) {
			// std::cout <<"entering is " << entering << std::endl;
			if (entering && !leaving) {
				if (in_n1) {
					// check if we are in final position for n1
					if (x == n1_x && y >= n_y) {
						y = n_y;
						// std::cout << "in final position for n1" << std::endl;
						entering = false;
						if (in_l1) {
							in_l1 = false;
							l1_full = false;
						}
						else if (in_l2) {
							in_l2 = false;
							l2_full = false;
						}
						up = false, right = false, down = false, left = false;
						// set number of eggs to lay
						egg_count = (rand() % 3) + 1;
					}
				}
				else if (in_n2) {
					// check if we are in final position for n2
					if (x == n2_x && y >= n_y) {
						y = n_y;
						// std::cout << "in final position for n2" << std::endl;
						entering = false;
						if (in_l1) {
							in_l1 = false;
							l1_full = false;
						}
						else if (in_l2) {
							in_l2 = false;
							l2_full = false;
						}
						up = false, right = false, down = false, left = false;
						// set number of eggs to lay
						egg_count = (rand() % 3) + 1;
					}
				}
				// update location if we are still in process of entering nest
				if (entering) {
					if (up)
						y = y - 1;
					else if (down)
						y = y + 1;
					else if (right)
						x = x + 1;
					else if (left)
						x = x - 1;
				}
			}
			else if (leaving && !entering) {
				// std::cout << "in_n2, in_l2 = " << in_n2 << ", " << in_l2 << ", leaving = " << leaving << ", entering = " << entering << std::endl;
				// std::cout << n_y+ch << ", " << y << std::endl;
				if (in_n1 && in_l1 && x == n1_x+cw && y == n_y) {
					leaving = false;
					in_n1 = false;
					n1_full = false;
				}
				else if (in_n1 && in_l2 && x == n1_x && y <= n_y+ch) {
					y = n_y;
					leaving = false;
					in_n1 = false;
					n1_full = false;
				}
				else if (in_n2 && in_l1 && x == n2_x-cw && y == n_y) {
					leaving = false;
					in_n2 = false;
					n2_full = false;
				}
				else if (in_n2 && in_l2 && x == n2_x && y <= n_y+ch) {
					y = n_y;
					leaving = false;
					in_n2 = false;
					n2_full = false;
				}

				// update location if we are still in process of leaving nest
				if (leaving) {
					if (up)
						y = y - 1;
					else if (down)
						y = y + 1;
					else if (right)
						x = x + 1;
					else if (left)
						x = x - 1;
				}
			}
		}
		else if (!(in_l1 || in_l2) && (in_n1 || in_n2) && !entering && !leaving) {
			egg_count--;
			// std::cout << "egg count = " << egg_count << std::endl;
			if (egg_count <= 0) {
				// select availble lane, set leaving to true
				leaving = true;
				entering = false;
				int new_lane = 0;
				if (!l1_full && !l2_full) {
					int choice = rand() % 2;
					if (choice)
						new_lane = 1;
					else
						new_lane = 2;
				}
				else if (!l1_full)
					new_lane = 1;
				else if (!l2_full)
					new_lane = 2;

				if (new_lane == 1) {
					l1_full = true;
					in_l1 = true;
					if (in_n1) {
						right = true, left = false, up = false, down = false;
					}
					else if (in_n2) {
						left = true, right = false, up = false, down = false;
					}
				}
				else if (new_lane == 2) {
					l2_full = true;
					in_l2 = true;
					up = true, left = false, down = false, right = false;		
				}
			}
			else if (egg_count > 0) {
				// TODO: add eggs
				if (in_n1 && nest1_eggs <= 3) {
					eggs_laid++;
					nest1_eggs++;
				}
				else if (in_n2 && nest2_eggs <= 3) {
					eggs_laid++;
					nest2_eggs++;
				}
			}
		}

		chicken.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
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
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves the farmer around randomly
void move_farmer(DisplayObject farmer, int ymin, int ymax, int xmin, int xmax)
{
	int y = ymin, oldy = ymin, x = xmin+5, oldx = xmin+5;
	bool right = true, left = false, up = false, down = false;
	bool waiting = false;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;

		if ((right && x == xmax) || (waiting && x == xmax && y <= ymin)) {
			if (n2_full) {
				waiting = true;
				right = false;
			}
			else {
				eggs_collected += nest2_eggs;
				nest2_eggs = 0;
				// TODO: update egg statistics
				right = false;
				down = true;
				waiting = false;
			}
		}
		else if (down && y >= ymax) {
			butter_collected += 2;
			down = false;
			left = true;
		}
		else if (left && x == xmin) {
			left = false;
			up = true;
		}
		else if ((up && y <= ymin) || (waiting && x == xmin && y <= ymin)) {
			if (n1_full) {
				waiting = true;
				up = false;
			}
			else {
				eggs_collected += nest1_eggs;
				nest1_eggs = 0;
				// TODO: update egg statistics
				up = false;
				right = true;
				waiting = false;
			}
		}

		if (up)
			y = y - 1;
		else if (down)
			y = y + 1;
		else if (right)
			x = x + 1;
		else if (left)
			x = x - 1;
		
		farmer.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves a truck along a vertical road (from ymin to ymax), should start from bottom of path, moving upwards
void move_truckv(DisplayObject truck, int ymin, int ymax, int xmin, int xmax)
{
	int y = ymax, oldy = ymax, x = xmin, oldx = xmin;
	bool up = true; // true if truck is currently moving "up" (with respect to the view of the screen)
	bool down = false;
	bool right = false; // true if truck is moving right
	bool left = false;
	bool in_int = false; // true if this truck is in the intersection
	// number of flour and sugar containers currently in this truck
	int num_flour = 0, num_sugar = 0;
	
	while (true) {
		// std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;

		// check if we are about to enter the intersection from either direction
		bool entering = (!up && y+3 == int_ymin) || (up && y == int_ymax);
		if (entering) {
			if (!int_full) {
				int_full = true;
				in_int = true;
			}	
		}

		// only update location if the intersection is unoccupied, or occupied by this truck, or if
		// we are not near the intersection
		if (!int_full || (int_full && in_int) || !entering) {
			if (y <= ymin) {
				up = false;
				down = true;
				left = false;
				right = false;
				num_flour = 3;
				num_sugar = 3;
			}
			else if (y >= ymax) {
				if (down) {
					right = true;
					left = false;
					up = false;
					down = false;
				}
				else {
					if (x == xmin) {
						up = true;
						down = false;
						right = false;
						left = false;
					} 
					else if (x == xmax) {
						if (num_flour == 0 && num_sugar == 0) {
							left = true;
							right = false;
							down = false;
							up = false;
						}
						else {
							num_flour--;
							num_sugar--;
							left = false, right = false, down = false, up = false;
						}
					}
				}
			}
			if (up)
				y = y - 1;
			else if (down)
				y = y + 1;
			else if (right)
				x = x + 1;
			else if (left)
				x = x - 1;
		}

		// check if we have left intersection so other truck can enter
		if ((!up && y == int_ymax) || (up && y+3 == int_ymin)) {
			int_full = false;
			in_int = false;
		}

		std::unique_lock<std::mutex> lock(cv_mutex);
		truck.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// moves a truck along a horizontal road (from xmin to xmax), should start from left of path, moving to the right
void move_truckh(DisplayObject truck, int ymin, int ymax, int xmin, int xmax)
{
	int y = ymax, oldy = ymax, x = xmin, oldx = xmin;
	bool right = true, left = false;; // true if truck is currently moving "right"
	bool in_int = false;
	// bool waiting = false;

	// number of crates of eggs and package of butter currently in truck
	int num_crates = 0, num_butter = 0;
	
	while (true) {
		// std::unique_lock<std::mutex> lock(cv_mutex);
		go = false;

		bool entering = (right && x+14 == int_xmin) || (left && x == int_xmax);
		if (entering) {
			if (!int_full) {
				int_full = true;
				in_int = true;
			}
		}

		if (!int_full || (int_full && in_int) || !entering) {
			if (x <= xmin) {
				if (eggs_collected >= 9 && butter_collected >= 3) {
					eggs_collected -= 9;
					butter_collected -= 3;
					num_crates = 3;
					num_butter = 3;
					right = true;
					left = false;
				}
				else {
					right = false, left = false;
				}
			}
			else if (x >= xmax) {
				// add condition for waiting to drop off at patisserie
				if (num_crates == 0 && num_butter == 0) {
					right = false;
					left = true;
				}
				else {
					num_crates--;
					num_butter--;
					right = false, left = false;
				}
			}
			if (right)
				x = x + 1;
			else if (left)
				x = x - 1;	
		}

		if ((right && x == int_xmax) || (left && x+14 == int_xmin)) {
			int_full = false;
			in_int = false;
		}
	
		std::unique_lock<std::mutex> lock(cv_mutex);
		truck.draw(oldy = y, oldx = x);
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
		cv.wait_until(lock, timeout, [&](){return go == true;});
	}
}

// adds an egg to the nest each iteration
void fill_nest(DisplayObject nest[4], int y, int x, int num) {
	// int n = num, l = 0;
	while (true) {
		std::unique_lock<std::mutex> lock(cv_mutex);
		if (num == 1) {
			if (nest1_eggs >= 0 && nest1_eggs <= 3)
				nest[nest1_eggs].draw(y,x);
		}
		else if (num == 2) {
			if (nest2_eggs >= 0 && nest2_eggs <= 3)
				nest[nest2_eggs].draw(y,x);
		}
		// if (n % 2 == 0)
		// 	l++;
		// std::unique_lock<std::mutex> lock(cv_mutex);
		// nest[l % 4].draw(y, x);
		// n++;
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
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
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
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
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
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
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
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
		auto timeout = std::chrono::system_clock::now() + std::chrono::milliseconds(delay*2);
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
	cow.draw(eggbarn_y, eggbarn_x+24);
	intersection.draw(eggbarn_y+3, eggbarn_x+36);
	
	// TODO: add PATH data structure, use relative locations rather than absolute locations
	std::thread rd(redisplay);
	std::thread c1(move_chicken, chicken1, 1, eggbarn_y-17, eggbarn_y-11, eggbarn_x, eggbarn_x+30);
	std::thread c2(move_chicken, chicken2, 2, eggbarn_y-17, eggbarn_y-11, eggbarn_x+15, eggbarn_x+30);
	std::thread tf(move_truckv, flour_truck, flourbarn_y, bakery_y+12, flourbarn_x+12, bakery_x-15);
	std::thread te(move_truckh, egg_truck, eggbarn_y+3, eggbarn_y+4, eggbarn_x+12, bakery_x-15);
	std::thread n1(fill_nest, nest1, eggbarn_y-8, eggbarn_x, 1);
	std::thread n2(fill_nest, nest2, eggbarn_y-8, eggbarn_x+24, 2);
	// std::thread n3(fill_nest, nest3, eggbarn_y-8, eggbarn_x+24, 3);
	std::thread fa(move_farmer, farmer, eggbarn_y-7, eggbarn_y-2, eggbarn_x, eggbarn_x+24);
	std::thread e1(move_conveyor, eggs1,   bakery_y+4,  bakery_x-7, 17, 0);
	std::thread e2(move_conveyor, eggs2,   bakery_y+4,  bakery_x-7, 8, 18);
	std::thread b1(move_conveyor, butter1,  bakery_y+8,  bakery_x-7, 17, 0);
	std::thread b2(move_conveyor, butter2,  bakery_y+8,  bakery_x-7, 8, 18);
	std::thread s1(move_conveyor, sugar1,  bakery_y+12-1, bakery_x-7, 17, 0);
	std::thread s2(move_conveyor, sugar2,  bakery_y+12-1, bakery_x-7, 8, 18);
	std::thread f1(move_conveyor, flour1, bakery_y+15, bakery_x-7, 17, 0);
	std::thread f2(move_conveyor, flour2, bakery_y+15, bakery_x-7, 8, 18);
	std::thread mx(fill_mixer, bakery_y+16, bakery_x+22);
	std::thread bt(draw_batter, bakery_y+9, bakery_x+24);
	std::thread ck(add_cakes, cupcakes, bakery_y+5, bakery_x+30);
	std::thread ch1(move_children, child, bakery_y+4,  bakery_x+80);
	std::thread ch2(move_children, DisplayObject(child), bakery_y+8,  bakery_x+50);
	std::thread ch3(move_children, DisplayObject(child), bakery_y+12, bakery_x+50);
	std::thread ch4(move_children, DisplayObject(child), bakery_y+16, bakery_x+50);
	std::thread ch5(move_children, DisplayObject(child), bakery_y+20, bakery_x+50);

	// c2.join();
	c1.join();
	rd.join();
    return 0;
}
