#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include "bignum.hpp"
#include <cmath>

//
// A Bignum is just a std::vector containing digits in the range 0-9, following the decimal
// convention that a number other than 0 has no leading 0's.  Oddly, that one rule made the
// code unexpectedly complicated; it would have been easier to just allow numbers like 0002!
//

std::map<int, Bignum> Bignum::pow_2;

Bignum::Bignum()
{
}

Bignum::Bignum(const Bignum& b)
{
	digits = b.digits;
}

Bignum::Bignum(int d)
{
	std::string s = std::to_string(d);
	// std::cout << "tes: " << s << std::endl;
	if (OPT4) {
		// s contains base-10 representation of bignum
		unsigned int num_zeros = 4; // always 4 since we assume base-10,000
		while (s.size() > num_zeros) {
			std::string last_four = s.substr(s.size() - num_zeros);
			digits.push_back(std::stoi(last_four));
			s.erase(s.size() - 4);
		}
		digits.push_back(std::stoi(s));
		std::reverse(digits.begin(), digits.end());
	}
	else {
		for(auto c: s)
			digits.push_back(c-'0');
	}
}

Bignum::Bignum(std::string s)
{
	if (!s.empty()) {
		// std::cout << "yeah: " << s << std::endl;
		if (OPT4) {
			// s contains base-10 representation of bignum
			unsigned int num_zeros = std::log10(BASE); // always 4 since we assume base-10,000
			// std::cout << num_zeros << std::endl;
			while (s.size() > num_zeros) {
				std::string last_four = s.substr(s.size() - num_zeros);
				// std::cout << "last four=";
				// for (auto c: last_four)
				// 	std::cout << c;
				// std::cout << std::endl;
				digits.push_back(std::stoi(last_four));
				s.erase(s.size() - num_zeros);
			}
			// std::cout << "string s = " << std::endl;
			digits.push_back(std::stoi(s));
			std::reverse(digits.begin(), digits.end());

		}
		else {
			for(auto c: s)
				digits.push_back(c-'0');
		}
	}
}

std::vector<int> Bignum::as_vec() const
{
	return digits;
}

std::string Bignum::to_string() const
{
	std::string s;
	unsigned int num_zeros = std::log10(BASE); 
	for(auto d: digits)
	{
		std::string d_to_string = std::to_string(d);
		unsigned int i = 0;
		if (s.size() != 0) {
			while (d_to_string.size() + i < num_zeros) {
				s += "0";
				i++;
			}
		}
		s += std::to_string(d);
	}
	return s;
}

bool Bignum::operator<(const Bignum& b) const
{
	int alen = digits.size();
	int blen = b.digits.size();
	int aindex = 0, bindex = 0;
	if(alen > blen)
	{
		return false;
	}
	if(blen > alen)
	{
		return true;
	}
	while(alen-- > 0)
	{
		if(digits[aindex] > b.digits[bindex])
		{
			return false;
		}
		if(digits[aindex++] < b.digits[bindex++])
		{
			return true;
		}
	}
	return false;
}

bool Bignum::operator>(const Bignum& b) const
{
	return b<*this;
}

bool Bignum::operator==(const Bignum& b) const
{
	return ! (*this<b || b<*this);
}

bool Bignum::operator<=(const Bignum& b) const
{
	return ! (*this>b);
}

bool Bignum::operator>=(const Bignum& b) const
{
	return ! (*this<b);
}

bool Bignum::operator!=(const Bignum& b) const
{
	return ! (*this==b);
}

Bignum Bignum::operator+(const Bignum& b) const
{
	Bignum res;
	int alen = digits.size();
	int blen = b.digits.size();
	int aindex = 0, bindex = 0;
	while(alen > blen)
	{
		res.digits.push_back(digits[aindex++]);
		--alen;
	}
	while(blen > alen)
	{
		res.digits.push_back(b.digits[bindex++]);
		--blen;
	}
	while(alen-- > 0)
	{
		res.digits.push_back(digits[aindex++] + b.digits[bindex++]);
	}
	int rlen = res.digits.size();
	int carry = 0;
	while(rlen > 0)
	{
		carry += res.digits[--rlen];
		res.digits[rlen] = carry%BASE;
		carry /= BASE;
	}
	if (OPT3) {
		std::vector<int> temp;
		while(carry > 0)
		{
			temp.push_back(carry%BASE);
			carry /= BASE;
		}
		std::reverse(temp.begin(), temp.end());
		res.digits.insert(res.digits.begin(), temp.begin(), temp.end());
	}
	else {
		while(carry > 0)
		{
			res.digits.emplace(res.digits.begin(), carry%BASE);
			carry /= BASE;
		}
	}
	return res;
}

