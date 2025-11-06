#include "ItemRepository.h"
#include "LevelDbProvider.h"
#include <algorithm> // For std::transform
#include <cctype>    // For std::tolower
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

ItemRepository::ItemRepository() = default;

std::string ItemRepository::toLower(std::string s) const {
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

nlohmann::json
ItemRepository::serializeItem(const Domain::Model::Item &item) const {
  nlohmann::json j;
  j["type"] = static_cast<int>(item.getType()); // Store enum as int
  j["name"] = item.getName();
  return j;
}

std::optional<Domain::Model::Item>
ItemRepository::deserializeItem(const nlohmann::json &j) const {
  try {
    if (j.contains("type") && j.contains("name")) {
      Domain::Model::Item::ItemType type =
          static_cast<Domain::Model::Item::ItemType>(j["type"].get<int>());
      std::string name = j["name"].get<std::string>();
      return Domain::Model::Item(type, name);
    }
    spdlog::error("ItemRepository: JSON missing 'type' or 'name' fields.");
    return std::nullopt;
  } catch (const nlohmann::json::exception &e) {
    spdlog::error("ItemRepository: Failed to deserialize Item JSON: {}",
                  e.what());
    return std::nullopt;
  }
}

void ItemRepository::saveForBatch(const std::string &key,
                                  const Domain::Model::Item &item) {
  std::string lower_key = toLower(key);
  nlohmann::json j = serializeItem(item);
  LevelDbProvider::getInstance().addToBatch(lower_key, j.dump());
  spdlog::debug("ItemRepository: Added Put for key '{}' to batch.", lower_key);
}

std::optional<Domain::Model::Item>
ItemRepository::findById(const std::string &key) {
  auto &provider = LevelDbProvider::getInstance();
  std::string lower_key = toLower(key);
  auto value_str_opt = provider.Get(lower_key);

  if (!value_str_opt) {
    return std::nullopt; // Not found or error already logged by provider
  }

  try {
    nlohmann::json j = nlohmann::json::parse(*value_str_opt);
    return deserializeItem(j);
  } catch (const nlohmann::json::exception &e) {
    spdlog::error("ItemRepository: Failed to parse JSON for key '{}': {}",
                  lower_key, e.what());
    return std::nullopt;
  }
}

void ItemRepository::deleteById(const std::string &key) {
  auto &provider = LevelDbProvider::getInstance();
  std::string lower_key = toLower(key);
  if (provider.Delete(lower_key)) {
    spdlog::debug("ItemRepository: Deleted key '{}'.", lower_key);
  }
}

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame