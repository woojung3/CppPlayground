#include "key_exchange.hpp"
#include "spdlog/fmt/fmt.h"

#include <cstdio>
#include <openssl/crypto.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdexcept>

// 기존 print()와 random_bytes() 구현
namespace autocrypt {

Bytes random_bytes(size_t length) {
  std::vector<unsigned char> buf(length);
  if (RAND_bytes(buf.data(), static_cast<int>(length)) != 1) {
    throw std::runtime_error("난수 생성 실패");
  }
  return buf;
}

// -------------------------------------------------------------------------
// PEM 파일에서 DER 형식으로 공개키 읽기
PublicKey both_get_public_key(const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "r");
  if (!fp) {
    throw std::runtime_error("공개키 파일 열기 실패: " + filename);
  }
  EVP_PKEY *pkey = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
  if (!pkey) {
    // PEM_read_PUBKEY 실패 시, 파일을 인증서로 읽어 공개키 추출
    rewind(fp);
    X509 *cert = PEM_read_X509(fp, nullptr, nullptr, nullptr);
    if (cert) {
      pkey = X509_get_pubkey(cert);
      X509_free(cert);
    }
  }
  fclose(fp);
  if (!pkey) {
    throw std::runtime_error("공개키 파일 읽기 실패: " + filename);
  }

  int len = i2d_PUBKEY(pkey, nullptr);
  if (len <= 0) {
    EVP_PKEY_free(pkey);
    throw std::runtime_error("공개키 DER 인코딩 길이 계산 실패");
  }
  PublicKey der(len);
  unsigned char *p = der.data();
  if (i2d_PUBKEY(pkey, &p) != len) {
    EVP_PKEY_free(pkey);
    throw std::runtime_error("공개키 DER 변환 실패");
  }
  EVP_PKEY_free(pkey);
  return der;
}

// -------------------------------------------------------------------------
// PEM 파일에서 DER 형식으로 개인키 읽기
PrivateKey both_get_private_key(const std::string &filename) {
  FILE *fp = fopen(filename.c_str(), "r");
  if (!fp) {
    throw std::runtime_error("개인키 파일 열기 실패: " + filename);
  }
  EVP_PKEY *pkey = PEM_read_PrivateKey(fp, nullptr, nullptr, nullptr);
  fclose(fp);
  if (!pkey) {
    throw std::runtime_error("개인키 파일 읽기 실패: " + filename);
  }

  int len = i2d_PrivateKey(pkey, nullptr);
  if (len <= 0) {
    EVP_PKEY_free(pkey);
    throw std::runtime_error("개인키 DER 인코딩 길이 계산 실패");
  }
  PrivateKey der(len);
  unsigned char *p = der.data();
  if (i2d_PrivateKey(pkey, &p) != len) {
    EVP_PKEY_free(pkey);
    throw std::runtime_error("개인키 DER 변환 실패");
  }
  EVP_PKEY_free(pkey);
  return der;
}

// -------------------------------------------------------------------------
// 서버에서 임시(Ephemeral) ECDHE 키 쌍 생성 (NIST P-256 사용)
KeyPair server_generate_ephemeral_key() {
  KeyPair kp;

  // 1. EC_KEY 생성 및 키 쌍 생성
  EC_KEY *eckey = EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
  if (!eckey) {
    throw std::runtime_error("EC_KEY 생성 실패");
  }
  if (EC_KEY_generate_key(eckey) != 1) {
    EC_KEY_free(eckey);
    throw std::runtime_error("임시 EC 키 쌍 생성 실패");
  }

  // 2. 생성한 EC_KEY를 EVP_PKEY로 래핑
  EVP_PKEY *pkey = EVP_PKEY_new();
  if (!pkey) {
    EC_KEY_free(eckey);
    throw std::runtime_error("EVP_PKEY 생성 실패");
  }
  if (EVP_PKEY_set1_EC_KEY(pkey, eckey) != 1) {
    EVP_PKEY_free(pkey);
    EC_KEY_free(eckey);
    throw std::runtime_error("EVP_PKEY_set1_EC_KEY 실패");
  }

  // 3. 공개키를 DER 형식으로 인코딩 (i2d_PUBKEY 사용)
  int pub_len = i2d_PUBKEY(pkey, nullptr);
  if (pub_len <= 0) {
    EVP_PKEY_free(pkey);
    EC_KEY_free(eckey);
    throw std::runtime_error("공개키 DER 인코딩 길이 계산 실패");
  }
  kp.public_key.resize(pub_len);
  unsigned char *pub_ptr = kp.public_key.data();
  if (i2d_PUBKEY(pkey, &pub_ptr) != pub_len) {
    EVP_PKEY_free(pkey);
    EC_KEY_free(eckey);
    throw std::runtime_error("공개키 DER 변환 실패");
  }

  // 4. 개인키를 DER 형식으로 인코딩 (i2d_PrivateKey 사용)
  int priv_len = i2d_PrivateKey(pkey, nullptr);
  if (priv_len <= 0) {
    EVP_PKEY_free(pkey);
    EC_KEY_free(eckey);
    throw std::runtime_error("개인키 DER 인코딩 길이 계산 실패");
  }
  kp.private_key.resize(priv_len);
  unsigned char *priv_ptr = kp.private_key.data();
  if (i2d_PrivateKey(pkey, &priv_ptr) != priv_len) {
    EVP_PKEY_free(pkey);
    EC_KEY_free(eckey);
    throw std::runtime_error("개인키 DER 변환 실패");
  }

  EVP_PKEY_free(pkey);
  EC_KEY_free(eckey);
  return kp;
}

