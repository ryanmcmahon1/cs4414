/*
 * Ken's bignum program
 */
#include <iostream>
#include <algorithm>
#include <vector>
#include "bignum.hpp"

std::string ops("+-*/%<>=^");
std::string gcd("gcd");
std::string rsa_n = "9616540267013058477253762977293425063379243458473593816900454019721117570003248808113992652836857529658675570356835067184715201230519907361653795328462699";
std::string rsa_e = "65537";
std::string rsa_d = "4802033916387221748426181350914821072434641827090144975386182740274856853318276518446521844642275539818092186650425384826827514552122318308590929813048801";

bool is_digit(const char value) { return std::isdigit(value); }
bool is_numeric(const std::string& value) { return std::all_of(value.begin(), value.end(), is_digit); }

bool Bignum::OPT1 = false;
bool Bignum::OPT2 = false;
bool Bignum::OPT3 = false;
bool Bignum::OPT4 = false;

int optimized_base = 10000;
int Bignum::BASE = 0;
Bignum Bignum::BZero;
Bignum Bignum::BOne;

void setbase(bool opt)
{
	if (opt) {
		Bignum::BASE = optimized_base;
		Bignum::BZero = Bignum("0");
		Bignum::BOne = Bignum("1");
	}

	else {
		Bignum::BASE = 10;
		Bignum::BZero = Bignum("0");
		Bignum::BOne = Bignum("1");
	}
}

std::string to_numeric(std::string next_line)
{
	std::string nlad("");
	for(auto c: next_line)
	{
		int d = ((int)c & 0xFF);
		nlad += (d/100)%10 + '0';
		nlad += (d/10)%10 + '0';
		nlad += d%10 + '0';
	}
	return nlad;
}

std::string to_chars(Bignum to_numeric)
{
	std::string nlas("");
	int d_to_go = to_numeric.as_vec().size()%3;
	if(d_to_go == 0)
		d_to_go = 3;
	int index = 0;
	int next_d = 0;
	int size = (to_numeric.as_vec().size()+2)/3;
	for(int n = 0; n < size; n++)
	{
		do
		{
			next_d = next_d*Bignum::BASE + to_numeric.as_vec()[index++];
		}
		while(--d_to_go);
		nlas += (char)next_d;
		next_d = 0;
		d_to_go = 3;
	}
	return nlas;
}

std::string print_encrypt(std::string s) {
	std::cout << "string = " << s << std::endl;
	std::string numeric = to_numeric(s);
	std::cout << "numeric = " << numeric << std::endl;
	Bignum yeysh = Bignum(numeric);
	
	std::cout << "Bignum represented as vector: ";
	for (auto i: yeysh.as_vec()) {
		std::cout << i << std::endl;
	}
	std::cout << "bignum represented as string:\n" << yeysh.to_string() << std::endl;

	Bignum result = yeysh.encrypt(rsa_n, rsa_e);
	std::cout << "result represented as vector: ";
	for (auto i: result.as_vec()) {
		std::cout << i << std::endl;
	}
	std::cout << "result represented as string:\n" << result.to_string() << std::endl;
	return result.to_string();
}

