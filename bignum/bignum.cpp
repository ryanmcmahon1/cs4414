// Name: Ryan McMahon
// NetID: rm756
// Date: 9/10/20

#include <algorithm>
#include <cmath>
#include <map>
#include <utility>
#include "bignum.hpp"

void Bignum::zero()
{
	value = vector<int>();
	value.push_back(0);
}

Bignum::Bignum()
{
	zero();
}

Bignum::Bignum(string s)
{
	if (s[0] == '0' && s.size() != 1) {
		cout << "Error: Bignum cannot have leading zeros" << endl;
		zero();
	}

	else {
		bool valid = true;
		value = vector<int>();
		for (char c: s) {
			if (c - '0' >= 0 && c - '0' <= 9) {
				value.push_back(c - '0');
			}

			else {
				cout << "Error: all values in bignum must be in range 0 to 9" << endl;
				valid = false;
			}
		}
		if (!valid)
			zero();
	}
}

int find_first_nonzero(vector<int> vec)
{
	int index = 0;
	for (int i: vec) {
		if (i == 0) {
			index++;
		}

		else {
			return index;
		}
	}

	return -1;
}

Bignum::Bignum(vector<int> vec)
{
	if (vec.size() == 0) {
		zero();
	}

	else if (vec[0] == 0 && vec.size() != 1) {
		// removing extra zeros from the beginning of vec
		int first_nonzero = find_first_nonzero(vec);

		if (first_nonzero != -1) {
			value = vector<int>();
			for (int i = first_nonzero; i < vec.size(); i++) {
				value.push_back(vec[i]);
			}
		}

		else {
			zero();
		}
	}

	else {
		bool valid = true;
		value = vector<int>();
		for (int i: vec) {
			if (i >= 0 && i <= 9) {
				value.push_back(i);
			}

			else {
				cout << "Error: all values in bignum must be in range 0 to 9" << endl;
				valid = false;
			}
		}
		if (!valid)
			zero();
	}
}

bool Bignum::operator<(const Bignum& b)
{
	// as long as we ensure all bignum objects are valid, we can just check the size
	if (size() < b.size())
		return true;

	else if (size() > b.size())
		return false;

	for (int i = 0; i < size(); i++) {
		if (value[i] < b.get_value()[i])
			return true;

		else if (value[i] > b.get_value()[i])
			return false;
	}

	return false;
}

bool Bignum::operator>(const Bignum& b)
{
	// as long as we ensure all bignum objects are valid, we can just check the size
	if (size() > b.size())
		return true;

	else if (size() < b.size())
		return false;

	for (int i = 0; i < size(); i++) {
		if (value[i] > b.get_value()[i])
			return true;

		else if (value[i] < b.get_value()[i])
			return false;
	}

	return false;
}

bool Bignum::operator==(const Bignum& b)
{
	if (b.size() != size()) {
		return false;
	}

	for (int i = 0; i < size(); i++) {
		if (value[i] != b.get_value()[i])
			return false;
	}
	
	return true;
}

void print_vector(vector<int> vec, string name) {
	cout << name << ": ";
	for (int i: vec) {
		cout << i;
	}
	cout << "\n";
}

Bignum Bignum::operator+(Bignum b)
{
	Bignum res = Bignum("");
	vector<int> vec1;
	vector<int> vec2;

	// making vectors the same size to simplify the addition
	int diff = size() - b.size();
	if (diff > 0) {
		vector<int> zeros(diff, 0);
		zeros.insert(zeros.end(), b.get_value().begin(), b.get_value().end());
		vec1 = zeros;
		vec2 = value;
	}

	else if (diff < 0) {
		vector<int> zeros(-diff, 0);
		zeros.insert(zeros.end(), value.begin(), value.end());
		vec1 = zeros;
		vec2 = b.get_value();
	}

	else {
		vec1 = value;
		vec2 = b.get_value();
	}

	int carry = 0;
	for (int i = vec1.size() - 1; i >= 0; i--) {
		if (vec1[i] > 9 || vec1[i] < 0 || vec2[i] > 9 || vec2[i] < 0) {
			cout << "Error: not a valid bignum" << endl;
			return Bignum();
		}

		int sum = vec1[i] + vec2[i] + carry;
		if (sum >= 10) {
			res.value.push_back(sum-10);
			carry = 1;
		}

		else {
			res.value.push_back(sum);
			carry = 0;
		}
	}

	// taking cases where the last two digits have a carry value
	if (carry > 0)
		res.value.push_back(carry);

	// adding elements in reverse order, then reversing at the end
	reverse(res.value.begin(), res.value.end());
	return res;
}


