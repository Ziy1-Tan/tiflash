#include <Core/Types.h>
#include <Poco/Logger.h>
#include <Storages/Page/PageDefines.h>
#include <Storages/Page/PageFile.h>
#include <Storages/Page/PageStorage.h>
#include <Storages/Page/WriteBatch.h>

#include <boost/core/noncopyable.hpp>
#include <tuple>

namespace DB
{

class LegacyCompactor : private boost::noncopyable
{
public:
    LegacyCompactor(const PageStorage & storage);

    std::tuple<PageFileSet, PageFileSet> tryCompact(PageFileSet && page_files, const std::set<PageFileIdAndLevel> & writing_file_ids);

private:
    // Return values: [files to remove, files to compact, compact sequence id]
    std::tuple<PageFileSet, PageFileSet, WriteBatch::SequenceID, std::optional<PageFile>> //
    collectPageFilesToCompact(const PageFileSet & page_files, const std::set<PageFileIdAndLevel> & writing_fiel_ids);

    static WriteBatch prepareCheckpointWriteBatch(const PageStorage::SnapshotPtr snapshot, const WriteBatch::SequenceID wb_sequence);
    static void writeToCheckpoint(const String & storage_path, const PageFileIdAndLevel & file_id, WriteBatch && wb, Poco::Logger * log);

private:
    const String & storage_name;
    const String & storage_path;

    const PageStorage::Config & config;

    Poco::Logger * log;
    Poco::Logger * page_file_log;

    PageStorage::VersionedPageEntries version_set;
    PageStorage::StatisticsInfo       info;
};

} // namespace DB