#pragma once

#include "LevelDbProvider.h"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <type_traits>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

inline std::string toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

template <typename T, typename Enable = void>
class StandardLayoutCrudRepository;

template <typename T>
class StandardLayoutCrudRepository<
    T, std::enable_if_t<std::is_standard_layout_v<T>>> {
public:
  static_assert(std::is_standard_layout_v<T>,
                "CrudRepository can only be used with standard layout types.");

  StandardLayoutCrudRepository() = default;

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

  void saveForBatch(const std::string &key, const T &entity) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    std::string value(reinterpret_cast<const char *>(&entity), sizeof(T));
    provider.addToBatch(lower_key, value);
    spdlog::debug("CrudRepository: Added Put for key '{}' to batch.",
                  lower_key);
  }

  std::optional<T> findById(const std::string &key) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    auto value_str_opt = provider.Get(lower_key);

    if (!value_str_opt) {
      return std::nullopt;
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

  void deleteForBatch(const std::string &key) {
    auto &provider = LevelDbProvider::getInstance();
    std::string lower_key = toLower(key);
    provider.deleteFromBatch(lower_key);
    spdlog::debug("CrudRepository: Added Delete for key '{}' to batch.",
                  lower_key);
  }

private:

};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame

