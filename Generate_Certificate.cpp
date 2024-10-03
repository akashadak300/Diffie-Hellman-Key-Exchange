#include "crypto_headers.h"
// Function to convert an Integer to a string representation
std::string IntegerToString(const CryptoPP::Integer& integer) {
    std::ostringstream oss;
    oss << integer; // Convert to decimal string
    return oss.str();
}
`
// Function to load an Integer from a binary file
void LoadIntegersFromFile(const std::string& filename, CryptoPP::Integer& p) {
    // Open the file for binary reading
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    // Read size and data
    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
    std::string pStr(size, 0);
    file.read(&pStr[0], size);
    p.Decode(reinterpret_cast<const CryptoPP::byte*>(pStr.data()), size);

    file.close();
}

// Function to encode data to Base64 format
std::string Base64Encode(const std::string& input) {
    std::string encoded;
    CryptoPP::StringSource ss(input, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encoded)));
    return encoded;
}

// Function to load a DSA private key from a file
void LoadDSAPrivateKey(const std::string& filename, CryptoPP::DSA::PrivateKey& key) {
    try {
        CryptoPP::FileSource file(filename.c_str(), true);
        key.Load(file);
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Error loading DSA private key: " << e.what() << std::endl;
        throw;
    }
}

// Function to generate a certificate for Alice or Bob
std::string GenerateCertificate(const std::string& publicKeyFile, const CryptoPP::DSA::PrivateKey& caPrivKey) {
    // Load the public key from the file
    CryptoPP::Integer p;
    LoadIntegersFromFile(publicKeyFile, p);

    // Prepare the certificate data
    std::string certData = "Signature Algorithm: DSA\nSubject PublicKey:\n" + IntegerToString(p);

    // Hash the certificate data using SHA-256
    std::string hash;
    try {
        CryptoPP::SHA256 hashFunction;
        CryptoPP::StringSource(certData, true, new CryptoPP::HashFilter(hashFunction, new CryptoPP::StringSink(hash)));
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Error hashing certificate data: " << e.what() << std::endl;
        throw;
    }

    // Sign the hash using the DSA private key
    std::string signature;
    try {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::DSA::Signer signer(caPrivKey);
        CryptoPP::StringSource(hash, true, new CryptoPP::SignerFilter(rng, signer, new CryptoPP::StringSink(signature)));
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Error signing hash: " << e.what() << std::endl;
        throw;
    }

    // Prepare the final certificate string
    std::string certificate = certData + "\nSignature:\n" + Base64Encode(signature);

    return certificate;
}

// Function to save the certificate to a file
void SaveCertificate(const std::string& filename, const std::string& certificate) {
    try {
        CryptoPP::FileSink file(filename.c_str());
        file.Put(reinterpret_cast<const CryptoPP::byte*>(certificate.data()), certificate.size());
        file.MessageEnd();
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Error saving certificate: " << e.what() << std::endl;
        throw;
    }
}

int main(int argc, char* argv[]) {
    // Ensure proper usage
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <Alice|Bob>" << std::endl;
        return 1;
    }

    // Determine if the user is Alice or Bob
    std::string user = argv[1];
    std::string publicKeyFile;
    std::string certificateFile;

    if (user == "Alice") {
        publicKeyFile = "publicKeyA.bin";
        certificateFile = "Certificate-A.bin";
    } else if (user == "Bob") {
        publicKeyFile = "publicKeyB.bin";
        certificateFile = "Certificate-B.bin";
    } else {
        std::cerr << "Invalid argument. Please use 'Alice' or 'Bob'." << std::endl;
        return 1;
    }

    try {
        // Load the CA's DSA private key
        CryptoPP::DSA::PrivateKey caDSAPrivKey;
        LoadDSAPrivateKey("CA_Priv.bin", caDSAPrivKey);

        // Generate the certificate for the user
        std::string certificate = GenerateCertificate(publicKeyFile, caDSAPrivKey);

        // Save the certificate to a file
        SaveCertificate(certificateFile, certificate);

        std::cout << "Certificate for " << user << " generated and saved successfully!" << std::endl;
    } catch (const CryptoPP::Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
// g++ -o test Generate_Certificate.cpp -lcryptopp -lpthread
// ./test Alice
// ./test Bob
