#ifndef SLT_RENDER_RESOURCE_CACHE_H
#define SLT_RENDER_RESOURCE_CACHE_H

#include <functional>
#include <list>
#include <map>
#include <string>

#include "slt/render/details/resource_ref.h"
#include "slt/string/string_view.h"
namespace slt {
namespace render {
namespace _ {

struct ResourceLoadError : public std::runtime_error {
  ResourceLoadError(std::string const& explanation)
      : std::runtime_error(explanation) {}
};

template <typename RES_T>
class ResourceCache {
 public:
  ~ResourceCache() {
    // SLT_ASSERT(loaded_cache_.empty());
  }

  void remove(RES_T* obj) {
    auto found = reverse_map_.find(obj);
    if(found != reverse_map_.end()) {
      loaded_cache_.erase(found->second);
      reverse_map_.erase(found);
    }
  }

  void add(std::string name, RES_T* obj) {
    SLT_ASSERT(loaded_cache_.end() == loaded_cache_.find(name));
    loaded_cache_.emplace(name, obj);
    reverse_map_.emplace(obj, std::move(name));
  }

  ResourceRef<RES_T> lookup(StringView name) {
    auto found = loaded_cache_.find(name);
    if(found == loaded_cache_.end()) {
      return ResourceRef<RES_T>();
    }

    return ResourceRef<RES_T>(found->second);
  }

 protected:
  std::map<std::string, RES_T*, std::less<>> loaded_cache_;
  std::map<RES_T*, std::string> reverse_map_;
};

template <typename RES_T>
class AsyncLoadingResourceCache : public ResourceCache<RES_T> {
 public:
  using Resource_t = RES_T;
  using LoadArg = typename ResourceRef<RES_T>;
  using FailArg = typename ResourceLoadError;

  template <typename ACTION_T>
  void asyncAdd(StringView name, std::function<void(LoadArg)> cb,
                std::function<void(FailArg)> fail_cb, ACTION_T load_action) {
    SLT_ASSERT(!name.empty());
    SLT_ASSERT(cb);

    auto existing = lookup(name);
    if(existing) {
      cb(existing);
      return;
    }

    auto loading_found = loading_cache_.find(name);
    if(loading_found != loading_cache_.end()) {
      loading_found->second.cb.emplace_back(std::move(cb));
      if(fail_cb) {
        loading_found->second.fail_cb.emplace_back(std::move(fail_cb));
      }
      return;
    }

    PendingResource request;
    request.cb.emplace_back(std::move(cb));
    if(fail_cb) {
      request.fail_cb.emplace_back(std::move(fail_cb));
    }

    loading_cache_.emplace(name.toString(), std::move(request));
    load_action();
  }

  void completePending(StringView name, Resource_t* loaded) {
    SLT_ASSERT(loaded);

    auto arg = LoadArg(loaded);

    auto found = loading_cache_.find(name);
    SLT_ASSERT(found != loading_cache_.end());

    auto emplace_result = loaded_cache_.emplace(name.toString(), loaded);
    loaded->assignCache(this);
    SLT_ASSERT(emplace_result.second);

    for(auto const& cb : found->second.cb) {
      cb(arg);
    }
    loading_cache_.erase(found);
  }

  void failPending(StringView name, FailArg const& reason) {
    auto found = loading_cache_.find(name);
    SLT_ASSERT(found != loading_cache_.end());

    for(auto const& fail_cb : found->second.fail_cb) {
      fail_cb(reason);
    }
    loading_cache_.erase(found);
  }

 private:
  struct PendingResource {
    std::list<std::function<void(LoadArg)>> cb;
    std::list<std::function<void(FailArg)>> fail_cb;
  };

  std::map<std::string, PendingResource, std::less<>> loading_cache_;
};
}
}
}

#endif