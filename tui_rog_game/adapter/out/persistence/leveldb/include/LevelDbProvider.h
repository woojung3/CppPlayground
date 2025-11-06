#pragma once

#include <memory>
#include <string>
#include <optional>

// Forward-declare LevelDB types to avoid including the header here.
namespace leveldb {
    class DB;
    class WriteBatch;
}

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

class LevelDbProvider {
public:
    static LevelDbProvider& getInstance();

    // Immediate (non-batched) operations
    std::optional<std::string> Get(const std::string& key);
    bool Put(const std::string& key, const std::string& value);
    bool Delete(const std::string& key);

    // Unit of Work (batched) operations
    void startBatch();
    void addToBatch(const std::string& key, const std::string& value);
    void deleteFromBatch(const std::string& key);
    bool commitBatch();

    LevelDbProvider(const LevelDbProvider&) = delete;
    LevelDbProvider& operator=(const LevelDbProvider&) = delete;
    LevelDbProvider(LevelDbProvider&&) = delete;
    LevelDbProvider& operator=(LevelDbProvider&&) = delete;

private:
    LevelDbProvider();
    ~LevelDbProvider();

    std::shared_ptr<leveldb::DB> db_;
    std::unique_ptr<leveldb::WriteBatch> current_batch_;
};

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
