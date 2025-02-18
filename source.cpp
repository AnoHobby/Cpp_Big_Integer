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
    bool is_negative;
    static constexpr auto CHUNK_BIT_SIZE = sizeof(decltype(digits)::value_type) * 8;
    static constexpr auto BASE = 1ULL << CHUNK_BIT_SIZE;
public:
    BigInt(std::string_view decimal="") {
        if (decimal.empty()) {
            is_negative = false;
            return;
        }
        decimal.remove_prefix(is_negative = decimal.front() == '-');
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
        if (is_negative)result.push_back('-');
        std::ranges::reverse(result);
        return result;
    }
    BigInt operator-(const BigInt& right)const;
    BigInt operator+(const BigInt& right)const;
    auto operator<=>(const BigInt& right)const{
        if (digits == right.digits)return std::strong_ordering::equal;
        else if (is_negative != right.is_negative) {
            return is_negative ?std::strong_ordering::less: std::strong_ordering::greater;
        }
        else if (digits.size() != right.digits.size()) {
            if (digits.size() < right.digits.size()) {
                return is_negative ? std::strong_ordering::greater : std::strong_ordering::less;
            }
            return is_negative ? std::strong_ordering::less : std::strong_ordering::greater;
        }
        for (auto i = digits.size() - 1; i >= 0; --i) {
            if (digits[i] == right.digits[i])continue;
            else if (digits[i] < right.digits[i]) {
                return is_negative ? std::strong_ordering::greater : std::strong_ordering::less;
            }
            return is_negative ? std::strong_ordering::less : std::strong_ordering::greater;
        }
    }
    auto operator-()const{
        BigInt result = *this;
        result.is_negative ^= 1;
        return result;
    }
    auto operator*(const BigInt& right)const {
        BigInt result;
        result.is_negative = is_negative != right.is_negative;
        result.digits = decltype(result.digits)(digits.size() + right.digits.size(),0);
        for (int k = 0; k < result.digits.size()-1; ++k) {
            std::uint64_t sum = 0;
            for (int i = 0; i <= k; ++i) {
                const auto j = k - i;
                if (i < digits.size() && j < right.digits.size()) {
                    sum += static_cast<std::uint64_t>(digits[i]) * right.digits[j];
                }
            }
            if (sum >= BASE) {
                result.digits[k + 1] += sum >>CHUNK_BIT_SIZE;
                sum &= BASE-1;
            }
            result.digits[k] +=sum;
        }
        if (!result.digits.back())result.digits.pop_back();
        return result;
    }
};
BigInt BigInt::operator+(const BigInt& right)const {
    if (is_negative != right.is_negative) {
        if (is_negative) {
            return right - (-*this);
        }
        return *this - (-right);
    }
    BigInt result;
    result.is_negative = is_negative;
    const auto max_size = std::max(digits.size(), right.digits.size());
    result.digits.reserve(max_size + 1/*carry*/);
    for (auto i = 0, carry = 0; i < max_size || carry; ++i) {
        uint64_t sum = carry;
        if (i < digits.size()) sum += digits[i];
        if (i < right.digits.size()) sum += right.digits[i];
        result.digits.push_back(sum & (BASE - 1));
        carry = sum >> CHUNK_BIT_SIZE;
    }
    return result;
}
BigInt BigInt::operator-(const BigInt& right)const {
    if (is_negative != right.is_negative) {
        return *this + (-right);
    }
    BigInt result;
    result.is_negative = *this < right;
    const BigInt& greater = result.is_negative ? right : *this;
    const BigInt& smaller = result.is_negative ? *this : right;

    result.digits.reserve(greater.digits.size());
    for (auto i = 0, borrow = 0; i < greater.digits.size(); ++i) {
        std::int64_t sum = greater.digits[i] - borrow;
        if (i < smaller.digits.size()) {
            sum -= smaller.digits[i];
        }
        if (borrow = (sum < 0)) {
            sum |= CHUNK_BIT_SIZE;
        }
        result.digits.push_back(sum);
    }
    return result;
}
static BigInt operator""_n(const char* c) {
    return BigInt(c);
}

int main() {
    auto a=1000_n,b=20000000_n;
    std::cout <<(a*b).to_string() << std::endl;
    return 0;
}