// -------------------------------------------------------------------------
// ECDHE: 주어진 개인키와 공개키(DER 형식)를 파싱하여 공유 비밀 계산
Bytes both_ecdhe(const PrivateKey &private_key, const PublicKey &public_key) {
  const unsigned char *p_priv = private_key.data();
  EVP_PKEY *pkey_priv =
      d2i_PrivateKey(EVP_PKEY_EC, nullptr, &p_priv, private_key.size());
  if (!pkey_priv) {
    throw std::runtime_error("개인키 DER 파싱 실패");
  }

  const unsigned char *p_pub = public_key.data();
  EVP_PKEY *pkey_pub = d2i_PUBKEY(nullptr, &p_pub, public_key.size());
  if (!pkey_pub) {
    EVP_PKEY_free(pkey_priv);
    throw std::runtime_error("공개키 DER 파싱 실패");
  }

  EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey_priv, nullptr);
  if (!ctx) {
    EVP_PKEY_free(pkey_priv);
    EVP_PKEY_free(pkey_pub);
    throw std::runtime_error("EVP_PKEY_CTX 생성 실패");
  }
  if (EVP_PKEY_derive_init(ctx) <= 0) {
    EVP_PKEY_free(pkey_priv);
    EVP_PKEY_free(pkey_pub);
    EVP_PKEY_CTX_free(ctx);
    throw std::runtime_error("EVP_PKEY_derive_init 실패");
  }
  if (EVP_PKEY_derive_set_peer(ctx, pkey_pub) <= 0) {
    EVP_PKEY_free(pkey_priv);
    EVP_PKEY_free(pkey_pub);
    EVP_PKEY_CTX_free(ctx);
    throw std::runtime_error("EVP_PKEY_derive_set_peer 실패");
  }

  size_t secret_len = 0;
  if (EVP_PKEY_derive(ctx, nullptr, &secret_len) <= 0) {
    EVP_PKEY_free(pkey_priv);
    EVP_PKEY_free(pkey_pub);
    EVP_PKEY_CTX_free(ctx);
    throw std::runtime_error("공유 비밀 길이 계산 실패");
  }
  Bytes secret(secret_len);
  if (EVP_PKEY_derive(ctx, secret.data(), &secret_len) <= 0) {
    EVP_PKEY_free(pkey_priv);
    EVP_PKEY_free(pkey_pub);
    EVP_PKEY_CTX_free(ctx);
    throw std::runtime_error("공유 비밀 연산 실패");
  }
  secret.resize(secret_len);

  EVP_PKEY_free(pkey_priv);
  EVP_PKEY_free(pkey_pub);
  EVP_PKEY_CTX_free(ctx);
  return secret;
}

