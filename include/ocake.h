#include <iostream>
#include <vector>
#include <string>
#include "ideal_cipher.h"       // tua implementazione AES-ECB + SHA3
#include "kem.h"                // incapsulazione post-quantum (es. Kyber)
#include "hash.h"               // SHA3-256 come H1, H2

class OcakeClient {
public:
    OcakeClient(const std::string& session_id, const std::string& password)
        : session_id(session_id), password(password) {
        key = H(concat_str(session_id, password));
        cipher = IdealCipher(key);
    }

    // Step 1: riceve Epk cifrato e decifra per ottenere pk
    void receive_encrypted_pk(const std::vector<uint8_t>& Epk) {
        
        pk = this->cipher.decrypt_cpp_int(Epk);
    }

    // Step 2: encapsula e autentica
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> encapsulate_and_auth() {
        auto [c, K] = kem_encapsulate(pk);
        this->c = c;
        this->K = K;

        std::vector<uint8_t> auth_input = concat({session_id, "client", "server", password}, Epk, c, K);
        Auth = H(auth_input);  // H1

        return {c, Auth};
    }

    // Step 3: derivazione chiave finale SK
    std::vector<uint8_t> derive_session_key() {
        std::vector<uint8_t> sk_input = concat({session_id, "client", "server"}, Epk, c, Auth, K);
        return H(sk_input);  // H2
    }

private:
    std::string session_id;
    std::string password;
    std::vector<uint8_t> key;
    IdealCipher cipher;

    std::vector<uint8_t> Epk, pk;
    std::vector<uint8_t> c, K;
    std::vector<uint8_t> Auth;

    std::vector<uint8_t> concat_str(const std::string& a, const std::string& b) {
        std::vector<uint8_t> out(a.begin(), a.end());
        out.insert(out.end(), b.begin(), b.end());
        return out;
    }

    std::vector<uint8_t> concat(std::initializer_list<std::string> strs, const std::vector<uint8_t>& a, const std::vector<uint8_t>& b, const std::vector<uint8_t>& c) {
        std::vector<uint8_t> out;
        for (const auto& s : strs) {
            out.insert(out.end(), s.begin(), s.end());
        }
        out.insert(out.end(), a.begin(), a.end());
        out.insert(out.end(), b.begin(), b.end());
        out.insert(out.end(), c.begin(), c.end());
        return out;
    }
};


class OcakeServer {
    public:
        OcakeServer(const std::string& session_id, const std::string& password)
        : session_id(session_id), password(password) {
        key = H(concat_str(session_id, password));
        cipher = IdealCipher(key);
    }

    // Step 1: riceve Epk cifrato e decifra per ottenere pk
    void receive_encrypted_pk(const std::vector<uint8_t>& Epk) {

        pk = this->cipher.decrypt_cpp_int(Epk);
    }

    // Step 2: encapsula e autentica
    std::pair<std::vector<uint8_t>, std::vector<uint8_t>> encapsulate_and_auth() {
        auto [c, K] = kem_encapsulate(pk);
        this->c = c;
        this->K = K;

        std::vector<uint8_t> auth_input = concat({session_id, "client", "server", password}, Epk, c, K);
        Auth = H(auth_input);  // H1

        return {c, Auth};
    }

    // Step 3: derivazione chiave finale SK
    std::vector<uint8_t> derive_session_key() {
        std::vector<uint8_t> sk_input = concat({session_id, "client", "server"}, Epk, c, Auth, K);
        return H(sk_input);  // H2
    }

private:
    std::string session_id;
    std::string password;
    std::vector<uint8_t> key;
    IdealCipher cipher;

    std::vector<uint8_t> Epk, pk;
    std::vector<uint8_t> c, K;
    std::vector<uint8_t> Auth;

    std::vector<uint8_t> concat_str(const std::string& a, const std::string& b) {
        std::vector<uint8_t> out(a.begin(), a.end());
        out.insert(out.end(), b.begin(), b.end());
        return out;
    }

    std::vector<uint8_t> concat(std::initializer_list<std::string> strs, const std::vector<uint8_t>& a, const std::vector<uint8_t>& b, const std::vector<uint8_t>& c) {
        std::vector<uint8_t> out;
        for (const auto& s : strs) {
            out.insert(out.end(), s.begin(), s.end());
        }
        out.insert(out.end(), a.begin(), a.end());
        out.insert(out.end(), b.begin(), b.end());
        out.insert(out.end(), c.begin(), c.end());
        return out;
    }
}