Bignum Bignum::operator-(Bignum b)
{
	if (size() < b.size() || (size() == b.size() && value[0] < b.get_value()[0])) {
		cout << "Unsupported: Negative number." << endl;
		return Bignum();	
	}

	Bignum res = Bignum("");
	vector<int> vec1;
	vector<int> vec2;

	// making vectors the same size to simplify the addition
	int diff = size() - b.size();
	if (diff > 0) {
		vector<int> zeros(diff, 0);
		zeros.insert(zeros.end(), b.get_value().begin(), b.get_value().end());
		vec2 = zeros;
		vec1 = value;
	}

	else if (diff < 0) {
		vector<int> zeros(-diff, 0);
		zeros.insert(zeros.end(), value.begin(), value.end());
		vec2 = b.get_value();
		vec1 = zeros;
	}

	else {
		vec1 = value;
		vec2 = b.get_value();
	}

	int carry = 0;
	for (int i = vec1.size() - 1; i >= 0; i--) {
		if (vec1[i] > 9 || vec1[i] < 0 || vec2[i] > 9 || vec2[i] < 0) {
			cout << "Error: not a valid bignum" << endl;
			return Bignum();
		}

		int val = vec1[i] - vec2[i] - carry;
		if (val < 0) {
			res.value.push_back(val + 10);
			carry = 1;
		}

		else {
			res.value.push_back(val);
			carry = 0;
		}
	}

	// taking cases where the last two digits have a carry value
	if (carry > 0) {
		cout << "Subtraction of larger numbers not supported" << endl;
		return Bignum();
	}

	// adding a check for leading zeros (at this point they would be at the end)
	int zero_count = 0;
	for (int i = res.size() - 1; i >= 0; i--) {
		if (res.value[i] == 0)
			zero_count++;
	}
	if (!zero_count)
		res.value.erase(res.value.begin()+res.size()-zero_count, res.value.end());

	// adding elements in reverse order, then reversing at the end
	reverse(res.value.begin(), res.value.end());
	return res;
}

vector<int> Bignum::add_vectors(vector<vector<int>> vecs)
{
	vector<int> result;

	int max_size = 0;
	for (auto vec: vecs) {
		if (vec.size() > max_size)
			max_size = vec.size();
	}

	int carry = 0;
	for (int i = 1; i <= max_size; i++) {
		int sum = carry;
		for (auto vec: vecs) {
			if (vec.size() - i >= 0 && vec.size() - i <= vec.size() - 1)
				sum += vec[vec.size() - i];
		}

		if (sum >= 10) {
			result.push_back(sum % 10);
			carry = sum / 10;
		}

		else {
			result.push_back(sum);
			carry = 0;
		}
	}

	if (carry > 0)
		result.push_back(carry);

	reverse(result.begin(), result.end());
	return result;
}

vector<int> Bignum::multiply(vector<int> vec, int mult)
{
	vector<vector<int>> results;
	int j = 0;
	for (int i = vec.size() - 1; i >= 0; i--) {
		vector<int> temp;
		int res = vec[i] * mult;

		for (char c: std::to_string(res))
			temp.push_back(c - '0');

		for (int k = 0; k < j; k++)
			temp.push_back(0);

		results.push_back(temp);
		j++;
	}

	return add_vectors(results);
}

Bignum Bignum::operator*(Bignum b)
{
	vector<int> vec1 = value;
	vector<int> vec2 = b.value;

	// this map is used to store the results of a vector multiplied by a certain value
	// so that we don't have to repeat the same calculation multiple times
	map<int, vector<int>> map;

	vector<vector<int>> results;
	int zeros = 0;
	for (int i = vec2.size() - 1; i >= 0; i--) {
		vector<int> res;
		auto it = map.find(vec2[i]);

		// if we have performed the calculation already, we can directly retrieve it from the map
		if (it != map.end()) {
			res = it->second;
		}

		else {
			res = multiply(vec1, vec2[i]);
			map[vec2[i]] = res;
		}
		
		for (int i = 0; i < zeros; i++)
			res.push_back(0);
		results.push_back(res);
		zeros++;
	}

	return Bignum(add_vectors(results));
}

