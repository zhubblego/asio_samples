//
// Copyright (c) 2010-2015 Marat Abrarov (abrarov@gmail.com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MA_CONTEXT_ALLOC_HANDLER_HPP
#define MA_CONTEXT_ALLOC_HANDLER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <ma/config.hpp>
#include <ma/handler_alloc_helpers.hpp>
#include <ma/handler_invoke_helpers.hpp>
#include <ma/handler_cont_helpers.hpp>
#include <ma/detail/type_traits.hpp>
#include <ma/detail/utility.hpp>

namespace ma {

/// Wrappers that override allocation strategy of the source handler.
/**
 * "Allocation strategy" means handler related pair of free functions:
 * asio_handler_allocate and asio_handler_deallocate or the default ones
 * defined by Asio.
 * http://www.boost.org/doc/libs/release/doc/html/boost_asio/reference/Handler.html
 *
 * "Execution strategy" means handler related free function asio_handler_invoke
 * or the default one defined by Asio.
 * http://www.boost.org/doc/libs/release/doc/html/boost_asio/reference/Handler.html
 *
 * Functors created by listed wrappers:
 *
 * @li override Asio allocation strategy to the one provided by context
 * parameter.
 * @li forward Asio execution strategy to the one provided by handler
 * parameter.
 * @li forward operator() to to the ones provided by handler parameter.
 *
 * The handler parameter must meet the requirements of Asio handler.
 * The context parameter must meet the requirements of Asio handler except
 * existance of asio_handler_invoke and operator() - these functions aren't
 * applied to context parameter.
 * The functors created by means of listed wrappers meet the requirements of
 * Asio handler.
 *
 * Usage of free functions called make_context_alloc_handler and
 * make_explicit_context_alloc_handler can help in construction of functors.
 *
 * Move semantic supported.
 * Move constructor is explicitly defined to support MSVC 2010.
 */

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4512)
#endif // #if defined(_MSC_VER)

template <typename Context, typename Handler>
class context_alloc_handler
{
private:
  typedef context_alloc_handler<Context, Handler> this_type;

public:
  typedef void result_type;

  template <typename C, typename H>
  context_alloc_handler(MA_FWD_REF(C) context, MA_FWD_REF(H) handler)
    : context_(detail::forward<C>(context))
    , handler_(detail::forward<H>(handler))
  {
  }

#if defined(MA_HAS_RVALUE_REFS) \
    && (defined(MA_NO_IMPLICIT_MOVE_CONSTRUCTOR) || !defined(NDEBUG))

  context_alloc_handler(this_type&& other)
    : context_(detail::move(other.context_))
    , handler_(detail::move(other.handler_))
  {
  }

  context_alloc_handler(const this_type& other)
    : context_(other.context_)
    , handler_(other.handler_)
  {
  }

#endif

#if !defined(NDEBUG)
  ~context_alloc_handler()
  {
  }
#endif

  friend void* asio_handler_allocate(std::size_t size, this_type* context)
  {
    // Forward to asio_handler_allocate provided by the specified allocation
    // context.
    return ma_handler_alloc_helpers::allocate(size, context->context_);
  }

  friend void asio_handler_deallocate(void* pointer, std::size_t size,
      this_type* context)
  {
    // Forward to asio_handler_deallocate provided by the specified allocation
    // context.
    ma_handler_alloc_helpers::deallocate(pointer, size, context->context_);
  }

#if defined(MA_HAS_RVALUE_REFS)

  template <typename Function>
  friend void asio_handler_invoke(MA_FWD_REF(Function) function,
      this_type* context)
  {
    // Forward to asio_handler_invoke provided by source handler.
    ma_handler_invoke_helpers::invoke(
        detail::forward<Function>(function), context->handler_);
  }

#else // defined(MA_HAS_RVALUE_REFS)

  template <typename Function>
  friend void asio_handler_invoke(Function& function, this_type* context)
  {
    // Forward to asio_handler_invoke provided by source handler.
    ma_handler_invoke_helpers::invoke(function, context->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(const Function& function, this_type* context)
  {
    // Forward to asio_handler_invoke provided by source handler.
    ma_handler_invoke_helpers::invoke(function, context->handler_);
  }

#endif // defined(MA_HAS_RVALUE_REFS)

  friend bool asio_handler_is_continuation(this_type* context)
  {
    return ma_handler_cont_helpers::is_continuation(context->handler_);
  }

  void operator()()
  {
    handler_();
  }

  template <typename Arg1>
  void operator()(MA_FWD_REF(Arg1) arg1)
  {
    handler_(detail::forward<Arg1>(arg1));
  }

  template <typename Arg1, typename Arg2>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2)
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2));
  }

  template <typename Arg1, typename Arg2, typename Arg3>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3)
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4)
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
      typename Arg5>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4, MA_FWD_REF(Arg5) arg5)
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4),
        detail::forward<Arg5>(arg5));
  }

  void operator()() const
  {
    handler_();
  }

  template <typename Arg1>
  void operator()(MA_FWD_REF(Arg1) arg1) const
  {
    handler_(detail::forward<Arg1>(arg1));
  }

  template <typename Arg1, typename Arg2>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2) const
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2));
  }

  template <typename Arg1, typename Arg2, typename Arg3>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3) const
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4) const
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
      typename Arg5>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4, MA_FWD_REF(Arg5) arg5) const
  {
    handler_(detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4),
        detail::forward<Arg5>(arg5));
  }

private:
  Context context_;
  Handler handler_;
}; // class context_alloc_handler

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // #if defined(_MSC_VER)

