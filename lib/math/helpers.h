// *************************************************************
// File:    helpers.h
// Descr:   helpers math and geometry functions
// Author:  Novoselov Anton @ 2017
// URL:     https://github.com/ans-hub/geomath_lib
// *************************************************************

#ifndef SMPL_HELPERS_H
#define SMPL_HELPERS_H

#include <vector>

namespace anshub {

namespace math_helpers {

template<class T>
void DummyPushBack(T& c) { c.push_back(c.front()); }

template<class T>
void DummyPopBack(T& c) { c.pop_back(); }

template<class T>
T Min(const T& a, const T& b) { return (a<b) ? a : b; }

template<class T>
T Max(const T& a, const T& b) { return (a>b) ? a : b; }

template<class T>
void Swap(T& a, T& b) { T t {std::move(a)}; a = std::move(b); b = std::move(t); }

} // namespace math_helpers

} // namespace anshub

#endif  // SMPL_HELPERS_H