// -------------------------------------------------------------------------
// 서버 개인키로 id || ephemeral_public_key 메시지에 대해 서명 (SHA384)
Bytes server_sign(const PrivateKey &server_private_key, const Bytes &id,
                  const PublicKey &ephemeral_public_key) {
  // 메시지: id || ephemeral_public_key
  Bytes message;
  message.reserve(id.size() + ephemeral_public_key.size());
  message.insert(message.end(), id.begin(), id.end());
  message.insert(message.end(), ephemeral_public_key.begin(),
                 ephemeral_public_key.end());

  const unsigned char *p = server_private_key.data();
  EVP_PKEY *pkey =
      d2i_PrivateKey(EVP_PKEY_EC, nullptr, &p, server_private_key.size());
  if (!pkey) {
    throw std::runtime_error("서명용 서버 개인키 DER 파싱 실패");
  }

  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (!mdctx) {
    EVP_PKEY_free(pkey);
    throw std::runtime_error("EVP_MD_CTX 생성 실패");
  }
  if (EVP_DigestSignInit(mdctx, nullptr, EVP_sha384(), nullptr, pkey) <= 0) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    throw std::runtime_error("EVP_DigestSignInit 실패");
  }
  if (EVP_DigestSignUpdate(mdctx, message.data(), message.size()) <= 0) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    throw std::runtime_error("EVP_DigestSignUpdate 실패");
  }
  size_t sig_len = 0;
  if (EVP_DigestSignFinal(mdctx, nullptr, &sig_len) <= 0) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    throw std::runtime_error("서명 길이 계산 실패");
  }
  Bytes signature(sig_len);
  if (EVP_DigestSignFinal(mdctx, signature.data(), &sig_len) <= 0) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    throw std::runtime_error("서명 생성 실패");
  }
  signature.resize(sig_len);

  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);
  return signature;
}

// -------------------------------------------------------------------------
// 서버 공개키로 id || ephemeral_public_key 메시지의 서명 검증 (SHA384)
bool client_verify(const PublicKey &server_public_key, const Bytes &signature,
                   const Bytes &id, const PublicKey &ephemeral_public_key) {
  // 메시지: id || ephemeral_public_key
  Bytes message;
  message.reserve(id.size() + ephemeral_public_key.size());
  message.insert(message.end(), id.begin(), id.end());
  message.insert(message.end(), ephemeral_public_key.begin(),
                 ephemeral_public_key.end());

  const unsigned char *p = server_public_key.data();
  EVP_PKEY *pkey = d2i_PUBKEY(nullptr, &p, server_public_key.size());
  if (!pkey) {
    throw std::runtime_error("서버 공개키 DER 파싱 실패");
  }
  EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
  if (!mdctx) {
    EVP_PKEY_free(pkey);
    throw std::runtime_error("EVP_MD_CTX 생성 실패 (verify)");
  }
  if (EVP_DigestVerifyInit(mdctx, nullptr, EVP_sha384(), nullptr, pkey) <= 0) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    throw std::runtime_error("EVP_DigestVerifyInit 실패");
  }
  if (EVP_DigestVerifyUpdate(mdctx, message.data(), message.size()) <= 0) {
    EVP_MD_CTX_free(mdctx);
    EVP_PKEY_free(pkey);
    throw std::runtime_error("EVP_DigestVerifyUpdate 실패");
  }
  int ret = EVP_DigestVerifyFinal(mdctx, signature.data(), signature.size());
  EVP_MD_CTX_free(mdctx);
  EVP_PKEY_free(pkey);
  return (ret == 1);
}

// -------------------------------------------------------------------------
// Concatenation KDF (NIST SP 800-56A, SHA384)
// 공유비밀 || [0x00 0x00 0x00 0x01] 에 대해 SHA384 해시 후,
// 앞 16바이트를 encrypt_key, 나머지 32바이트를 mac_key로 사용.
SessionKeys both_kdf(const Bytes &shared_secret) {
  SessionKeys keys;
  Bytes input;

  // 1. 카운터 4바이트 (값 1, 빅엔디언)
  uint8_t counter[4] = {0x00, 0x00, 0x00, 0x01};
  input.insert(input.end(), counter, counter + 4);

  // 2. Z (ECDHE 공유 비밀)
  input.insert(input.end(), shared_secret.begin(), shared_secret.end());

  // 3. keydatalen = 384 (384 bits = 0x00000180) 를 4바이트 빅엔디언으로 표현
  uint8_t keydatalen[4] = {0x00, 0x00, 0x01, 0x80};
  input.insert(input.end(), keydatalen, keydatalen + 4);

  // 4. Algorithm ID, ID_U, ID_V 추가 (각각 0x01, 0x55, 0x56)
  input.push_back(0x01);
  input.push_back(0x55);
  input.push_back(0x56);

  // 5. SHA384 해시를 적용 (출력: 384비트 = 48바이트)
  unsigned char hash[SHA384_DIGEST_LENGTH]; // SHA384_DIGEST_LENGTH == 48
  if (!EVP_Digest(input.data(), input.size(), hash, nullptr, EVP_sha384(),
                  nullptr)) {
    throw std::runtime_error("KDF: EVP_Digest 실패");
  }

  // 6. 출력된 48바이트 중 상위 16바이트를 AES session key, 하위 32바이트를 MAC
  // key로 사용
  keys.encrypt_key.assign(hash, hash + 16);
  keys.mac_key.assign(hash + 16, hash + SHA384_DIGEST_LENGTH);
  return keys;
}

