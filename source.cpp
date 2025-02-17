import <string>;
import <string_view>;
import <vector>;
import <iostream>;
import <vector>;
import <string>;
import <algorithm>;
import <iomanip>;
import <cstdint>;
import <ranges>;
import <cmath>;

class BigInt {
private:
    std::vector<std::uint32_t> digits; // 下位桁から先頭に格納
    //123->[3,2,1]
    static constexpr auto CHUNK_BIT_SIZE = sizeof(decltype(digits)::value_type) * 8;
    static constexpr auto BASE = 1ULL << CHUNK_BIT_SIZE;
public:
    BigInt() = default;
    BigInt(std::string_view decimal) {

        digits.reserve(((decimal.size() - 1) / log10(BASE)) + 1);
        for (std::string remainder_str(decimal); remainder_str.size();) {
            std::uint64_t remainder = 0;
            std::string quotient_str;
            for (const auto &c : remainder_str) {
                remainder = remainder * 10/*//Decimal(10) to 2^32*/ + (c - '0'/*string to decimal*/);
                if (remainder >= BASE) {
                    std::uint64_t quotient_digit = remainder >> CHUNK_BIT_SIZE;//remainder/BASE
                    remainder &= BASE-1;//remainder%=BASE
                    quotient_str += std::to_string(quotient_digit);
                }
                else if (!quotient_str.empty()) {
                    quotient_str += '0';
                }
            }
            digits.push_back(remainder);
            remainder_str = std::move(quotient_str);
        }
    }
    std::string to_string() const {
        decltype(digits) remainder_digits(digits.rbegin(),digits.rend());
        std::string result;
        result.reserve(digits.size() * log10(BASE) + 1);
        for (; remainder_digits.size();) {
            std::uint64_t remainder = 0;
            std::vector<std::uint32_t> new_digits;
            for (const auto &it:remainder_digits) {
                std::uint64_t value = (remainder << CHUNK_BIT_SIZE) + it;
                std::uint32_t quotient_digit =value / 10;
                remainder = value % 10;

                if (new_digits.size() || quotient_digit) {
                    new_digits.push_back(quotient_digit);
                }
            }

            result.push_back('0' + remainder);
            remainder_digits = new_digits;
        }
        std::ranges::reverse(result);
        return result;
    }
    auto operator+(const BigInt& right) {
        BigInt result;
        const auto MAX_SIZE = std::max(digits.size(), right.digits.size());
        result.digits.reserve(MAX_SIZE+1);
        for (auto i = 0, carry = 0; i < MAX_SIZE||carry; ++i) {
            uint64_t sum = carry;
            if (i < digits.size()) sum += digits[i];
            if (i < right.digits.size()) sum += right.digits[i];
            result.digits.push_back(sum & (BASE - 1));
            carry = sum >> CHUNK_BIT_SIZE;
        }
        return result;

    }

};
static BigInt operator""n(const char* c) {
    return BigInt(c);
}
int main() {
    auto a=0n,b=0n;
    std::cout << (a+b).to_string() << std::endl;
	return 0;
}