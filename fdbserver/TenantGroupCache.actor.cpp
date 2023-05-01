/**
 * TenantGroupCache.actor.cpp
 */

#include "fdbclient/DatabaseContext.h"
#include "fdbserver/TenantGroupCache.h"
#include "flow/actorcompiler.h" // must be last include

class TenantGroupCacheImpl {
public:
	ACTOR static Future<Void> run(TenantGroupCache* self, Database cx) {
		loop {
			loop {
				state Reference<ReadYourWritesTransaction> tr = cx->createTransaction();
				try {
					KeyBackedRangeResult<std::pair<int64_t, TenantMapEntry>> idToTenantMapEntry =
					    wait(TenantMetadata::tenantMap().getRange(tr, {}, {}, CLIENT_KNOBS->MAX_TENANTS_PER_CLUSTER));
					self->idToGroup.clear();
					for (auto const& [id, tenantMapEntry] : idToTenantMapEntry.results) {
						self->idToGroup[id] = tenantMapEntry.tenantGroup;
					}
					break;
				} catch (Error& e) {
					wait(tr->onError(e));
				}
			}
			wait(delay(5.0));
		}
	}
}; // class TenantGroupCacheImpl

Optional<TenantGroupName> TenantGroupCache::getTenantGroup(int64_t tenantId) const {
	auto it = idToGroup.find(tenantId);
	if (it != idToGroup.end()) {
		return it->second;
	} else {
		return {};
	}
}

Future<Void> TenantGroupCache::run(Database cx) {
	return TenantGroupCacheImpl::run(this, cx);
}
