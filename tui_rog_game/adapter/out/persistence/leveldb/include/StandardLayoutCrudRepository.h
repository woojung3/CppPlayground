#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_CRUDREPOSITORY_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_CRUDREPOSITORY_H

#include "LevelDbProvider.h" // LevelDbProvider 사용
#include <algorithm>         // For std::transform
#include <cctype>            // For std::tolower
#include <cstring>           // For std::memcpy
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits> // For std::is_standard_layout_v

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

// Helper to convert string to lowercase (원본 코드에 있었으므로 유지)
inline std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

template <typename T, typename Enable = void>
class StandardLayoutCrudRepository; // Forward declaration

// standard_layout 타입에 대한 특수화
template <typename T>
class StandardLayoutCrudRepository<
    T, std::enable_if_t<std::is_standard_layout_v<T>>> {
public:
  static_assert(std::is_standard_layout_v<T>,
                "CrudRepository can only be used with standard layout types.");

  // 생성자는 더 이상 db_를 받지 않습니다.
  StandardLayoutCrudRepository() = default;

  // 즉시 저장 (LevelDbProvider의 Put 사용)
  void save(const std::string &key, const T &entity) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    std::string value(reinterpret_cast<const char *>(&entity), sizeof(T));
    if (!provider.Put(lower_key, value)) {
      spdlog::error("CrudRepository: Failed to save key '{}' immediately.",
                    lower_key);
    } else {
      spdlog::debug("CrudRepository: Saved key '{}' of size {} immediately.",
                    lower_key, sizeof(T));
    }
  }

  // 배치 저장 (LevelDbProvider의 addToBatch 사용)
  void saveForBatch(const std::string &key, const T &entity) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    std::string value(reinterpret_cast<const char *>(&entity), sizeof(T));
    provider.addToBatch(lower_key, value); // Provider의 배치 기능 사용
    spdlog::debug("CrudRepository: Added Put for key '{}' to batch.",
                  lower_key);
  }

  std::optional<T> findById(const std::string &key) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    auto value_str_opt = provider.Get(lower_key);

    if (!value_str_opt) {
      return std::nullopt; // Provider에서 이미 에러 로깅 또는 찾지 못함
    }

    if (value_str_opt->length() == sizeof(T)) {
      T result;
      std::memcpy(&result, value_str_opt->data(), sizeof(T));
      spdlog::debug("CrudRepository: Found key '{}' of size {}", lower_key,
                    sizeof(T));
      return result;
    } else {
      spdlog::error("CrudRepository: Value length mismatch for key '{}'. "
                    "Expected {} bytes, got {} bytes.",
                    lower_key, sizeof(T), value_str_opt->length());
      return std::nullopt;
    }
  }

  void deleteById(const std::string &key) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    if (!provider.Delete(lower_key)) {
      spdlog::error("CrudRepository: Failed to delete key '{}'.", lower_key);
    } else {
      spdlog::debug("CrudRepository: Deleted key '{}'.", lower_key);
    }
  }

  // 배치 삭제 (LevelDbProvider의 deleteFromBatch 사용)
  void deleteForBatch(const std::string &key) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    provider.deleteFromBatch(lower_key);
    spdlog::debug("CrudRepository: Added Delete for key '{}' to batch.",
                  lower_key);
  }

private:
  // db_ 멤버는 더 이상 없습니다.
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_CRUDREPOSITORY_H
