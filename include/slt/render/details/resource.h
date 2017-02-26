#ifndef SLT_RENDER_RESOURCE_H
#define SLT_RENDER_RESOURCE_H

#include <functional>
#include <list>
#include <map>
#include <string>

#include "slt/render/details/resource_cache.h"
#include "slt/string/string_view.h"

namespace slt {
namespace render {
class ResourceManager;

namespace _ {
// Render resources are intrusively ref-counted constructs that maintain a
// copy of their underlying handles inside of the references. This allows
// for acessing the resource directly from the ref-counting reference
// without any dereferencing.
template <typename RES_T, typename IMPL_T>
struct Resource {
  using handle_t = RES_T;
  handle_t handle_;

  void assignCache(ResourceCache<IMPL_T>* cache) {
    cache_ = cache;
  }

 protected:
  Resource() {}
  ~Resource() {
    if(cache_) {
      cache_->remove(static_cast<IMPL_T*>(this));
    }
  }

 private:
  ResourceCache<IMPL_T>* cache_ = nullptr;
  unsigned int ref_count_ = 0;

  template <typename T>
  friend void addRef(T*);
  template <typename T>
  friend void removeRef(T*);
};

template <typename T>
inline void addRef(T* res) {
  if(res) {
    res->ref_count_ += 1;
  }
}

template <typename T>
inline void removeRef(T* res) {
  if(res) {
    res->ref_count_ -= 1;
    if(res->ref_count_ == 0) {
      delete res;
    }
  }
}
}
}
}

#endif