// -------------------------------------------------------------------------
// AES-CBC-128 암호화 (PKCS7 padding)
Bytes both_encrypt(const Bytes &encrypt_key, const Bytes &iv,
                   const Bytes &data) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    throw std::runtime_error("both_encrypt: EVP_CIPHER_CTX 생성 실패");
  }
  if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, encrypt_key.data(),
                         iv.data()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_encrypt: EVP_EncryptInit_ex 실패");
  }
  Bytes ciphertext;
  ciphertext.resize(data.size() + EVP_CIPHER_block_size(EVP_aes_128_cbc()));
  int out_len1 = 0;
  if (EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len1, data.data(),
                        data.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_encrypt: EVP_EncryptUpdate 실패");
  }
  int out_len2 = 0;
  if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + out_len1, &out_len2) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_encrypt: EVP_EncryptFinal_ex 실패");
  }
  ciphertext.resize(out_len1 + out_len2);
  EVP_CIPHER_CTX_free(ctx);
  return ciphertext;
}

// -------------------------------------------------------------------------
// HMAC-SHA256: MAC = HMAC(mac_key, iv || data)
Bytes both_mac(const Bytes &mac_key, const Bytes &iv, const Bytes &data) {
  Bytes msg;
  msg.reserve(iv.size() + data.size());
  msg.insert(msg.end(), iv.begin(), iv.end());
  msg.insert(msg.end(), data.begin(), data.end());
  unsigned int len = 0;
  Bytes tag(EVP_MAX_MD_SIZE);
  unsigned char *result = HMAC(EVP_sha256(), mac_key.data(), mac_key.size(),
                               msg.data(), msg.size(), tag.data(), &len);
  if (!result) {
    throw std::runtime_error("both_mac: HMAC 생성 실패");
  }
  tag.resize(len);
  return tag;
}

// -------------------------------------------------------------------------
// AES-CBC-128 복호화 (PKCS7 padding)
Bytes both_decrypt(const Bytes &encrypt_key, const Bytes &iv,
                   const Bytes &cipher) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    throw std::runtime_error("both_decrypt: EVP_CIPHER_CTX 생성 실패");
  }
  if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, encrypt_key.data(),
                         iv.data()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_decrypt: EVP_DecryptInit_ex 실패");
  }
  Bytes plaintext;
  plaintext.resize(cipher.size());
  int out_len1 = 0;
  if (EVP_DecryptUpdate(ctx, plaintext.data(), &out_len1, cipher.data(),
                        cipher.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_decrypt: EVP_DecryptUpdate 실패");
  }
  int out_len2 = 0;
  if (EVP_DecryptFinal_ex(ctx, plaintext.data() + out_len1, &out_len2) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_decrypt: EVP_DecryptFinal_ex 실패");
  }
  plaintext.resize(out_len1 + out_len2);
  EVP_CIPHER_CTX_free(ctx);
  return plaintext;
}

// -------------------------------------------------------------------------
// MAC 검증: 제공된 tag와 HMAC(mac_key, iv || data)를 비교 (constant-time)
bool both_verify_mac(const Bytes &mac_key, const Bytes &iv, const Bytes &data,
                     const Bytes &tag) {
  Bytes computed = both_mac(mac_key, iv, data);
  if (computed.size() != tag.size()) {
    return false;
  }
  // OpenSSL의 constant-time memcmp 사용
  return (CRYPTO_memcmp(computed.data(), tag.data(), tag.size()) == 0);
}

