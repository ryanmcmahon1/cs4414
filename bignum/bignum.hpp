// Name: Ryan McMahon
// NetID: rm756
// Date: 9/10/20

#include <iostream>
#include <vector>
using namespace std;

/*
 * Bignum class uses an internal vector of integers to store very large numbers so
 * that various arithmetic operations can be performed on them
 */
class Bignum
{
	public:
		// Constructors
		Bignum();
		Bignum(string);
		Bignum(vector<int>);

		// operator overloaders
		bool operator<(const Bignum& b);
		bool operator>(const Bignum& b);
		bool operator==(const Bignum& b);
		Bignum operator+(Bignum b);
		Bignum operator-(Bignum b);
		Bignum operator*(Bignum b);
		Bignum operator/(Bignum b);
		Bignum operator%(Bignum b);

		static Bignum gcd(Bignum num1, Bignum num2);

		string to_string();
		const vector<int>& get_value() const;
		void print() const;
		int size() const;

	private:
		// vector that stores each digit of the bignum (0-9)
		vector<int> value;
		
		// helper methods
		vector<int> multiply(vector<int> vec, int mult);
		pair<vector<int>, vector<int>> divide(vector<int> top, vector<int> btm);

		bool check(Bignum);
		void zero();
		vector<int> add_vectors(vector<vector<int>> vecs);
};
