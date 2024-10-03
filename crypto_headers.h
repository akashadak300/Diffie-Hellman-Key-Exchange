#ifndef CRYPTO_HEADERS_H
#define CRYPTO_HEADERS_H

// Define this before including the MD5 header to suppress the warning
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <iostream>
#include <fstream>
#include <string>
#include <bits/stdc++.h>
#include <stdexcept>
#include <crypto++/cryptlib.h>   // Core Crypto++ classes
#include <crypto++/dsa.h>        // DSA key handling
#include <crypto++/osrng.h>      // Random number generation
#include <crypto++/files.h>      // File operations
#include <crypto++/base64.h>     // Base64 encoding/decoding
#include <crypto++/sha.h>        // SHA hash functions
#include <crypto++/filters.h>    // StringSink and StringSource
#include <crypto++/asn.h>        // ASN.1 encoding and decoding
#include <crypto++/md5.h>        // MD5 hash function
#include <crypto++/hex.h>        // Hex encoding/decoding

#endif // CRYPTO_HEADERS_H
