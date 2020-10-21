#include <map>

class Bignum
{
	public:
		Bignum();
		Bignum(const Bignum&);
		Bignum(int);
		Bignum(std::string);
		void construct(std::string);
		std::string to_string() const;
		void check(std::string) const;
		bool operator<(const Bignum&) const;
		bool operator>(const Bignum&) const;
		bool operator==(const Bignum&) const;
		bool operator<=(const Bignum&) const;
		bool operator>=(const Bignum&) const;
		bool operator!=(const Bignum&) const;
		Bignum operator+(const Bignum&) const;
		Bignum operator-(const Bignum&) const;
		Bignum operator*(int) const;
		Bignum operator*(const Bignum&) const;
		Bignum operator/(const Bignum&) const;
		Bignum operator%(const Bignum&) const;
		Bignum expmod(Bignum, const Bignum&) const;
		std::pair<Bignum, Bignum> divmod(const Bignum&) const;
		Bignum gcd(const Bignum&) const;
		Bignum encrypt(std::string, std::string) const;
		Bignum decrypt(std::string, std::string) const;
		Bignum two_exp(int) const;
		int get_value(int) const;
		std::vector<int> as_vec() const;
		bool is_even(const Bignum&) const;
		int size() const;
		static bool OPT1;
		static bool OPT2;
		static bool OPT3;
		static bool OPT4;
		static int BASE;
		static Bignum BZero;
		static Bignum BOne;
	private:
		std::vector<int> digits;
		void append_digit(int);
		static std::map<int, Bignum> pow_2; // map that stores powers of 2 to avoid repeated calculations
};