Bignum Bignum::operator-(const Bignum& b) const
{
	Bignum res;
	int alen = digits.size();
	int blen = b.digits.size();
	if(*this < b)
	{
		return BZero;
	}
	int borrow = 0;
	int zeros = 0;
	while(alen > 0 && blen > 0)
	{
		int d = digits[--alen]-b.digits[--blen]-borrow;
		borrow = 0;
		if(d < 0)
		{
			d += BASE;
			borrow = 1;
		}
		if(d == 0)
		{
			++zeros;
		}
		else
		{
			if (OPT3) {
				std::vector<int> temp(zeros+1, 0);
				temp[0] = d;
				zeros = 0;
				res.digits.insert(res.digits.begin(), temp.begin(), temp.end());
			}
			else {
				while(zeros--)
				{
					res.digits.emplace(res.digits.begin(), 0);
				}
				zeros = 0;
				res.digits.emplace(res.digits.begin(), d);
			}
		}
	}
	while(alen > 0)
	{
		int d = digits[--alen]-borrow;
		borrow = 0;
		if(d < 0)
		{
			d += BASE;
			borrow = 1;
		}
		if(d == 0)
		{
			++zeros;
		}
		else
		{
			if (OPT3) {
				std::vector<int> temp(zeros+1, 0);
				temp[0] = d;
				zeros = 0;
				res.digits.insert(res.digits.begin(), temp.begin(), temp.end());
			}
			else {
				while(zeros--)
				{
					res.digits.emplace(res.digits.begin(), 0);
				}
				zeros = 0;
				res.digits.emplace(res.digits.begin(), d);
			}
		}
	}
	if(res.digits.size() == 0)
	{
		return BZero;
	}
	return res;
}

// Helper procedure to multiple a Bignum by an integer.  This could be just a single
// digit, but in fact it will sometimes be 10 in my code.  We multiple all the digits
// first, then sweep from low to high digit applying the carries.
Bignum Bignum::operator*(int d) const
{
	if(d == 0)
	{
		return BZero;
	}
	if(d == 1)
	{
		return *this;
	}
	Bignum res;
	int alen = digits.size();
	int aindex = 0;
	while(alen--)
	{
		res.digits.push_back(digits[aindex++] * d);
	}
	int carry = 0;
	while(aindex-- > 0)
	{
		carry += res.digits[aindex];
		res.digits[aindex] = carry%BASE;
		carry /= BASE;
	}
	if (OPT3) {
		std::vector<int> temp;
		while(carry > 0)
		{
			temp.push_back(carry%BASE);
			carry /= BASE;
		}
		std::reverse(temp.begin(), temp.end());
		res.digits.insert(res.digits.begin(), temp.begin(), temp.end());
	}
	else {
		while(carry > 0)
		{
			res.digits.emplace(res.digits.begin(), carry%BASE);
			carry /= BASE;
		}
	}
	return res;
}

Bignum Bignum::operator*(const Bignum& b) const
{
	int blen = b.digits.size();
	Bignum tmp = *this;
	Bignum res = BZero;
	while(blen > 0)
	{
		int d = b.digits[--blen];
		if(d == 1)
		{
			res = res + tmp;
		}
		else if(d != 0)
		{
			res = res + tmp*d;
		}
		if(blen > 0)
		{
			tmp = tmp*BASE;
		}
	}
	return res;
}

int Bignum::size() const
{
	return digits.size();
}

// returns 2^n in Bignum form
Bignum Bignum::two_exp(int n) const
{
	// auto it = pow_2.find(n);
	// if (it != pow_2.end()) {
	// 	// std::cout << "Found value for 2^" << n << ": " << it->second.to_string() << std::endl;
	// 	return it->second;
	// }
	// else {
	// 	// std::cout << "Not found in pow_2, result for 2^" << n << "is: ";
	if (n <= 0) {
		return Bignum("1");
	}
	Bignum res = Bignum("2");
	// for (int i = 1; i < n; i++) {
	// 	res = res * 2;
	// }
	Bignum temp = Bignum("1");
	while (n > 1) {
		if ((n & 1) == 0) {
			n >>= 1;
		}
		else {
			temp = res * temp;
			n = (n-1) >> 1;
		}
		res = res * res;
	}
	res = res * temp;
	pow_2[n] = res;
	// std::cout << res.to_string() << std::endl;
	return res;
	// }
}

// get value from certain index in vector
int Bignum::get_value(int index) const
{
	return digits[index];
}

