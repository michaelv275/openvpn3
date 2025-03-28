//    OpenVPN -- An application to securely tunnel IP networks
//               over a single port, with support for SSL/TLS-based
//               session authentication and key exchange,
//               packet encryption, packet authentication, and
//               packet compression.
//
//    Copyright (C) 2012- OpenVPN Inc.
//
//    SPDX-License-Identifier: MPL-2.0 OR AGPL-3.0-only WITH openvpn3-openssl-exception
//

#pragma once

#include <string>
#include <cstring> // for std::strlen, and std::memset
#include <ostream>

#include <openvpn/common/strneq.hpp>
#include <openvpn/buffer/buffer.hpp>
#include <openvpn/buffer/bufstr.hpp>

namespace openvpn {
/**
    @brief A string-like type that clears the buffer contents on delete
*/
class SafeString
{
    static constexpr size_t INITIAL_CAPACITY = 32;
    static constexpr BufferFlags BUF_FLAGS{BufAllocFlags::DESTRUCT_ZERO | BufAllocFlags::GROW};

  public:
    SafeString()
    {
    }

    SafeString(const char *str, const size_t size)
        : data(size + 1, BUF_FLAGS)
    {
        if (size == std::numeric_limits<size_t>::max())
            OPENVPN_BUFFER_THROW(buffer_overflow)
        data.write((unsigned char *)str, size);
        trail();
    }

    SafeString(const char *str)
        : SafeString(str, std::strlen(str))
    {
    }

    SafeString(const std::string &str)
        : SafeString(str.c_str(), str.length())
    {
    }

    const char *c_str() const
    {
        if (data.defined())
            return (const char *)data.c_data();
        else
            return "";
    }

    // Note: unsafe because of conversion to std::string
    std::string to_string() const
    {
        return buf_to_string(data);
    }

    size_t length() const
    {
        return data.size();
    }

    bool empty() const
    {
        return !length();
    }

    char &operator[](size_t pos)
    {
        return *reinterpret_cast<char *>(data.index(pos));
    }

    const char &operator[](size_t pos) const
    {
        return *reinterpret_cast<const char *>(data.c_index(pos));
    }

    bool operator==(const char *str) const
    {
        return !operator!=(str);
    }

    bool operator!=(const char *str) const
    {
        return crypto::str_neq(str, c_str());
    }

    bool operator==(const std::string &str) const
    {
        return !operator!=(str);
    }

    bool operator!=(const std::string &str) const
    {
        return crypto::str_neq(str.c_str(), c_str());
    }

    SafeString &operator+=(char c)
    {
        alloc();
        data.push_back((unsigned char)c);
        trail();
        return *this;
    }

    SafeString &operator+=(const char *s)
    {
        return append(s);
    }

    SafeString &operator+=(const SafeString &str)
    {
        return append(str);
    }

    SafeString &append(const char *s)
    {
        alloc();
        data.write((unsigned char *)s, std::strlen(s));
        trail();
        return *this;
    }

    SafeString &append(const SafeString &str)
    {
        alloc();
        data.append(str.data);
        trail();
        return *this;
    }

    SafeString &append(const SafeString &str, size_t subpos, size_t sublen)
    {
        alloc();
        data.append(str.data.range(subpos, sublen));
        trail();
        return *this;
    }

    void reserve(const size_t n)
    {
        if (data.allocated())
            data.reserve(n + 1);
        else
            data.init(n + 1, BUF_FLAGS);
    }

    void wipe()
    {
        data.clear();
    }

  private:
    void alloc()
    {
        if (!data.allocated())
            data.init(INITIAL_CAPACITY, BUF_FLAGS);
    }

    void trail()
    {
        data.set_trailer(0);
    }

    BufferAllocated data;
};

template <typename Elem, typename Traits>
std::basic_ostream<Elem, Traits> &operator<<(std::basic_ostream<Elem, Traits> &os,
                                             const SafeString &ss)
{
    os << ss.c_str();
    return os;
}
} // namespace openvpn
