#pragma once

#include "Item.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

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
  std::string toLower(std::string s) const;

  nlohmann::json serializeItem(const Domain::Model::Item &item) const;
  std::optional<Domain::Model::Item>
  deserializeItem(const nlohmann::json &j) const;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