int main(int argc, char **argv)
{
	std::string op;
	std::string opts;
	std::string num1;
	std::string num2;
	std::string num3;

	if(argc != 5 && (argc != 6 || *argv[2] != '^') && (argc != 3 || (*argv[2] != 'e' && *argv[2] != 'd')))
	{
		std::cout << "Run as bignum -op number1 number2, or bignum -help, where:\n	op is one of + - * / %% gcd encrypt decrypt\n	number1 and number2 are positive integers of arbitrary length\n" << std::endl;
		return 0;
	}
	op = std::string(argv[2]);
	if(op[0] != 'd' && op[0] != 'e')
	{
		num1 = std::string(argv[3]);
		if(!is_numeric(num1))
		{
			std::cout << "Error: " << num1 << " is not an unsigned integer.\n" << std::endl;
			return 1;
		}
		num2 = std::string(argv[4]);
		if(!is_numeric(num2))
		{
			std::cout << "Error: " << num2 << " is not an unsigned integer.\n" << std::endl;
			return 1;
		}
		if(argc == 6)
		{
			num3 = std::string(argv[5]);
			if(!is_numeric(num3))
			{
				std::cout << "Error: " << num3 << " is not an unsigned integer.\n" << std::endl;
				return 1;
			}
		}
		else if(op[0] == '^')
		{
			std::cout << "Error: Modular exponentiation requires an exponent and a modulus!\n" << std::endl;
			return 1;
		}
	}
	opts = std::string(argv[1]);
	// Bignum dummy = Bignum();
	for (char c: opts) {
		if (c == '1')
			Bignum::OPT1 = true;
		else if (c == '2')
			Bignum::OPT2 = true;
		else if (c == '3')
			Bignum::OPT3 = true;
		else if (c == '4')
			Bignum::OPT4 = true;
	}
	setbase(Bignum::OPT4);
	// Bignum b1 = Bignum(num1);
	// Bignum b2 = Bignum(num2);
	// // // for (auto i: b1.as_vec()) {
	// // // 	std::cout << i << std::endl;
	// // // }
	// // Bignum result = b1 * b2;
	// // std::cout << "result: ";
	// // for (auto i: result.as_vec()) {
	// // 	std::cout << i << std::endl;
	// // }

	// // std::cout << "result as string: ";
	// // for (auto i: result.to_string()) {
	// // 	std::cout << i << std::endl;
	// // }
	// // std::cout << b1.to_string();
	// std::cout << (b1/b2).to_string() << std::endl;
	// return 0;
	int counter = 3;
	switch(op[0])
	{
		case '+':
			std::cout << (Bignum(num1)+Bignum(num2)).to_string() << std::endl;
			return 0;
		case '-':
			std::cout << (Bignum(num1)-Bignum(num2)).to_string() << std::endl;
			return 0;
		case '*':
			std::cout << (Bignum(num1)*Bignum(num2)).to_string() << std::endl;
			return 0;
		case '/':
			std::cout << (Bignum(num1)/Bignum(num2)).to_string() << std::endl;
			return 0;
		case '%':
			std::cout << (Bignum(num1)%Bignum(num2)).to_string() << std::endl;
			return 0;
		case '<':
			std::cout << (Bignum(num1)<Bignum(num2)) << std::endl;
			return 0;
		case '>':
			std::cout << (Bignum(num1)>Bignum(num2)) << std::endl;
			return 0;
		case '=':
			std::cout << (Bignum(num1)==Bignum(num2)) << std::endl;
			return 0;
		case '^':
			std::cout << Bignum(num1).expmod(num2, num3).to_string() << std::endl;
			return 0;
		case 'g':
			std::cout << (Bignum(num1).gcd(Bignum(num2))).to_string() << std::endl;
			return 0;
		case 'e':
			while(!std::cin.eof())
			{
				std::string next_line;
				std::getline(std::cin, next_line);
				// Many famous movie lines are too long to encrypt in one chunk, so we do all of them as two lines
				// A line too long for two-chunk encryption is truncated (this only affects one line)
				// if(next_line.size()*3 > rsa_n.size())
				// {
				// 	std::cout << Bignum(to_numeric(next_line.substr(0, rsa_n.size()/3))).encrypt(rsa_n, rsa_e).to_string() << std::endl;
				// 	std::cout << Bignum(to_numeric(next_line.substr(rsa_n.size()/3, rsa_n.size()/3))).encrypt(rsa_n, rsa_e).to_string() << std::endl;
				// }
				// else  // Fits on one line
				// {
				// 	std::cout << Bignum(to_numeric(next_line)).encrypt(rsa_n, rsa_e).to_string() << std::endl;
				// 	std::cout << Bignum(++counter).encrypt(rsa_n, rsa_e).to_string() << std::endl;
				// }
				if(next_line.size()*3 > rsa_n.size())
				{
					// print_encrypt(next_line.substr(0, rsa_n.size()/3));
					// print_encrypt(next_line.substr(rsa_n.size()/3, rsa_n.size()/3));

					std::cout << Bignum(to_numeric(next_line.substr(0, rsa_n.size()/3))).encrypt(rsa_n, rsa_e).to_string() << std::endl;
					std::cout << Bignum(to_numeric(next_line.substr(rsa_n.size()/3, rsa_n.size()/3))).encrypt(rsa_n, rsa_e).to_string() << std::endl;
				}
				else  // Fits on one line
				{
					// print_encrypt(next_line);
					// print_encrypt(++counter);
					std::cout << Bignum(to_numeric(next_line)).encrypt(rsa_n, rsa_e).to_string() << std::endl;
					std::cout << Bignum(++counter).encrypt(rsa_n, rsa_e).to_string() << std::endl;
				}
			}
			return 0;
		case 'd':
			while(!std::cin.eof())
			{
				std::string part1, part2;
				std::getline(std::cin, part1);
				std::getline(std::cin, part2);

				// Bignum p1 = Bignum(part1);
				// std::cout << "Bignum1 represented as vector: ";
				// for (auto i: p1.as_vec())
				// 	std::cout << i << std::endl;
				// std::cout << "bignum1 represented as string:\n" << p1.to_string() << std::endl;

				// Bignum p2 = Bignum(part2);
				// std::cout << "Bignum2 represented as vector: ";
				// for (auto i: p2.as_vec())
				// 	std::cout << i << std::endl;
				// std::cout << "bignum1 represented as string:\n" << p2.to_string() << std::endl;

				// Bignum res1 = p1.decrypt(rsa_n, rsa_d);
				// std::cout << "res1 represented as vector: ";
				// for (auto i: res1.as_vec())
				// 	std::cout << i << std::endl;
				// std::cout << "res1 represented as string:\n" << res1.to_string() << std::endl;
				// std::cout << "to chars output for line 1:\n" << to_chars(res1.to_string());

				std::cout << to_chars(Bignum(part1).decrypt(rsa_n, rsa_d).to_string());
				Bignum p2 = Bignum(part2).decrypt(rsa_n, rsa_d);
				if(p2 > Bignum("100000"))
				{
					std::cout << "to chars output for line 1\n" << to_chars(p2.to_string());
				}
				std::cout << std::endl;
			}
			return 0;
		default:
			std::cout << "Error: " << op << " is not a supported operator\n" << std::endl;
			return 1;
	}

}


