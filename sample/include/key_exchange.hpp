#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace autocrypt {

// 키 자료형을 바이트 배열로 표현 (필요에 따라 클래스나 별도 타입을 정의할 수도
// 있습니다)
using Bytes = std::vector<uint8_t>;
using PublicKey = Bytes;
using PrivateKey = Bytes;

/**
 * @brief 공개키/개인키 쌍을 나타내는 구조체.
 */
struct KeyPair {
  PublicKey public_key;   ///< 공개키
  PrivateKey private_key; ///< 개인키
};

/**
 * @brief 세션 키 (암호화 키와 MAC 키)를 나타내는 구조체.
 */
struct SessionKeys {
  Bytes encrypt_key; ///< 암호화에 사용되는 키
  Bytes mac_key;     ///< MAC 생성에 사용되는 키
};

/**
 * @brief 지정된 파일에서 공개키를 읽어 반환합니다.
 *
 * @param filename 공개키 PEM 파일 이름.
 * @return PublicKey 파일로부터 읽어낸 공개키 (바이트 배열).
 */
PublicKey both_get_public_key(const std::string &filename);

/**
 * @brief 지정된 파일에서 개인키를 읽어 반환합니다.
 *
 * @param filename 개인키 PEM 파일 이름.
 * @return PrivateKey 파일로부터 읽어낸 개인키 (바이트 배열).
 */
PrivateKey both_get_private_key(const std::string &filename);

/**
 * @brief 서버에서 임시(Ephemeral) ECDHE 키 쌍을 생성합니다.
 *
 * @return KeyPair 생성된 임시 키 쌍.
 */
KeyPair server_generate_ephemeral_key();

/**
 * @brief ECDHE 키 교환을 수행하여 공유 비밀을 계산합니다.
 *
 * @param private_key 자신의 개인키.
 * @param public_key 상대방의 공개키.
 * @return Bytes 계산된 공유 비밀 (바이트 배열).
 */
Bytes both_ecdhe(const PrivateKey &private_key, const PublicKey &public_key);

/**
 * @brief 서버의 개인키를 이용하여 서명을 생성합니다.
 * 서명할 메시지는 id || ephemeral_public_key 입니다(concatenate)
 *
 * @param server_private_key 서명에 사용할 서버의 개인키 (환경부 개인키 등).
 * @param id 식별자 (예: 일련번호 등, 서명 메시지의 일부로 사용).
 * @param ephemeral_public_key 임시 키 쌍의 공개키 (서명할 데이터의 일부).
 * @return Bytes 생성된 서명 (바이트 배열).
 */
Bytes server_sign(const PrivateKey &server_private_key, const Bytes &id,
                  const PublicKey &ephemeral_public_key);

/**
 * @brief 서버의 공개키를 이용하여 서명의 유효성을 검증합니다.
 * 검증할 메시지는 id || ephemeral_public_key 입니다(concatenate)
 *
 * @param server_public_key 서버의 공개키.
 * @param signature 검증할 서명 (바이트 배열).
 * @param id 식별자 (예: 일련번호 등, 검증 메시지의 일부로 사용).
 * @param ephemeral_public_key 임시 키 쌍의 공개키 (검증할 메시지의 일부).
 * @return true 서명이 올바른 경우.
 * @return false 서명이 올바르지 않은 경우.
 */
bool client_verify(const PublicKey &server_public_key, const Bytes &signature,
                   const Bytes &id, const PublicKey &ephemeral_public_key);

/**
 * @brief 공유 비밀을 기반으로 세션 키들을 파생시킵니다.
 * Concatenation KDF as per NIST SP 800-56A with SHA384
 *
 * @param shared_secret ECDHE를 통해 얻은 공유 비밀.
 * @return SessionKeys 파생된 세션 키 (암호화 키와 MAC 키).
 */
SessionKeys both_kdf(const Bytes &shared_secret);

/**
 * @brief 지정된 암호화 키와 IV를 사용하여 데이터를 암호화합니다.
 * AES-CBC-128 as per NIST 800-38A
 *
 * @param encrypt_key 암호화에 사용되는 키.
 * @param iv 초기화 벡터.
 * @param data 암호화할 평문 데이터.
 * @return 암호문
 */
Bytes both_encrypt(const Bytes &encrypt_key, const Bytes &iv,
                   const Bytes &data);

/**
 * @brief 지정된 MAC 키와 IV를 사용하여 데이터의 메시지 인증 코드를 생성합니다.
 * HMAC-SHA256 as per NIST 800-38A
 *
 * @param mac_key MAC 생성에 사용되는 키.
 * @param iv 초기화 벡터.
 * @param data MAC을 생성할 데이터.
 * @return MAC 태그
 */
Bytes both_mac(const Bytes &mac_key, const Bytes &iv, const Bytes &data);

/**
 * @brief 지정된 암호화 키와 IV를 사용하여 데이터를 복호화합니다.
 *
 * @param encrypt_key 복호화에 사용되는 키.
 * @param iv 초기화 벡터.
 * @param data 암호화할 평문 데이터.
 * @return 평문
 */
Bytes both_decrypt(const Bytes &encrypt_key, const Bytes &iv,
                   const Bytes &cipher);

/**
 * @brief 지정된 MAC 키와 IV, 태그(인증 코드)를 사용하여 데이터의 메시지 인증
 * 코드를 검증합니다.
 *
 * @param mac_key MAC 생성에 사용되는 키.
 * @param iv 초기화 벡터.
 * @param data MAC을 생성할 데이터.
 * @return MAC 태그(인증 코드)
 */
bool both_verify_mac(const Bytes &mac_key, const Bytes &iv, const Bytes &data,
                     const Bytes &tag);

/**
 * 기본적으로 both_encrypt/both_decrypt는 PKCS#7 패딩을 사용함
 * KECO 패딩 요건에 정확히 맞추려면 하기 함수를 사용할 것.
 */
Bytes custom_pad(const Bytes &data);
Bytes custom_unpad(const Bytes &data);

Bytes both_encrypt_custom(const Bytes &encrypt_key, const Bytes &iv,
                          const Bytes &data);

Bytes both_decrypt_custom(const Bytes &encrypt_key, const Bytes &iv,
                          const Bytes &cipher);

Bytes random_bytes(size_t length);

} // namespace autocrypt
