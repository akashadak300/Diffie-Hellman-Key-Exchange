#include "crypto_headers.h"

using namespace CryptoPP;
using namespace std;

// Function to load integers from a binary file
void LoadIntegersFromFile(const std::string& filename, Integer& p, Integer& q, Integer& g) {
    // Open file for binary reading
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    // Read sizes and data
    size_t size;
    
    // Read p
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string pStr(size, 0);
    file.read(&pStr[0], size);
    p.Decode(reinterpret_cast<const CryptoPP::byte*>(pStr.data()), size);

    // Read q
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string qStr(size, 0);
    file.read(&qStr[0], size);
    q.Decode(reinterpret_cast<const CryptoPP::byte*>(qStr.data()), size);

    // Read g
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string gStr(size, 0);
    file.read(&gStr[0], size);
    g.Decode(reinterpret_cast<const CryptoPP::byte*>(gStr.data()), size);

    file.close();
}

// Function to save an integer to a binary file
void SaveIntegersToFile(const std::string& filename, const Integer& a) {
    // Open file for binary writing
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Serialize the Integer to a byte array
    std::string aStr(a.MinEncodedSize(), 0);
    a.Encode(reinterpret_cast<CryptoPP::byte*>(&aStr[0]), aStr.size());

    // Write the size followed by the data
    size_t aSize = aStr.size();
    file.write(reinterpret_cast<const char*>(&aSize), sizeof(size_t));
    file.write(aStr.data(), aSize);

    file.close();
}

int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Alice/Bob>" << std::endl;
        return 1;
    }

    // Load global parameters from the binary file
    Integer p, q, g;
    LoadIntegersFromFile("params.bin", p, q, g);

    // Initialize random number generator
    AutoSeededRandomPool rng;

    // Generate private key in the range [1, q-1]
    Integer privateKey;
    privateKey.Randomize(rng, 1, q - 1);

    // Save the private key to the appropriate file based on the argument
    std::string filename;
    if (std::string(argv[1]) == "Alice") {
        filename = "privatekeyA.bin";
    } else if (std::string(argv[1]) == "Bob") {
        filename = "privatekeyB.bin";
    } else {
        std::cerr << "Invalid argument. Please use 'Alice' or 'Bob'." << std::endl;
        return 1;
    }

    SaveIntegersToFile(filename, privateKey);
    std::cout << "Private key generated and saved to " << filename << std::endl;

    return 0;
}

// g++ -o test generate_private_key.cpp -lcryptopp
// ./test Alice
// ./test Bob
