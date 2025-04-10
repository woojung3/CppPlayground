#include "key_exchange.hpp"
#include <gtest/gtest.h>

namespace autocrypt {
TEST(keco, ecdhe) {
  // 통합연계서버(A) 동작
  auto server_public_key = both_get_public_key("keco.pem");
  auto server_private_key = both_get_private_key("keco.key");

  // 1. 임시키쌍 생성
  auto ephemeral_keypair = server_generate_ephemeral_key();

  // 2. B_pub 획득(충전기 인증서)
  auto client_public_key = both_get_public_key("charger.pem");

  // 3. 공유 비밀 생성
  // Z = ECDHE(A_priv, B_pub)
  auto z_server = both_ecdhe(ephemeral_keypair.private_key, client_public_key);

  // 4. 환경부 개인키로 서명
  // S = Sign_환경부개인키(일련번호 + A_pub)
  auto id = random_bytes(16);
  auto s = server_sign(server_private_key, id, ephemeral_keypair.public_key);

  // 5. 데이터 전달

  // 6. 수신 및 환경부 서명 검증
  // Verify_환경부공개키(S, 일련번호 + A_pub)
  ASSERT_TRUE(
      client_verify(server_public_key, s, id, ephemeral_keypair.public_key));

  // 7. 공유 비밀 생성
  // Z = ECDHE(B_priv, A_pub)
  auto client_private_key = both_get_private_key("charger.key");
  auto z_client = both_ecdhe(client_private_key, ephemeral_keypair.public_key);

  ASSERT_EQ(z_server, z_client);

  auto session_keys = both_kdf(z_client);

  Bytes data{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  Bytes iv = random_bytes(16);

  auto cipher = both_encrypt_custom(session_keys.encrypt_key, iv, data);
  auto tag = both_mac(session_keys.mac_key, iv, data);

  auto decrypted = both_decrypt_custom(session_keys.encrypt_key, iv, cipher);
  ASSERT_EQ(data, decrypted);
  ASSERT_TRUE(both_verify_mac(session_keys.mac_key, iv, data, tag));
}

// 이하 기타 테스트

// ─────────────────────────────────────────────────────────────────────────────
//  Test 1. AES-CBC with Custom Padding
// ─────────────────────────────────────────────────────────────────────────────
//
//  고정 AES-128 키와 IV, 평문에 대해 custom_pad 기반 암/복호화 테스트
TEST(TestVectors, AES_CustomPadding) {
  // 예제 키: NIST 예제 벡터의 키 (16바이트)
  Bytes key = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
               0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
  // 예제 IV: 16바이트
  Bytes iv = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
              0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  // 평문: 길이가 5바이트 (예: {0x01,0x02,0x03,0x04,0x05})
  Bytes plaintext = {0x01, 0x02, 0x03, 0x04, 0x05};

  // custom_pad에 의하면, 5 mod 16 = 5, pad_val = 15 - 5 = 10 (0x0A) 및 pad_len
  // = 11. 따라서 padded plaintext = {0x01,0x02,0x03,0x04,0x05} || 11 x {0x0A}.
  Bytes padded_expected = plaintext;
  padded_expected.insert(padded_expected.end(), 11, 0x0A);
  ASSERT_EQ(custom_pad(plaintext), padded_expected);

  // 암호화 및 복호화 수행
  Bytes cipher = both_encrypt_custom(key, iv, plaintext);
  Bytes decrypted = both_decrypt_custom(key, iv, cipher);

  // 복호화 후 custom_unpad에서 패딩이 제거되어 원래의 평문과 일치해야 함.
  ASSERT_EQ(decrypted, plaintext);

  // (추가로, 오프라인 계산된 expected ciphertext와 비교할 수도 있음)
  // Bytes expected_cipher = { ... };
  // ASSERT_EQ(cipher, expected_cipher);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test 2. HMAC-SHA256 (RFC 4231 Test Case 1)
// ─────────────────────────────────────────────────────────────────────────────
//
//  키: 20바이트의 0x0B, 데이터: "Hi There"
TEST(TestVectors, HMAC_SHA256_RFC4231) {
  Bytes key(20, 0x0B);
  Bytes data = {'H', 'i', ' ', 'T', 'h', 'e', 'r', 'e'};
  Bytes iv; // 빈 벡터를 사용하면 HMAC(key, data) 계산

  // RFC 4231 Test Case 1에서 기대하는 HMAC-SHA256 값:
  // expected:
  // 0xb0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7
  Bytes expected_hmac = {0xb0, 0x34, 0x4c, 0x61, 0xd8, 0xdb, 0x38, 0x53,
                         0x5c, 0xa8, 0xaf, 0xce, 0xaf, 0x0b, 0xf1, 0x2b,
                         0x88, 0x1d, 0xc2, 0x00, 0xc9, 0x83, 0x3d, 0xa7,
                         0x26, 0xe9, 0x37, 0x6c, 0x2e, 0x32, 0xcf, 0xf7};

  Bytes hmac = both_mac(key, iv, data);
  ASSERT_EQ(hmac, expected_hmac);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Test 3. Concatenation KDF (NIST SP 800-56A with SHA384)
// ─────────────────────────────────────────────────────────────────────────────
//  입력: shared_secret Z, keydatalen=384, Algorithm ID = 0x01, ID_U = 0x55,
//  ID_V = 0x56. 여기서는 고정된 shared_secret로 32바이트 모두 0x00를 사용하여
//  테스트함.
TEST(TestVectors, KDF_Concatenation) {
  Bytes shared_secret(32, 0x00);
  SessionKeys keys = both_kdf(shared_secret);

  Bytes expected_encrypt_key = {0x1E, 0xB2, 0x66, 0xFB, 0x67, 0x52, 0x23, 0xD2,
                                0xB6, 0xAF, 0xD1, 0xFF, 0x85, 0x8E, 0x70, 0x55};

  Bytes expected_mac_key = {0x7C, 0x67, 0x02, 0xB1, 0xC5, 0xD7, 0xBB, 0xD4,
                            0xF3, 0x0D, 0x5C, 0x61, 0xEA, 0x39, 0xF8, 0xB7,
                            0xD9, 0xE4, 0x42, 0x47, 0x5E, 0x7A, 0x2A, 0xF5,
                            0x8E, 0x3D, 0x23, 0x8A, 0x1C, 0xA8, 0xB9, 0xAF};

  ASSERT_EQ(keys.encrypt_key, expected_encrypt_key);
  ASSERT_EQ(keys.mac_key, expected_mac_key);
}

} // namespace autocrypt
