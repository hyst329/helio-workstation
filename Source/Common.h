/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

//===----------------------------------------------------------------------===//
// Pragmas
//===----------------------------------------------------------------------===//

// unreferenced formal parameter
#pragma warning(disable: 4100)
// hides class member
#pragma warning(disable: 4458)
// decorated name length exceeded, name was truncated
#pragma warning(disable: 4503)

//===----------------------------------------------------------------------===//
// JUCE
//===----------------------------------------------------------------------===//

#if JUCE_LINUX
#   define JUCE_USE_FREETYPE_AMALGAMATED 1
#endif

#include "JuceHeader.h"
#include "Serializable.h"

#include <limits.h>
#include <float.h>
#include <math.h>

//===----------------------------------------------------------------------===//
// SparsePP
//===----------------------------------------------------------------------===//

#include "../../ThirdParty/FlatHashMap/flat_hash_map.hpp"

template <class Key, class T, class HashFn = std::hash<Key>, class EqualKey = std::equal_to<Key>>
using FlatHashMap = ska::flat_hash_map<Key, T, HashFn, EqualKey>;

template <class Value, class HashFn = std::hash<Value>, class EqualKey = std::equal_to<Value>>
using SparseHashSet = ska::flat_hash_set<Value, HashFn, EqualKey>;

using HashCode = size_t;

#if !defined HASH_CODE_MAX
#   define HASH_CODE_MAX SIZE_MAX
#endif

struct StringHash
{
    inline HashCode operator()(const juce::String &key) const noexcept
    {
        return static_cast<HashCode>(key.hashCode()) % HASH_CODE_MAX;
    }
};

struct IdentifierHash
{
    inline HashCode operator()(const juce::Identifier &key) const noexcept
    {
        return static_cast<HashCode>(key.toString().hashCode()) % HASH_CODE_MAX;
    }

    static int generateHash(const Identifier& key, int upperLimit) noexcept
    {
        return uint32(key.toString().hashCode()) % (uint32)upperLimit;
    }
};

//===----------------------------------------------------------------------===//
// Various helpers
//===----------------------------------------------------------------------===//

template <class T>
using UniquePointer = std::unique_ptr<T>;

template <class T>
using Function = std::function<T>;

#if _MSC_VER
inline float roundf(float x)
{
    return (x >= 0.0f) ? floorf(x + 0.5f) : ceilf(x - 0.5f);
}
#endif

#if JUCE_ANDROID || JUCE_IOS
#   define HELIO_MOBILE 1
#else
#   define HELIO_DESKTOP 1
#endif

// Beat is essentially a quarter-note
#define BEATS_PER_BAR 4

// Defines the maximum available resolution
#define TICKS_PER_BEAT 16

#define VELOCITY_SAVE_ACCURACY 1024.f

// Rolls allow up to 16 divisions per beat, there's no need for better accuracy:
inline float roundBeat(float beat)
{
    return roundf(beat * static_cast<float>(TICKS_PER_BEAT)) / static_cast<float>(TICKS_PER_BEAT);
}

#define forEachValueTreeChildWithType(parentElement, child, requiredType) \
    for (const auto &child : parentElement) if (child.hasType(requiredType))

#define callbackOnMessageThread(cls, function, ...) \
    MessageManager::getInstance()->callFunctionOnMessageThread([](void *ptr) -> void* \
        { \
            const auto *self = static_cast<cls *>(ptr); \
            if (self->function != nullptr) \
            { \
                self->function(__VA_ARGS__); \
            } \
            return nullptr; \
        }, this)

// Implements compile-time FNV1a hash:
constexpr uint32 fnv1a_32_val = 0x811c9dc5;
constexpr uint64 fnv1a_32_prime = 0x1000193;
inline constexpr uint32 CompileTimeHash(const char *const str, const uint32 value = fnv1a_32_val) noexcept
{
    return (str[0] == '\0') ? value : CompileTimeHash(&str[1], uint32(value ^ uint32(str[0])) * fnv1a_32_prime);
}

//===----------------------------------------------------------------------===//
// Internationalization
//===----------------------------------------------------------------------===//

#include "TranslationsManager.h"

#if defined TRANS
#   undef TRANS
#endif

#define TRANS(stringLiteral) TranslationsManager::getInstance().translate(stringLiteral)
#define TRANS_PLURAL(stringLiteral, intValue) TranslationsManager::getInstance().translate(stringLiteral, intValue)
