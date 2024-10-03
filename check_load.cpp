#include "crypto_headers.h"

using namespace CryptoPP;

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

int main() {
    Integer p, q, g;

    // Load from file
    LoadIntegersFromFile("params.bin", p, q, g);

    std::cout << "Integers loaded from file successfully." << std::endl;
    std::cout << "p: " << p << std::endl;
    std::cout << "q: " << q << std::endl;
    std::cout << "g: " << g << std::endl;

    return 0;
}

