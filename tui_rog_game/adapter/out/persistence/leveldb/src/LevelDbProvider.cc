#include "LevelDbProvider.h"
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <spdlog/spdlog.h>

namespace TuiRogGame {
namespace Adapter {
namespace Out {
namespace Persistence {

LevelDbProvider& LevelDbProvider::getInstance() {
    static LevelDbProvider instance;
    return instance;
}

LevelDbProvider::LevelDbProvider() {
    leveldb::DB* db_raw = nullptr;
    leveldb::Options options;
    options.create_if_missing = true;
    const std::string db_path = "game.db"; // Or from config

    leveldb::Status status = leveldb::DB::Open(options, db_path, &db_raw);
    if (status.ok()) {
        db_.reset(db_raw);
        spdlog::info("LevelDbProvider: Successfully opened LevelDB at {}", db_path);
    } else {
        spdlog::error("LevelDbProvider: Failed to open LevelDB at {}: {}", db_path, status.ToString());
        db_ = nullptr;
    }
}

LevelDbProvider::~LevelDbProvider() = default;

std::optional<std::string> LevelDbProvider::Get(const std::string& key) {
    if (!db_) {
        spdlog::error("LevelDbProvider: Cannot Get, DB not open.");
        return std::nullopt;
    }
    std::string value;
    leveldb::Status status = db_->Get(leveldb::ReadOptions(), key, &value);
    if (status.ok()) {
        return value;
    } else {
        if (!status.IsNotFound()) {
             spdlog::error("LevelDbProvider: Failed to Get key '{}': {}", key, status.ToString());
        }
        return std::nullopt;
    }
}

bool LevelDbProvider::Put(const std::string& key, const std::string& value) {
    if (!db_) {
        spdlog::error("LevelDbProvider: Cannot Put, DB not open.");
        return false;
    }
    leveldb::Status status = db_->Put(leveldb::WriteOptions(), key, value);
    if (!status.ok()) {
        spdlog::error("LevelDbProvider: Failed to Put key '{}': {}", key, status.ToString());
    }
    return status.ok();
}

bool LevelDbProvider::Delete(const std::string& key) {
    if (!db_) {
        spdlog::error("LevelDbProvider: Cannot Delete, DB not open.");
        return false;
    }
    leveldb::Status status = db_->Delete(leveldb::WriteOptions(), key);
    if (!status.ok()) {
        spdlog::error("LevelDbProvider: Failed to Delete key '{}': {}", key, status.ToString());
    }
    return status.ok();
}

void LevelDbProvider::startBatch() {
    current_batch_ = std::make_unique<leveldb::WriteBatch>();
}

void LevelDbProvider::addToBatch(const std::string& key, const std::string& value) {
    if (current_batch_) {
        current_batch_->Put(key, value);
    } else {
        spdlog::warn("LevelDbProvider: addToBatch called without starting a batch.");
    }
}

void LevelDbProvider::deleteFromBatch(const std::string& key) {
    if (current_batch_) {
        current_batch_->Delete(key);
    } else {
        spdlog::warn("LevelDbProvider: deleteFromBatch called without starting a batch.");
    }
}

bool LevelDbProvider::commitBatch() {
    if (!db_) {
        spdlog::error("LevelDbProvider: Cannot commit batch, DB not open.");
        return false;
    }
    if (!current_batch_) {
        spdlog::warn("LevelDbProvider: commitBatch called with no active batch.");
        return true; // Nothing to do
    }

    leveldb::Status status = db_->Write(leveldb::WriteOptions(), current_batch_.get());
    current_batch_.reset(); // Clear the batch after commit

    if (!status.ok()) {
        spdlog::error("LevelDbProvider: Failed to commit WriteBatch: {}", status.ToString());
    }
    return status.ok();
}

} // namespace Persistence
} // namespace Out
} // namespace Adapter
} // namespace TuiRogGame
