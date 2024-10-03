#include "crypto_headers.h"

using namespace CryptoPP;

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

// Function to read certificate file
std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Function to extract data and signature from certificate
void ExtractDataAndSignature(const std::string& certificate, std::string& data, std::string& signature) {
    std::string delimiter = "\nSignature:\n";
    size_t pos = certificate.find(delimiter);
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid certificate format");
    }
    data = certificate.substr(0, pos);
    signature = certificate.substr(pos + delimiter.length());
}

// Function to extract the public key from the given string
Integer ExtractPublicKey(const std::string& input) {
    std::string keyString;

    // Find the part of the input that contains the public key
    std::string::size_type startPos = input.find("Subject PublicKey:");
    if (startPos != std::string::npos) {
        // Move past the label "Subject PublicKey:"
        startPos += std::strlen("Subject PublicKey:");

        // Extract the public key part
        std::string::size_type endPos = input.find_first_of("0123456789", startPos);
        if (endPos != std::string::npos) {
            keyString = input.substr(endPos);

            // Trim any leading or trailing whitespace
            keyString.erase(0, keyString.find_first_not_of(" \n\r\t"));
            keyString.erase(keyString.find_last_not_of(" \n\r\t") + 1);

            // Convert the decimal string to CryptoPP::Integer
            return Integer(keyString.c_str());
        } else {
            throw std::runtime_error("Public key not found in the input string.");
        }
    } else {
        throw std::runtime_error("Public key label not found in the input string.");
    }
}

void LoadPrivateKey(const std::string& filename, Integer& p) {
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

    file.close();
}

void SaveIntegersToFile(const std::string& filename, const Integer& p) {
    // Open file for binary writing
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    // Serialize each Integer to a byte array
    std::string pStr(p.MinEncodedSize(), 0);

    p.Encode(reinterpret_cast<CryptoPP::byte*>(&pStr[0]), pStr.size());

    // Write sizes followed by the data
    size_t pSize = pStr.size();

    file.write(reinterpret_cast<const char*>(&pSize), sizeof(size_t));
    file.write(pStr.data(), pSize);

    file.close();
}


int main(int argc,char* argv[]){
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <certificate_file>" << std::endl;
        return 1;
    }
    std::string certFile = argv[1];
    std::string private_key = argv[2];
    std::string save_SSNK = argv[3];
    Integer p, q, g;
    // Load from file
    LoadIntegersFromFile("params.bin", p, q, g);
    // Read certificate
    std::string certificate = ReadFile(certFile);
        // Extract data and signature
    std::string certData, signature;
    ExtractDataAndSignature(certificate, certData, signature);
    Integer Oth_pub_key=ExtractPublicKey(certData);
    Integer alpha;
    LoadPrivateKey(private_key,alpha);
    Integer SSNK=ModExp(Oth_pub_key,alpha,p);
    SaveIntegersToFile(save_SSNK,SSNK);
}

// g++ -o test SSNK.cpp -lcryptopp -lpthread
// ./test Certificate-A.bin privatekeyA.bin SSNKA.bin
// ./test Certificate-B.bin privatekeyB.bin SSNKB.bin
// md5sum SSNKA.bin
// md5sum SSNKB.bin