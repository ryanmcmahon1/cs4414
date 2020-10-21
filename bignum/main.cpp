// Name: Ryan McMahon
// NetID: rm756
// Date: 9/10/20

#include <iostream>
#include <set>
#include <algorithm>
#include "bignum.hpp"

using namespace std;

bool isValidInteger(string arg)
{
	for (auto c: arg) {
		if (!isdigit(c)) {
			cout << "Error: \"" << arg << "\" is not an unsigned integer" << endl;
			return false;
		}
	} 
	return true;
}

bool isValidOperator(string s)
{
	// set of valid operators
	set<string> args = {">", "<", "==", "+", "-", "*", "/", "%", "gcd"};

	if (args.find(s) != args.end()) {
		return true;
	}

	cout << "Error: " << s << " is not a supported operator" << endl;
	return false;
}

void compute(string arg, Bignum num1, Bignum num2)
{
	if (arg == ">") {
		cout << (num1 > num2) << endl;
	}
	else if (arg == "<") {
		cout << (num1 < num2) << endl;
	}
	else if (arg == "==") {
		cout << (num1 == num2) << endl;
	}
	else if (arg == "+") {
		Bignum res = num1 + num2;
		res.print();
	}
	else if (arg == "-") {
		Bignum res = num1 - num2;
		res.print();
	}
	else if (arg == "*") {
		Bignum res = num1 * num2;
		res.print();
	}
	else if (arg == "/") {
		Bignum res = num1 / num2;
		res.print();
	}
	else if (arg == "%") {
		Bignum res = num1 % num2;
		res.print();
	}
	else if (arg == "gcd") {
		Bignum res = Bignum::gcd(num1, num2);
		res.print();
	}
	else {
		cout << "Error: not a valid operator" << endl;
	}
}

int main(int argc, char** argv)
{
	if (argc != 4) {
		cout << "bignum\nUsage: bignum op number1 number2" << endl;
		return 1;
	}
	
	string arg1 = string(argv[1]);
	string arg2 = string(argv[2]);
	string arg3 = string(argv[3]);
	bool res2 = isValidInteger(arg2);
	bool res3 = isValidInteger(arg3);
	
	if (!res2 || !res3) {
		return 1;
	}
	
	if (!isValidOperator(arg1)) {
		return 1;
	}

	Bignum num1 = Bignum(arg2);
	Bignum num2 = Bignum(arg3);

	compute(arg1, num1, num2);
	
	return 0;
}
