#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <x86intrin.h>

inline uint64_t rdtsc() { return __rdtsc(); }

template <typename Func>
uint64_t measure_cycles(Func f, int iterations = 1000) {
    uint64_t start = rdtsc();
    for (int i = 0; i < iterations; ++i) {
        f();
    }
    uint64_t end = rdtsc();
    return (end - start) / iterations;
}

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

        Galois square() const {
            std::vector<bool> tmp(2 * M, false);

            for (int i = 0; i < M; ++i) {
                if (bits[i]) {
                    tmp[2 * i] = true;
                }
            }
            for (int i = 2 * M - 1; i >= M; --i) {
                if (tmp[i]) {
                    int shift = i - M;

                    tmp[i] = false;
                    tmp[shift + 10] = tmp[shift + 10] ^ true;
                    tmp[shift + 2] = tmp[shift + 2] ^ true;
                    tmp[shift + 1] = tmp[shift + 1] ^ true;
                    tmp[shift + 0] = tmp[shift + 0] ^ true;
                }
            }

            Galois res;
            for (int i = 0; i < M; ++i) {
                res.bits[i] = tmp[i];
            }

            return res;
        }

        Galois operator^(int pow) const {
            if (pow == 0)
                return Galois::one();
            if (pow == 1)
                return *this;
            if (pow == 2)
                return this->square();

            Galois res = Galois::one();
            Galois base = *this;
            int p = pow;
            while (p > 0) {
                if (p & 1)
                    res = res * base;
                base = base.square();
                p >>= 1;
            }
            return res;
        }

        Galois operator^(const std::string &hexExponent) const {
            Galois result = Galois::one();
            Galois base = *this;

            for (char c : hexExponent) {
                int v = hexValue(c);
                for (int b = 3; b >= 0; --b) {
                    result = result.square();
                    if ((v >> b) & 1) {
                        result = result * base;
                    }
                }
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
        Galois Trace() {
            Galois copy = *this;
            Galois res = *this;
            for (int i = 1; i < M; i++) {
                copy = copy.square();
                res = res + copy;
            }
            return res;
        }

        Galois Inverse() const {
            Galois res = Galois::one();
            Galois base = *this;

            for (int i = 0; i < M - 1; ++i) {
                base = base.square();
                res = res * base;
            }
            return res;
        }
};

int main() {
    Galois A = Galois::fromHex("792d6ef9416cb2076ac2e6368471fda47e5a95c35208fc2"
                               "77169197530cfec614dcbf69");
    Galois B = Galois::fromHex("56df1366d2e020579e22f67e66b8a4ee397db984f686bfa"
                               "1d0ace4f5727d9cf9b6269d2");
    std::string exp = "4bca4fb7e3c9fb60d73c99a671842d3ce3e063c65c44a37"
                      "61bc5f28e40594f157bea46";

    A.Print("A");
    B.Print("B");
    Galois res = A + B;
    res.Print("A + B");
    res = A * B;
    res.Print("A * B");
    res = A ^ 2;
    res.Print("A^2");
    res = A ^ exp;
    res.Print("A^C");
    res = A.Trace();
    res.Print("Trace(A)");
    res = A.Inverse();
    res.Print("Inverse(A)");

    std::cout << "\n=========================================\n\n";
    Galois C = Galois::fromHex("4bca4fb7e3c9fb60d73c99a671842d3ce3e063c65c44a37"
                               "61bc5f28e40594f157bea46");
    res = (A + B) * C;
    res.Print("(A + B)*C");
    res = B * C + C * A;
    res.Print("B*C + C*A");

    res = A;
    for (int i = 1; i < M; ++i) { // A^(2^k -1) = (A^(2^(k-1) -1))^2 * A
        res = res.square();
        res = res * A;
    }
    res.Print("A^(2^M - 1)");

    int N = 1000;

    uint64_t add_cycles =
        measure_cycles([&]() { volatile Galois r = A + B; }, N);

    uint64_t mul_cycles =
        measure_cycles([&]() { volatile Galois r = A * B; }, N);

    uint64_t sqr_cycles =
        measure_cycles([&]() { volatile Galois r = A.square(); }, N);

    uint64_t trace_cycles =
        measure_cycles([&]() { volatile Galois r = A.Trace(); }, 100);
    uint64_t inv_cycles =
        measure_cycles([&]() { volatile Galois r = A.Inverse(); }, 10);

    std::cout << "\nAverage CPU cycles:\n";
    std::cout << "Addition:   " << add_cycles << "\n";
    std::cout << "Multiply:   " << mul_cycles << "\n";
    std::cout << "Square:     " << sqr_cycles << "\n";
    std::cout << "Trace:      " << trace_cycles << "\n";
    std::cout << "Inverse:    " << inv_cycles << "\n";

    return 0;
}
