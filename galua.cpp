#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#define M 173 // варіант 2, x^173+x^10+x^2+x+1

static char hexChar(int v) {
    if (v < 10)
        return '0' + v;
    return 'a' + (v - 10);
}
static int hexValue(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    if ('a' <= c && c <= 'f')
        return c - 'a' + 10;
    if ('A' <= c && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

class Galois {
    private:
        std::vector<bool> bits;

        void multiplyByXAndReduce() {
            bool needReduction = bits[M - 1];

            for (int i = M - 1; i > 0; --i) {
                bits[i] = bits[i - 1];
            }
            bits[0] = false;

            if (needReduction) {
                bits[10] = bits[10] ^ true;
                bits[2] = bits[2] ^ true;
                bits[1] = bits[1] ^ true;
                bits[0] = bits[0] ^ true;
            }
        }

    public:
        Galois()
            : bits(M, false) {}

        static Galois zero() { return Galois(); }

        static Galois one() {
            Galois result;
            result.bits[0] = true;
            return result;
        }
        Galois(std::initializer_list<bool> values)
            : Galois() {
            int i = 0;
            for (bool val : values) {
                if (i >= M)
                    break;
                bits[i] = val;
                i++;
            }
        }
        Galois operator+(const Galois &other) const {
            Galois result;
            for (int i = 0; i < M; ++i) {
                result.bits[i] = bits[i] ^ other.bits[i];
            }
            return result;
        }

        Galois operator*(const Galois &other) const {
            Galois result;
            Galois temp = *this;

            for (int i = 0; i < M; ++i) {
                if (other.bits[i]) {
                    result = result + temp;
                }
                temp.multiplyByXAndReduce();
            }
            return result;
        }

        void print() const { print("value"); }

        void print(std::string str) const {

            std::cout << str << " = (";
            for (int i = 0; i < M; ++i) {
                std::cout << bits[i];
                if (i + 1 < M)
                    std::cout << ", ";
            }
            std::cout << ")" << std::endl;
        }

        void Print() const { Print("value"); }

        void Print(const std::string &name) const {
            std::string hex;

            for (int i = 0; i < M; i += 4) {
                int v = 0;
                for (int b = 0; b < 4; ++b) {
                    if (i + b < M && bits[i + b])
                        v |= (1 << b);
                }
                hex.push_back(hexChar(v));
            }

            while (hex.size() > 1 && hex.back() == '0')
                hex.pop_back();

            std::reverse(hex.begin(), hex.end());

            std::cout << name << " = " << hex << std::endl;
        }

        static Galois fromHex(const std::string &hex) {
            Galois res = Galois::zero();

            for (char c : hex) {
                int v = hexValue(c);

                for (int b = 3; b >= 0; --b) {
                    res.multiplyByXAndReduce();

                    if ((v >> b) & 1) {
                        res = res + Galois::one();
                    }
                }
            }
            return res;
        }
};

int main() {
    Galois A = Galois::fromHex("792d6ef9416cb2076ac2e6368471fda47e5a95c35208fc2"
                               "77169197530cfec614dcbf69");
    Galois B = Galois::fromHex("56df1366d2e020579e22f67e66b8a4ee397db984f686bfa"
                               "1d0ace4f5727d9cf9b6269d2");

    A.Print("A");
    B.Print("B");
    Galois res = A + B;
    res.Print("A + B");
    res = A * B;
    res.Print("A * B");
    res = A * A;
    res.Print("A^2");
    return 0;
}
