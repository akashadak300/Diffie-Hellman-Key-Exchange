#include "crypto_headers.h"

using namespace CryptoPP;
using namespace std;
// Function to perform modular exponentiation
Integer ModExp(const Integer& base, const Integer& exponent, const Integer& modulus) {
    Integer result = 1;
    Integer b = base % modulus;
    Integer e = exponent;
    
    while (e > 0) {
        if (e.GetBit(0)) {
            result = (result * b) % modulus;
        }
        e >>= 1;
        b = (b * b) % modulus;
    }
    return result;
}

// Function to perform Miller-Rabin primality test
bool IsProbablePrime(const Integer& n, RandomNumberGenerator& rng, unsigned int rounds = 10) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    Integer d = n - 1;
    Integer s = 0;
    while (d % 2 == 0) {
        d >>= 1;
        s += 1;
    }

    for (unsigned int i = 0; i < rounds; i++) {
        Integer a;
        a.Randomize(rng, 1, n - 1);  // Random number in range [1, n-1]

        Integer x = ModExp(a, d, n);
        if (x == 1 || x == n - 1) continue;

        bool continueLoop = false;
        for (Integer r = 0; r < s; r++) {
            x = ModExp(x, Integer(2), n);
            if (x == n - 1) {
                continueLoop = true;
                break;
            }
        }

        if (!continueLoop) return false;
    }

    return true;
}

// Function to generate a prime number with the specified number of bits
Integer GeneratePrime(size_t numBits) {
    AutoSeededRandomPool prng;  // Random number generator

    Integer prime;
    while (true) {
        // Generate a random number with the desired number of bits
        prime.Randomize(prng, numBits / 8);

        // Ensure the number is within the desired bit length
        prime |= (Integer(1) << (numBits - 1)); // Set the most significant bit (MSB)

        // Ensure the number is odd
        if (prime.GetBit(0) == 0) {
            prime += 1; // Make it odd by adding 1
        }

        // Check if the generated number is prime
        if (IsProbablePrime(prime, prng)) {
            return prime;
        }
    }
}

// Function to generate a prime p where p - 1 is divisible by q
Integer GeneratePrimeWithCondition(size_t numBits, const Integer& q, RandomNumberGenerator& rng) {
    Integer k;
    Integer p;
    size_t kBits = numBits - q.BitCount(); // Estimate bits needed for k
    AutoSeededRandomPool prng; 
    while (true) {
        k.Randomize(rng, kBits / 8);
        k |= (Integer(1) << (kBits - 1)); // Ensure k has enough bits
        
        p = k * q + 1;

        // Ensure p has the correct number of bits
        if (IsProbablePrime(p,prng,20)) {
            return p;
        }
    }
}

// Function to find a generator of a cyclic group of order q
Integer FindGenerator(const Integer& q, const Integer& p) {
    Integer candidate;
    AutoSeededRandomPool rng; 
    int iterate=100;
    while (iterate--) {
        // Generate a random candidate in the range [1, q-1]
        candidate.Randomize(rng, 2, p-1);

        // Check if result is 1, which means the candidate is a generator
        if (ModExp(candidate,(p-1)/q,p) != Integer(1)) {
            return candidate;
        }
    }

    return candidate;
}

// Function to save integers to a binary file
void SaveIntegersToFile(const std::string& filename, const Integer& p, const Integer& q, const Integer& g) {
    // Open file for binary writing
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Serialize each Integer to a byte array
    std::string pStr(p.MinEncodedSize(), 0);
    std::string qStr(q.MinEncodedSize(), 0);
    std::string gStr(g.MinEncodedSize(), 0);

    p.Encode(reinterpret_cast<CryptoPP::byte*>(&pStr[0]), pStr.size());
    q.Encode(reinterpret_cast<CryptoPP::byte*>(&qStr[0]), qStr.size());
    g.Encode(reinterpret_cast<CryptoPP::byte*>(&gStr[0]), gStr.size());

    // Write sizes followed by the data
    size_t pSize = pStr.size();
    size_t qSize = qStr.size();
    size_t gSize = gStr.size();

    file.write(reinterpret_cast<const char*>(&pSize), sizeof(size_t));
    file.write(pStr.data(), pSize);

    file.write(reinterpret_cast<const char*>(&qSize), sizeof(size_t));
    file.write(qStr.data(), qSize);

    file.write(reinterpret_cast<const char*>(&gSize), sizeof(size_t));
    file.write(gStr.data(), gSize);

    file.close();
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <number_of_bits>" << std::endl;
        return 1;
    }

    size_t numBitsP = std::stoi(argv[1]); // Convert the argument to an integer
    size_t numBitsQ = std::stoi(argv[2]); 
    
    try {
    
        AutoSeededRandomPool rng; 
        Integer q = GeneratePrime(numBitsQ);
        Integer p = GeneratePrimeWithCondition(numBitsP, q, rng);
        Integer g = FindGenerator(q,p);
        cout<<p<<'\n';
        cout<<q<<'\n';
        cout<<g<<'\n';
        // Save to file
    SaveIntegersToFile("params.bin", p, q, g);
    } catch (const Exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


// g++ -o test generate_params.cpp -lcryptopp
//  ./test 1024 160