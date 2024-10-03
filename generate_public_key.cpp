#include "crypto_headers.h"

typedef CryptoPP::Integer Integer;
typedef CryptoPP::byte byte;

// Function to perform modular exponentiation
Integer ModExp(const Integer& base, const Integer& exponent, const Integer& modulus) {
    Integer result = 1;
    Integer b = base % modulus;
    Integer e = exponent;
    
    // Exponentiation by squaring
    while (e > 0) {
        if (e.GetBit(0)) {
            result = (result * b) % modulus;
        }
        e >>= 1;
        b = (b * b) % modulus;
    }
    return result;
}

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
    p.Decode(reinterpret_cast<const byte*>(pStr.data()), size);

    // Read q
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string qStr(size, 0);
    file.read(&qStr[0], size);
    q.Decode(reinterpret_cast<const byte*>(qStr.data()), size);

    // Read g
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string gStr(size, 0);
    file.read(&gStr[0], size);
    g.Decode(reinterpret_cast<const byte*>(gStr.data()), size);

    file.close();
}

// Function to load private key from a binary file
void LoadPrivateKey(const std::string& filename, Integer& a) {
    // Open file for binary reading
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    // Read sizes and data
    size_t size;
    
    // Read private key
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string aStr(size, 0);
    file.read(&aStr[0], size);
    a.Decode(reinterpret_cast<const byte*>(aStr.data()), size);

    file.close();
}

// Function to save an integer to a binary file
void SaveIntegerToFile(const std::string& filename, const Integer& a) {
    // Open file for binary writing
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Serialize the Integer to a byte array
    std::string aStr(a.MinEncodedSize(), 0);
    a.Encode(reinterpret_cast<byte*>(&aStr[0]), aStr.size());

    // Write size followed by the data
    size_t aSize = aStr.size();
    file.write(reinterpret_cast<const char*>(&aSize), sizeof(size_t));
    file.write(aStr.data(), aSize);
    file.close();
}

int main(int argc, char* argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <alice|bob>" << std::endl;
        return 1;
    }

    std::string party = argv[1];
    Integer p, q, g, a;

    // Load parameters from the common file
    LoadIntegersFromFile("params.bin", p, q, g);

    if (party == "Alice") {
        // Load Alice's private key and generate her public key
        LoadPrivateKey("privatekeyA.bin", a);
        Integer KA = ModExp(g, a, p);
        SaveIntegerToFile("publicKeyA.bin", KA);
        std::cout << "Alice's public key generated and saved to publicKeyA.bin" << std::endl;
    } else if (party == "Bob") {
        // Load Bob's private key and generate his public key
        LoadPrivateKey("privatekeyB.bin", a);
        Integer KB = ModExp(g, a, p);
        SaveIntegerToFile("publicKeyB.bin", KB);
        std::cout << "Bob's public key generated and saved to publicKeyB.bin" << std::endl;
    } else {
        std::cerr << "Invalid argument. Please specify 'alice' or 'bob'." << std::endl;
        return 1;
    }

    return 0;
}


// g++ -o test generate_public_key.cpp -lcryptopp
// ./test alice
// ./test bob

