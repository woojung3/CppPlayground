#pragma once

#include <memory>            // For std::shared_ptr
#include <nlohmann/json.hpp> // For JSON serialization
#include <optional>
#include <string>

#include "Item.h" // Domain Model Item

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class ItemRepository {
public:
  explicit ItemRepository();

  void saveForBatch(const std::string &key, const Domain::Model::Item &item);
  std::optional<Domain::Model::Item> findById(const std::string &key);
  void deleteById(const std::string &key);

private:
  // Helper to convert string to lowercase
  std::string toLower(std::string s) const;

  // Serialization/Deserialization helpers
  nlohmann::json serializeItem(const Domain::Model::Item &item) const;
  std::optional<Domain::Model::Item>
  deserializeItem(const nlohmann::json &j) const;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame