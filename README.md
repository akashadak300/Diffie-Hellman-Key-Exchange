# Secure Key Exchange via Diffie-Hellman Protocol

This project implements the **Fixed Diffie-Hellman Key Exchange Protocol** to securely establish a shared secret session key between two parties (Alice and Bob) over an insecure communication channel. The implementation avoids the use of external cryptography libraries such as Crypto++, relying only on the `Integer` class for cryptographic operations.

## Features

1. **Prime Number Generation:** Generates large prime numbers and their corresponding generator for the key exchange process.
2. **Private/Public Key Generation:** Generates private keys and public keys for both parties (Alice and Bob).
3. **Certificate Signing & Verification:** Verifies the authenticity of public keys through certificates signed with digital signatures.
4. **Session Key Generation:** Establishes a shared session key using the public keys of both parties and verifies the integrity using `md5sum`.
5. **Security:** Ensures secure key exchange over an insecure channel without exposing private keys.

## Phases of the Protocol

### 1. Setup Phase
- **Objective:** Generate large prime number `p`, and a generator `g` of a subgroup of Zp, whose order is another prime number `q`.
- **Output:** Store the generated parameters (`g`, `p`, `q`) in `params.bin`.

### 2. Private Key Generation Phase
- **Objective:** Generate private keys `α` and `β` for Alice and Bob, and store them in `privateKeyA.bin` and `privateKeyB.bin`, respectively.

### 3. Public Key Generation Phase
- **Objective:** Generate public keys `KA` and `KB` for Alice and Bob, and store them in `publicKeyA.bin` and `publicKeyB.bin`, respectively.

### 4. Certificate Generation Phase
- **Objective:** Sign the Diffie-Hellman public keys of Alice and Bob with their respective certificates for authentication purposes.

### 5. Certificate Verification Phase
- **Objective:** Verify the Diffie-Hellman public key certificates to ensure the authenticity of both Alice and Bob.

### 6. Session Key Generation & Verification Using `md5sum`
- **Objective:** Generate the shared secret session key for both Alice and Bob and verify it using `md5sum`.

## How to Run

1. **Setup Phase:**
   ```bash
   ./setup params.bin 1024 160
   ```

2. **Private Key Generation for Alice and Bob:**
   ```bash
   ./privateKeyGen params.bin privateKeyA.bin
   ./privateKeyGen params.bin privateKeyB.bin
   ```

3. **Public Key Generation for Alice and Bob:**
   ```bash
   ./publicKeyGen params.bin privateKeyA.bin publicKeyA.bin
   ./publicKeyGen params.bin privateKeyB.bin publicKeyB.bin
   ```

4. **Certificate Generation:**
   ```bash
   ./issueCertificate Alice publicKeyA.bin CA_Priv.bin
   ./issueCertificate Bob publicKeyB.bin CA_Priv.bin
   ```

5. **Certificate Verification:**
   ```bash
   ./verifyCertificate certificateA.bin CA_Pub.bin
   ./verifyCertificate certificateB.bin CA_Pub.bin
   ```

6. **Session Key Generation:**
   ```bash
   ./sessionKeyGen publicKeyB.bin privateKeyA.bin SSNKA.bin
   ./sessionKeyGen publicKeyA.bin privateKeyB.bin SSNKB.bin
   ```

7. **Session Key Verification:**
   ```bash
   md5sum SSNKA.bin
   md5sum SSNKB.bin
   ```

## Tools and Technologies Used

- **Language:** C++
- **Algorithms:** Diffie-Hellman Key Exchange, MD5 for session key verification
- **Tools:** `md5sum` for integrity checks
- **Environment:** Linux-based systems (Ubuntu/Fedora)

## Project Workflow

1. **Prime and Generator Generation (Setup):** This phase generates a large prime `p`, a generator `g`, and another prime `q`, which will be used in the key exchange process.
2. **Private Key Generation:** Both Alice and Bob generate their private keys `α` and `β` using the previously generated parameters (`p`, `q`, and `g`).
3. **Public Key Exchange:** Alice and Bob compute their public keys `KA` and `KB` based on their private keys and exchange them securely.
4. **Certificate Signing & Verification:** Both Alice and Bob's public keys are signed using digital certificates and verified for authenticity using the CA's public key.
5. **Session Key Generation:** Using the exchanged public keys and their respective private keys, Alice and Bob compute a shared session key.
6. **Session Key Verification:** Finally, the integrity of the session key is verified using the `md5sum` utility to ensure both parties have the same key.

## Authors

- **Your Name: AKASH ADAK**
- **Institution:** IIIT Allahabad
