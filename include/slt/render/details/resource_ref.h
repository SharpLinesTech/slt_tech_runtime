#ifndef SLT_RENDER_RESOURCE_REF_H
#define SLT_RENDER_RESOURCE_REF_H
namespace slt {
namespace render {
namespace _ {

// Reference counting reference to a resource. handle_cache_ will contain a
// copy of the underlying handle.
template <typename T>
struct ResourceRef {
  using handle_t = typename T::handle_t;

  T* resource_;
  handle_t handle_cache_;

  ResourceRef() : resource_(nullptr), handle_cache_() {}

  ~ResourceRef() {
    removeRef(resource_);
  }

  explicit ResourceRef(T* res) : resource_(res), handle_cache_(res->handle_) {
    addRef(res);
  }

  ResourceRef(ResourceRef const& rhs)
      : resource_(rhs.resource_), handle_cache_(rhs.handle_cache_) {
    addRef(resource_);
  }

  ResourceRef(ResourceRef&& rhs)
      : resource_(rhs.resource_), handle_cache_(rhs.handle_cache_) {
    rhs.resource_ = nullptr;
    rhs.handle_cache_ = handle_t();
  }

  ResourceRef operator=(ResourceRef const& rhs) {
    resource_ = rhs.resource_;
    handle_cache_ = rhs.handle_cache_;
    addRef(resource_);
    return *this;
  }

  ResourceRef operator=(ResourceRef&& rhs) {
    resource_ = rhs.resource_;
    handle_cache_ = rhs.handle_cache_;
    rhs.resource_ = nullptr;
    rhs.handle_cache_ = handle_t();
    return *this;
  }

  operator bool() const {
    return resource_ != nullptr;
  }
};
}
}
}
#endif