// 입력 평문에 대해 커스텀 패딩을 적용하여 AES 블록(16바이트) 배수로 만든다.
Bytes custom_pad(const Bytes &data) {
  Bytes padded = data;
  size_t L = data.size();
  size_t n = L % 16;
  uint8_t pad_val = static_cast<uint8_t>(15 - n); // 계산: 15 - (L mod 16)
  size_t pad_len = pad_val + 1;                   // 총 추가 바이트 수
  padded.insert(padded.end(), pad_len, pad_val);
  return padded;
}

// 복호화된 평문에서 커스텀 패딩을 제거한다.
Bytes custom_unpad(const Bytes &data) {
  if (data.empty() || (data.size() % 16) != 0) {
    throw std::runtime_error("custom_unpad: 올바르지 않은 데이터 길이");
  }
  uint8_t pad_val = data.back();
  size_t pad_len = pad_val + 1;
  if (pad_len > data.size()) {
    throw std::runtime_error("custom_unpad: 패딩 길이가 데이터 크기보다 큼");
  }
  // 패딩 영역의 모든 바이트가 pad_val인지 확인
  for (size_t i = data.size() - pad_len; i < data.size(); i++) {
    if (data[i] != pad_val) {
      throw std::runtime_error("custom_unpad: 올바르지 않은 패딩 내용");
    }
  }
  Bytes unpadded(data.begin(), data.end() - pad_len);
  return unpadded;
}

// ---------- 커스텀 패딩을 적용한 AES-CBC-128 암호화 (패딩 직접 적용)
// ----------
Bytes both_encrypt_custom(const Bytes &encrypt_key, const Bytes &iv,
                          const Bytes &data) {
  // 1. 평문에 커스텀 패딩 적용
  Bytes padded = custom_pad(data);

  // 2. OpenSSL AES-CBC 암호화 (내부 패딩은 비활성화)
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    throw std::runtime_error("both_encrypt_custom: EVP_CIPHER_CTX 생성 실패");
  }
  if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, encrypt_key.data(),
                         iv.data()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_encrypt_custom: EVP_EncryptInit_ex 실패");
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0); // 내부 패딩 비활성화

  Bytes ciphertext;
  ciphertext.resize(padded.size() + EVP_CIPHER_block_size(EVP_aes_128_cbc()));
  int out_len1 = 0;
  if (EVP_EncryptUpdate(ctx, ciphertext.data(), &out_len1, padded.data(),
                        padded.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_encrypt_custom: EVP_EncryptUpdate 실패");
  }
  int out_len2 = 0;
  if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + out_len1, &out_len2) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_encrypt_custom: EVP_EncryptFinal_ex 실패");
  }
  ciphertext.resize(out_len1 + out_len2);
  EVP_CIPHER_CTX_free(ctx);
  return ciphertext;
}

// ---------- 커스텀 패딩을 적용한 AES-CBC-128 복호화 ----------
Bytes both_decrypt_custom(const Bytes &encrypt_key, const Bytes &iv,
                          const Bytes &cipher) {
  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    throw std::runtime_error("both_decrypt_custom: EVP_CIPHER_CTX 생성 실패");
  }
  if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, encrypt_key.data(),
                         iv.data()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_decrypt_custom: EVP_DecryptInit_ex 실패");
  }
  EVP_CIPHER_CTX_set_padding(ctx, 0); // 내부 패딩 비활성화

  Bytes plaintext;
  plaintext.resize(cipher.size());
  int out_len1 = 0;
  if (EVP_DecryptUpdate(ctx, plaintext.data(), &out_len1, cipher.data(),
                        cipher.size()) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_decrypt_custom: EVP_DecryptUpdate 실패");
  }
  int out_len2 = 0;
  if (EVP_DecryptFinal_ex(ctx, plaintext.data() + out_len1, &out_len2) != 1) {
    EVP_CIPHER_CTX_free(ctx);
    throw std::runtime_error("both_decrypt_custom: EVP_DecryptFinal_ex 실패");
  }
  plaintext.resize(out_len1 + out_len2);
  EVP_CIPHER_CTX_free(ctx);

  // 수동으로 커스텀 패딩 제거
  return custom_unpad(plaintext);
}

} // namespace autocrypt
