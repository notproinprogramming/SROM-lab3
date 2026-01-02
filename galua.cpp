#include <iostream>
#include <string>
#include <vector>

#define M 173 // варіант 2, x^173+x^10+x^2+x+1
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

        void print() const {
            std::cout << "(";
            for (int i = 0; i < M; ++i) {
                std::cout << bits[i];
                if (i + 1 < M)
                    std::cout << ", ";
            }
            std::cout << ")" << std::endl;
        }
        void print(std::string str) const {

            std::cout << str << " = (";
            for (int i = 0; i < M; ++i) {
                std::cout << bits[i];
                if (i + 1 < M)
                    std::cout << ", ";
            }
            std::cout << ")" << std::endl;
        }
};

int main() {
    Galois A = {true, false, true};
    Galois B = {true, true};
    Galois res = A * B;
    res.print("res");
    return 0;
}
