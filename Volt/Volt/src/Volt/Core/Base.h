#pragma once

#include <memory>
#include <string>

#define BIT(X) (1 << (X))

#ifdef VT_DEBUG
#define VT_DEBUGBREAK() __debugbreak()
#define VT_ENABLE_DEBUG_ALLOCATIONS
#define VT_ENABLE_SHADER_DEBUG
#define VT_PROFILE_GPU

#else

#ifdef VT_RELEASE
#define VT_ENABLE_SHADER_DEBUG
#define VT_PROFILE_GPU
#endif

#define VT_DEBUGBREAK();
#endif

#define SAFE_RELEASE(x) \
if (x)					\
{						\
	x->Release();		\
	x = nullptr;		\
}						\

#ifdef VT_ENABLE_ASSERTS
#define VT_ASSERT(x, ...) { if(!(x)) { VT_ERROR("Assertion Failed: {0}", __VA_ARGS__); VT_DEBUGBREAK(); } }
#define VT_CORE_ASSERT(x, ...) { if(!(x)) { VT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); VT_DEBUGBREAK(); } }
#else
#define VT_ASSERT(x, ...)
#define VT_CORE_ASSERT(x, ...)
#endif

#define VT_IMPORT __declspec(dllimport)

#define VT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}