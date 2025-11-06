#ifndef TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_ENEMYREPOSITORY_H
#define TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_ENEMYREPOSITORY_H

#include <string>
#include <optional>
#include <memory> // For std::shared_ptr
#include <nlohmann/json.hpp> // For JSON serialization

#include "Enemy.h" // Domain Model Enemy
#include "Stats.h" // For Enemy's Stats
#include "Position.h" // For Enemy's Position

namespace TuiRogGame {
    namespace Adapter {
        namespace Out {
            namespace Persistence {

                class EnemyRepository {
                public:
                    explicit EnemyRepository();

                    void saveForBatch(const std::string& key, const Domain::Model::Enemy& enemy);
                    std::optional<Domain::Model::Enemy> findById(const std::string& key);
                    void deleteById(const std::string& key);

                private:
                    // Helper to convert string to lowercase
                    std::string toLower(std::string s) const;

                    // Serialization/Deserialization helpers
                    nlohmann::json serializeEnemy(const Domain::Model::Enemy& enemy) const;
                    std::optional<Domain::Model::Enemy> deserializeEnemy(const nlohmann::json& j) const;
                };

            } // namespace Persistence
        } // namespace Out
    } // namespace Adapter
} // namespace TuiRogGame

#endif // TUI_ROG_GAME_ADAPTER_OUT_PERSISTENCE_ENEMYREPOSITORY_H