/// Helper for creation of wrapped handler.
template <typename Context, typename Handler>
inline context_alloc_handler<
    typename detail::decay<Context>::type,
    typename detail::decay<Handler>::type>
make_context_alloc_handler(MA_FWD_REF(Context) context,
    MA_FWD_REF(Handler) handler)
{
  typedef typename detail::decay<Context>::type context_type;
  typedef typename detail::decay<Handler>::type handler_type;
  return context_alloc_handler<context_type, handler_type>(
      detail::forward<Context>(context), detail::forward<Handler>(handler));
}

/// Specialized version of context_alloc_handler that is optimized for reuse of
/// specified context by the specified source handler.
/**
 * The context wrapped by explicit_context_alloc_handler is additionally passed
 * (by const reference) to the source handler as first parameter. Comparing to
 * context_alloc_handler explicit_context_alloc_handler helps to reduce
 * the resulted handler size and its copy cost.
 */

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4512)
#endif // #if defined(_MSC_VER)

template <typename Context, typename Handler>
class explicit_context_alloc_handler
{
private:
  typedef explicit_context_alloc_handler<Context, Handler> this_type;

public:
  typedef void result_type;

  template <typename C, typename H>
  explicit_context_alloc_handler(MA_FWD_REF(C) context, MA_FWD_REF(H) handler)
    : context_(detail::forward<C>(context))
    , handler_(detail::forward<H>(handler))
  {
  }

#if defined(MA_HAS_RVALUE_REFS) \
    && (defined(MA_NO_IMPLICIT_MOVE_CONSTRUCTOR) || !defined(NDEBUG))

  explicit_context_alloc_handler(this_type&& other)
    : context_(detail::move(other.context_))
    , handler_(detail::move(other.handler_))
  {
  }

  explicit_context_alloc_handler(const this_type& other)
    : context_(other.context_)
    , handler_(other.handler_)
  {
  }

#endif

#if !defined(NDEBUG)
  ~explicit_context_alloc_handler()
  {
  }
#endif

  friend void* asio_handler_allocate(std::size_t size, this_type* context)
  {
    return ma_handler_alloc_helpers::allocate(size, context->context_);
  }

  friend void asio_handler_deallocate(void* pointer, std::size_t size,
      this_type* context)
  {
    ma_handler_alloc_helpers::deallocate(pointer, size, context->context_);
  }

#if defined(MA_HAS_RVALUE_REFS)

  template <typename Function>
  friend void asio_handler_invoke(MA_FWD_REF(Function) function,
      this_type* context)
  {
    ma_handler_invoke_helpers::invoke(
        detail::forward<Function>(function), context->handler_);
  }

#else // defined(MA_HAS_RVALUE_REFS)

  template <typename Function>
  friend void asio_handler_invoke(Function& function, this_type* context)
  {
    ma_handler_invoke_helpers::invoke(function, context->handler_);
  }

  template <typename Function>
  friend void asio_handler_invoke(const Function& function, this_type* context)
  {
    ma_handler_invoke_helpers::invoke(function, context->handler_);
  }

#endif // defined(MA_HAS_RVALUE_REFS)

  friend bool asio_handler_is_continuation(this_type* context)
  {
    return ma_handler_cont_helpers::is_continuation(context->handler_);
  }

  void operator()()
  {
    handler_(context_);
  }

  template <typename Arg1>
  void operator()(MA_FWD_REF(Arg1) arg1)
  {
    handler_(context_, detail::forward<Arg1>(arg1));
  }

  template <typename Arg1, typename Arg2>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2)
  {
    handler_(context_, detail::forward<Arg1>(arg1),
        detail::forward<Arg2>(arg2));
  }

  template <typename Arg1, typename Arg2, typename Arg3>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3)
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4)
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
      typename Arg5>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4, MA_FWD_REF(Arg5) arg5)
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4),
        detail::forward<Arg5>(arg5));
  }

  void operator()() const
  {
    handler_(context_);
  }

  template <typename Arg1>
  void operator()(MA_FWD_REF(Arg1) arg1) const
  {
    handler_(context_, detail::forward<Arg1>(arg1));
  }

  template <typename Arg1, typename Arg2>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2) const
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2));
  }

  template <typename Arg1, typename Arg2, typename Arg3>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3) const
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4) const
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4));
  }

  template <typename Arg1, typename Arg2, typename Arg3, typename Arg4,
      typename Arg5>
  void operator()(MA_FWD_REF(Arg1) arg1, MA_FWD_REF(Arg2) arg2,
      MA_FWD_REF(Arg3) arg3, MA_FWD_REF(Arg4) arg4, MA_FWD_REF(Arg5) arg5) const
  {
    handler_(context_, detail::forward<Arg1>(arg1), detail::forward<Arg2>(arg2),
        detail::forward<Arg3>(arg3), detail::forward<Arg4>(arg4),
        detail::forward<Arg5>(arg5));
  }

private:
  Context context_;
  Handler handler_;
}; // class explicit_context_alloc_handler

#if defined(_MSC_VER)
#pragma warning(pop)
#endif // #if defined(_MSC_VER)

template <typename Context, typename Handler>
inline explicit_context_alloc_handler<
    typename detail::decay<Context>::type,
    typename detail::decay<Handler>::type>
make_explicit_context_alloc_handler(MA_FWD_REF(Context) context,
    MA_FWD_REF(Handler) handler)
{
  typedef typename detail::decay<Context>::type context_type;
  typedef typename detail::decay<Handler>::type handler_type;
  return explicit_context_alloc_handler<context_type, handler_type>(
      detail::forward<Context>(context), detail::forward<Handler>(handler));
}

} // namespace ma

#endif // MA_CONTEXT_ALLOC_HANDLER_HPP
