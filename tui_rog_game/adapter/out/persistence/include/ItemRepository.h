#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_ITEMREPOSITORY_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_ITEMREPOSITORY_H

#include <string>
#include <optional>
#include <memory> // For std::shared_ptr
#include <leveldb/db.h> // LevelDB header
#include <nlohmann/json.hpp> // For JSON serialization

#include "Item.h" // Domain Model Item

#include <leveldb/write_batch.h> // For leveldb::WriteBatch

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                class ItemRepository {
                public:
                    explicit ItemRepository(std::shared_ptr<leveldb::DB> db);

                    void save(const std::string& key, const Domain::Model::Item& item, leveldb::WriteBatch& batch);
                    std::optional<Domain::Model::Item> findById(const std::string& key);
                    void deleteById(const std::string& key);

                private:
                    std::shared_ptr<leveldb::DB> db_;

                    // Helper to convert string to lowercase
                    std::string toLower(std::string s) const;

                    // Serialization/Deserialization helpers
                    nlohmann::json serializeItem(const Domain::Model::Item& item) const;
                    std::optional<Domain::Model::Item> deserializeItem(const nlohmann::json& j) const;
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_ITEMREPOSITORY_H