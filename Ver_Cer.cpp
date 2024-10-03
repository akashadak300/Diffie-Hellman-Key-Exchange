#include "crypto_headers.h"     // ASN.1 encoding and decoding

using namespace CryptoPP;

// Function to load DSA public key from a file
void LoadPublicKey(const std::string& filename, DSA::PublicKey& key) {
    FileSource file(filename.c_str(), true);
    key.Load(file);
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

// Function to base64 decode a string
std::string Base64Decode(const std::string& encoded) {
    std::string decoded;
    StringSource ss(encoded, true, new Base64Decoder(new StringSink(decoded)));
    return decoded;
}

// Function to verify certificate
bool VerifyCertificate(const std::string& certFile, const std::string& caPubKeyFile) {
    try {
        // Load CA public key
        DSA::PublicKey caPublicKey;
        LoadPublicKey(caPubKeyFile, caPublicKey);

        // Read certificate
        std::string certificate = ReadFile(certFile);

        // Extract data and signature
        std::string certData, signature;
        ExtractDataAndSignature(certificate, certData, signature);

        // Base64 decode the signature
        std::string decodedSignature = Base64Decode(signature);

        // Hash the certificate data
        std::string hash;
        SHA256 hashFunction;
        StringSource(certData, true, new HashFilter(hashFunction, new StringSink(hash)));

        // Verify the signature
        DSA::Verifier verifier(caPublicKey);
        bool result = verifier.VerifyMessage(reinterpret_cast<const byte*>(hash.data()), hash.size(),
                                             reinterpret_cast<const byte*>(decodedSignature.data()), decodedSignature.size());

        if (!result) {
            std::cerr << "Signature verification failed." << std::endl;
            return false;
        }

        std::cout << "Certificate verification successful." << std::endl;
        return true;
    }
    catch (const Exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <certificate_file> <ca_pub_key_file>" << std::endl;
        return 1;
    }

    std::string certFile = argv[1];
    std::string caPubKeyFile = argv[2];

    return VerifyCertificate(certFile, caPubKeyFile) ? 0 : 1;
}

// g++ -o test Ver_Cer.cpp -lcryptopp -lpthread
// ./test Certificate-A.bin CA_Pub.bin
// ./test Certificate-B.bin CA_Pub.bin

