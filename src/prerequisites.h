#pragma once

#ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0A00 // Windows 10/11
#endif

#include <string>
#include <memory>
#include <asio.hpp>

#ifdef ERROR
    #undef ERROR
#endif

namespace ObsCamMove {
    // String-Types
    using String = std::string;
    using StringView = std::string_view;

    // Numeric types
    using uchar_t = unsigned char;
    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;
    using usize = std::size_t;

    typedef std::shared_ptr<asio::ip::tcp::socket> AsioTcpSocketPtr;
}