std::pair<Bignum, Bignum> Bignum::divmod(const Bignum& b) const
{
	std::cout << "dividing " << to_string() << " by " << b.to_string() << std::endl;
	// first: division result, second: modulus result
	std::pair<Bignum, Bignum> divmod_result;
	// Checking simple cases
	if(b > *this)
	{
		divmod_result.first = BZero;
		divmod_result.second = *this;
		return divmod_result;
	}
	if(b == *this)
	{
		divmod_result.first = BOne;
		divmod_result.second = BZero;
		return divmod_result;
	}
	if(b == BZero)
	{
		std::cout << "Error: Divide by 0" << std::endl;
		exit(1);
	}
	Bignum res;
	Bignum tmp;

	bool first = true;
	if (OPT4) {
		tmp = *this;

		// std::cout << "after copying, tmp = " << tmp.to_string() << std::endl;
		// want to calculate tmp / b, store result in digits
		std::map<int, Bignum> table;
		int k = 0;
		table[k] = b;
		// creating table of values that we will use in our binary search
		do
		{
			k++;
			table[k] = table[k-1] + table[k-1];
		} while (table[k] <= tmp);

		// for (unsigned int i = 0; i < table.size(); i++) {
		// 	std::cout << "index = " << i << std::endl;
		// 	std::cout << "key = " << table[i].to_string() << std::endl;
		// }

		// std::cout << "largest element in table = " << table[table.size()-1].to_string() << std::endl;
		// std::cout << "table size = " << table.size() << std::endl;
		int prev_index = table.size() - 1; // used to store the previous table index we used; we now can start the downward search from here

		while (tmp >= b) {

			int index = -200;
			for (unsigned int k = prev_index; k >= 0; k--) {
				// std::cout << "table element = " << table[k].to_string() << std::endl;
				// std::cout << "tmp = " << tmp.to_string() << std::endl;

				if (table[k] <= tmp) {
					index = k;
					break;
				}
				// if (table[k] == tmp) {
				// 	index = k-1;
				// 	break;
				// }
			}
			if (index == -200) {
				std::cout << "ERROR AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << std::endl;
			}
			tmp = tmp - table[index];
			// std::cout << "correct index = " << index << std::endl;
			// std::cout << "index gives value of = " << table[index].to_string() << std::endl;
			res = res + two_exp(index);
			// std::cout << "tmp after this iteration is = " << tmp.to_string() << std::endl;
			// std::cout << "res after this iteration is = " << res.to_string() << std::endl;
			prev_index = index;
		}	
	}
	else {
		int alen = digits.size();
		int blen = b.digits.size();
		int aindex = 0;
		bool placed_first = false;
		// storing the value of digits into tmp (while dealing with leading zeros)
		while(aindex < blen)
		{
			int d = digits[aindex++];
			if(d != 0 || placed_first)
			{
				tmp.digits.push_back(d);
				placed_first = true;
			}
		}
		if(!placed_first)
		{
			tmp = BZero;
		}
		do
		{
			int d = 0;
			if(b > tmp && aindex < alen)
			{
				d = digits[aindex++];
				if(d != 0 || tmp != BZero)
				{
					if(tmp == BZero)
					{
						tmp.digits[0] = d;	
					}
					else
					{
						tmp.digits.push_back(d);
					}
				}
			}
			d = 0;
			while(b <= tmp)
			{
				++d;
				tmp = tmp-b;
			}
			if(d != 0 || !first)
			{
				first = false;
				res.digits.push_back(d);
			}
		} while(aindex < alen);
	}

	if(res.digits.size() == 0)
	{
		divmod_result.first = BZero;
		divmod_result.second = BZero;
		return divmod_result;
	}
	divmod_result.first = res;
	divmod_result.second = tmp;
	return divmod_result;
}

Bignum Bignum::operator/(const Bignum& b) const
{
	return divmod(b).first;
}

Bignum Bignum::operator%(const Bignum& b) const
{
	if (OPT2) {
		return divmod(b).second;
	}
	return *this - (*this/b)*b;
}

Bignum Bignum::expmod(Bignum exp, const Bignum& mod) const
{
	if(exp == BZero)
	{
		return BOne;
	}
	Bignum two = Bignum(2);
	Bignum res = BOne;
	Bignum tmp = *this;
	while(exp > 0)
	{
		// Idea 1: use bitwise and instead of modulus operator
		if (OPT1) {
			if(!is_even(exp)) {
			// if ((exp & 1) == 0) {
				res = res*tmp;
				res = res % mod;
			}
		}

		else {
			if(exp % two == BOne) {
				res = res*tmp;
				res = res % mod;
			}
		}
		tmp = tmp*tmp % mod;
		exp = exp/two;

	}
	return res;
}

Bignum Bignum::gcd(const Bignum& b) const
{
	if (b == BZero)
		return *this;
	return b.gcd(*this % b);
}

Bignum Bignum::encrypt(std::string rsa_n, std::string rsa_e) const
{
	return expmod(Bignum(rsa_e), Bignum(rsa_n)).to_string();
}

Bignum Bignum::decrypt(std::string rsa_n, std::string rsa_d) const
{
	return expmod(Bignum(rsa_d), Bignum(rsa_n));
}

bool Bignum::is_even(const Bignum& num) const
{
	if (num.digits.empty()){
		return false;
	}
	return num.digits.back() % 2 == 0;
}