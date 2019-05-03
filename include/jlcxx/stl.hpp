﻿#ifndef JLCXX_STL_HPP
#define JLCXX_STL_HPP

#include <vector>

#include "module.hpp"
#include "type_conversion.hpp"

namespace jlcxx
{

namespace detail
{

struct UnusedT {};

/// Replace T1 by UnusedT if T1 == T2, return T1 otherwise
template<typename T1, typename T2>
struct SkipIfSameAs
{
  using type = T1;
};

template<typename T>
struct SkipIfSameAs<T,T>
{
  using type = UnusedT;
};

template<typename T1, typename T2> using skip_if_same = typename SkipIfSameAs<T1,T2>::type;

}

namespace stl
{

class StlWrappers
{
public:
  TypeWrapper<Parametric<TypeVar<1>>> vector;

  static void instantiate(Module& mod);
  static StlWrappers& instance();

private:
  StlWrappers(Module& mod);
  static std::unique_ptr<StlWrappers> m_instance;
};

StlWrappers& wrappers();

// using stltypes = filter_parameterlist<detail::UnusedT, ParameterList
// <
//   bool,
//   int,
//   double,
//   float,
//   short,
//   unsigned int,
//   unsigned char,
//   int64_t,
//   uint64_t,
//   detail::skip_if_same<long, int64_t>,
//   detail::skip_if_same<long long, int64_t>,
//   detail::skip_if_same<unsigned long, uint64_t>,
//   wchar_t,
//   void*
// >>;

using stltypes = filter_parameterlist<detail::UnusedT, ParameterList
<
  bool,
  int,
  double,
  float,
  int64_t,
  std::string
>>;

template<typename TypeWrapperT>
void wrap_common(TypeWrapperT& wrapped)
{
  using WrappedT = typename TypeWrapperT::type;
  using T = typename WrappedT::value_type;
  wrapped.method("cppsize", &WrappedT::size);
  wrapped.method("resize", [] (WrappedT& v, const int_t s) { v.resize(s); });
  wrapped.method("append", [] (WrappedT& v, jlcxx::ArrayRef<T> arr)
  {
    const std::size_t addedlen = arr.size();
    v.reserve(v.size() + addedlen);
    for(size_t i = 0; i != addedlen; ++i)
    {
      v.push_back(arr[i]);
    }
  });
}

template<typename T>
struct WrapVectorImpl
{
  template<typename TypeWrapperT>
  static void wrap(TypeWrapperT&& wrapped)
  {
    using WrappedT = std::vector<T>;
    
    wrap_common(wrapped);
    wrapped.method("push_back", static_cast<void (WrappedT::*)(const T&) >(&WrappedT::push_back));
    wrapped.method("getindex", [] (const WrappedT& v, int_t i) { return v[i-1]; });
    wrapped.method("setindex!", [] (WrappedT& v, const T& val, int_t i) { v[i-1] = val; });
  }
};

template<>
struct WrapVectorImpl<bool>
{
  template<typename TypeWrapperT>
  static void wrap(TypeWrapperT&& wrapped)
  {
    using WrappedT = std::vector<bool>;

    wrap_common(wrapped);
    wrapped.method("push_back", [] (WrappedT& v, const bool val) { v.push_back(val); });
    wrapped.method("getindex", [] (const WrappedT& v, int_t i) { return bool(v[i-1]); });
    wrapped.method("setindex!", [] (WrappedT& v, const bool val, int_t i) { v[i-1] = val; });
  }
};

struct WrapVector
{
  template<typename TypeWrapperT>
  void operator()(TypeWrapperT&& wrapped)
  {
    using WrappedT = typename TypeWrapperT::type;
    using T = typename WrappedT::value_type;
    WrapVectorImpl<T>::wrap(wrapped);
  }
};

template<typename T>
inline void register_vector_type()
{
  assert(registry().has_current_module());
  jl_datatype_t* jltype = julia_type<T>();
  if(jltype->name->module != registry().current_module().julia_module())
  {
    const std::string tname = julia_type_name(jltype);
    throw std::runtime_error("Type for std::vector<" + tname + "> must be defined in the same module as " + tname);
  }
  wrappers().vector.apply<std::vector<T>>(WrapVector());
}

}

template<typename T>
struct static_type_mapping<std::vector<T>> : static_type_mapping_base<std::vector<T>>
{
  using base = static_type_mapping_base<std::vector<T>>;

  static void create_on_demand()
  {
    if(base::type_pointer() == nullptr)
    {
      stl::register_vector_type<T>();
    }
  }

  static jl_datatype_t* julia_type()
  {
    create_on_demand();
    return base::type_pointer();
  }

  // Immutable (stack-allocated) reference to existing C++ pointers
  static jl_datatype_t* julia_reference_type()
  {
    create_on_demand();
    assert(base::reference_type_pointer());
    return base::reference_type_pointer();
  }

  // Type holding a pointer allocated using new. Stack allocated and instances are created with a finalizer that calls delete.
  static jl_datatype_t* julia_allocated_type()
  {
    create_on_demand();
    assert(base::allocated_type_pointer());
    return base::allocated_type_pointer();
  }
};

}

#endif