// Returns true if the value stored in vec1 <= vec2
bool compare_vectors(vector<int> vec1, vector<int> vec2)
{

	if (vec1.size() == vec2.size()) {
		for (int i = 0; i < vec1.size(); i++) {
			if (vec1[i] < vec2[i])
				return true;
			else if (vec1[i] > vec2[i])
				return false;
		}

		return false;
	}

	else {
		if (vec1.size() < vec2.size())
			return true;
		else
			return false;
	}
}

pair<vector<int>, vector<int>> Bignum::divide(vector<int> top, vector<int> btm)
{
	// result.first = division, result.second = remainder
	pair<vector<int>, vector<int>> result;

	long long int bm = 0;
	for (auto i: btm)
		bm = (bm * 10) + i;

	if (bm == 0) {
		cout << "Error: Divide by zero." << endl;
		result.first.push_back(0);
		result.second.push_back(0);
		return result;
	}

	if (top[0] == 0) {
		result.first.push_back(0);
		for (int i: btm)
			result.second.push_back(i);
		return result;
	}

	if (top.size() == 1) {
		result.first.push_back(top[0] / bm);
		result.second.push_back(top[0] % bm);
		return result;
	}

	int index = 1;
	vector<int> prefix = {top[0]};

	while(index < top.size()) {
		// making sure the prefix is larger than the divisor
		while (compare_vectors(prefix, btm)) {
			if (index >= top.size()) {

				for (int i: prefix)
					result.second.push_back(i);
				return result;
			}

			prefix.push_back(top[index]);
			result.first.push_back(0);
			index++;
		}

		long long int tp = 0;
		for (auto i: prefix)
			tp = (tp * 10) + i;

		result.first.push_back(tp / bm);
		if (tp / bm > 9) {
			cout << "Division error: value cannot be greater than 9" << endl;
		}
		prefix.clear();
		long long int rm = tp % bm;
		for (char c: std::to_string(rm)) {
			prefix.push_back(c - '0');
		}

		if (index < top.size()) {
			prefix.push_back(top[index]);
			index++;
		}

		else {
			for (int i: prefix)
				result.second.push_back(i);
			return result;
		}

		if (index >= top.size()) {
			long long int tp = 0;
			for (auto i: prefix)
				tp = (tp * 10) + i;
			result.first.push_back(tp / bm);

			for (char c: std::to_string(tp % bm))
				result.second.push_back(c - '0');
			return result;
		}
	}

	for (int i: prefix)
		result.second.push_back(i);
	return result;
}

Bignum Bignum::operator/(Bignum b)
{
	vector<int> top = value;
	vector<int> btm = b.value;

	pair<vector<int>, vector<int>> result = divide(top, btm);
	return Bignum(result.first);
}

Bignum Bignum::operator%(Bignum b)
{
	vector<int> top = value;
	vector<int> btm = b.value;

	pair<vector<int>, vector<int>> result = divide(top, btm);
	return Bignum(result.second);
}

Bignum Bignum::gcd(Bignum num1, Bignum num2)
{
	if (num2.value[0] == 0 && num2.size() == 1) {
		if (num1.value[0] == 0 && num1.size() == 1) {
			cout << "Error: Divide by zero." << endl;
			return Bignum(vector<int>(0));
		}
		return num1;
	}
	
	return gcd(num2, num1 % num2);
}

string Bignum::to_string()
{
	string output = "";

	for (int i: value)
		output += std::to_string(i);

	return output;
}

bool Bignum::check(Bignum num)
{
	bool valid = true;

	if (num.get_value()[0] == 0 && num.size() != 1) {
		cout << "Error: bignum cannot contain leading zeros" << endl;
		valid = false;
	}

	for (int i: num.get_value()) {
		if (i < 0 || i > 9) {
			cout << "Error: Value in this bignum is not between 0 and 9" << endl;
			valid = false;
		}
	}

	return valid;
}

int Bignum::size() const
{
	return value.size();
}

const vector<int>& Bignum::get_value() const
{
	return value;
}

void Bignum::print() const
 {
 	for (int i: value)
 		cout << i;
 	cout << "\n";
 }