/********************************************************************************
* @author: Huang Pisong
* @email: huangpisong@foxmail.com
* @date: 2024/03/27 16:19:20
* @version: 1.0
* @description: 
********************************************************************************/ 
#ifndef UTIL_H
#define UTIL_H
#include <byteswap.h>
#include <regex>
#include <string>
#include <type_traits>
#include <unistd.h>
#include <syscall.h>
#include <sys/time.h>

#include <bit>
#include <concepts>
#include <cstdint>
#include <string_view>
#include <algorithm>
#include <iostream>
#include <type_traits>
#include <vector>

/**
 * @brief 字节序转换
 * 
 */
template<typename   T,typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
constexpr T byte_swap(T value) {
    if constexpr(sizeof(T) == sizeof(uint8_t)){
        return (T)bswap_16((uint16_t)value);
    } else if constexpr(sizeof(T) == sizeof(uint16_t)){
        return (T)bswap_16((uint16_t)value);
    } else if constexpr(sizeof(T) == sizeof(uint32_t)){
        return (T)bswap_32((uint32_t)value);
    } else if constexpr(sizeof(T) == sizeof(uint64_t)){
        return (T)bswap_64((uint64_t)value);
    }
}

/**
 * @brief 
 * 小端转为大端
 * @tparam T 
 * @tparam std::enable_if<std::is_integral<T>::value, int>::type 
 * @param value 
 * @return constexpr T 
 */
template<typename   T,typename std::enable_if<std::is_integral<T>::value, int>::type = 0>
constexpr T endian_cast(T value){
    if constexpr (sizeof(T) == sizeof(uint8_t) || std::endian::big == std::endian::native){
        return value;
    }else if constexpr (std::endian::little == std::endian::native){
        return byte_swap(value);
    }

}

inline std::vector<std::string> parse_path(const std::string& path){
    std::vector<std::string> components;
    std::regex rgx("([^/]+)/?");
    std::sregex_iterator iter(path.begin(),path.end(),rgx);
    std::sregex_iterator end;
    while (iter != end) {
        components.push_back((*iter)[1].str());
        ++iter;
    }
    return components;
}

#endif // UTIL_H

