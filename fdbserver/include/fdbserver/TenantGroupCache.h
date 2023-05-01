/**
 * TenantGroupCache.h
 */

#pragma once

#include "fdbclient/Tenant.h"
#include "flow/Optional.h"

#include <unordered_map>

// Maintains a mapping of tenantId -> tenantGroup.
// TODO: Possibly merge this class with TenantEntryCache?
class TenantGroupCache {
	friend class TenantGroupCacheImpl;
	std::unordered_map<int64_t, Optional<TenantGroupName>> idToGroup;

public:
	Optional<TenantGroupName> getTenantGroup(int64_t tenantId) const;
	Future<Void> run(Database cx);
};
