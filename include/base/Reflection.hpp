/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/02 10:02:55
 * @version: 1.0
 * @description:
 ********************************************************************************/

#ifndef REFLECTION_H
#define REFLECTION_H
#include <iostream>
#include <type_traits>
#include <variant>

template <typename T> constexpr std::size_t MemberNum = 0;

struct AnyType {
	template <typename T> operator T();
};

/**
 * @brief 计算类型T的属性个数
 *
 * @tparam T
 * @param Args
 * @return consteval
 */
template <typename T> consteval std::size_t MemberCountImpl(auto&&... Args) {
	if constexpr (!requires { T{Args...}; }) {
		return sizeof...(Args) - 1;
	} else {
		return MemberCountImpl<T>(Args..., AnyType{});
	}
}

/**
 * @brief
 *
 * @tparam T
 * @return consteval
 */
template <typename T> consteval std::size_t MemberCount() {
	if constexpr (MemberNum<T> > 0) {
		return MemberNum<T>;
	} else {
		return MemberCountImpl<T>();
	}
}

constexpr static auto MaxVisitMembers = 64;

constexpr decltype(auto) VisitMembers(auto&& object, auto&& visitor) {
	using type = std::remove_cvref_t<decltype(object)>; // 移除对象的cv特性
	constexpr auto count = MemberCount<type>(); // 计算类型的属性个数
	if constexpr (count == 0 && std::is_class_v<type> &&
	              !std::is_same_v<type, std::monostate>) {
		static_assert(!sizeof(type), "empty struct/class is not allowed!");
	}
	static_assert(count <= MaxVisitMembers, "exceed max visit members");

	if constexpr (count == 0) {
		return visitor();
	} else if constexpr (count == 1) {
		auto&& [a1] = object;
		return visitor(a1);
	} else if constexpr (count == 2) {
		auto&& [a1, a2] = object;
		return visitor(a1, a2);
	} else if constexpr (count == 3) {
		auto&& [a1, a2, a3] = object;
		return visitor(a1, a2, a3);
	} else if constexpr (count == 4) {
		auto&& [a1, a2, a3, a4] = object;
		return visitor(a1, a2, a3, a4);
	} else if constexpr (count == 5) {
		auto&& [a1, a2, a3, a4, a5] = object;
		return visitor(a1, a2, a3, a4, a5);
	} else if constexpr (count == 6) {
		auto&& [a1, a2, a3, a4, a5, a6] = object;
		return visitor(a1, a2, a3, a4, a5, a6);
	} else if constexpr (count == 7) {
		auto&& [a1, a2, a3, a4, a5, a6, a7] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7);
	} else if constexpr (count == 8) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8);
	} else if constexpr (count == 9) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9);
	} else if constexpr (count == 10) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
	} else if constexpr (count == 11) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11);
	} else if constexpr (count == 12) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
	} else if constexpr (count == 13) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13] =
		    object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13);
	} else if constexpr (count == 14) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14] =
		    object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14);
	} else if constexpr (count == 15) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15);
	} else if constexpr (count == 16) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16);
	} else if constexpr (count == 17) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17);
	} else if constexpr (count == 18) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18);
	} else if constexpr (count == 19) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19);
	} else if constexpr (count == 20) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20);
	} else if constexpr (count == 21) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21);
	} else if constexpr (count == 22) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22);
	} else if constexpr (count == 23) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23);
	} else if constexpr (count == 24) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24);
	} else if constexpr (count == 25) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25);
	} else if constexpr (count == 26) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26] =
		    object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26);
	} else if constexpr (count == 27) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26,
		        a27] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27);
	} else if constexpr (count == 28) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28);
	} else if constexpr (count == 29) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29);
	} else if constexpr (count == 30) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30);
	} else if constexpr (count == 31) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31);
	} else if constexpr (count == 32) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32);
	} else if constexpr (count == 33) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33);
	} else if constexpr (count == 34) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34);
	} else if constexpr (count == 35) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35);
	} else if constexpr (count == 36) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36);
	} else if constexpr (count == 37) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37);
	} else if constexpr (count == 38) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38);
	} else if constexpr (count == 39) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39] =
		    object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39);
	} else if constexpr (count == 40) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39,
		        a40] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40);
	} else if constexpr (count == 41) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41);
	} else if constexpr (count == 42) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42);
	} else if constexpr (count == 43) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43);
	} else if constexpr (count == 44) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44);
	} else if constexpr (count == 45) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45);
	} else if constexpr (count == 46) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46);
	} else if constexpr (count == 47) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47);
	} else if constexpr (count == 48) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48);
	} else if constexpr (count == 49) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49);
	} else if constexpr (count == 50) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50);
	} else if constexpr (count == 51) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51);
	} else if constexpr (count == 52) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52] =
		    object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52);
	} else if constexpr (count == 53) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52,
		        a53] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53);
	} else if constexpr (count == 54) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54);
	} else if constexpr (count == 55) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55);
	} else if constexpr (count == 56) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56);
	} else if constexpr (count == 57) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57);
	} else if constexpr (count == 58) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58);
	} else if constexpr (count == 59) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58, a59] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58, a59);
	} else if constexpr (count == 60) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58, a59, a60] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58, a59, a60);
	} else if constexpr (count == 61) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58, a59, a60, a61] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58, a59, a60, a61);
	} else if constexpr (count == 62) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58, a59, a60, a61, a62] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58, a59, a60, a61, a62);
	} else if constexpr (count == 63) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58, a59, a60, a61, a62, a63] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58, a59, a60, a61, a62, a63);
	} else if constexpr (count == 64) {
		auto&& [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14,
		        a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27,
		        a28, a29, a30, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40,
		        a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53,
		        a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64] = object;
		return visitor(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13,
		               a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24,
		               a25, a26, a27, a28, a29, a30, a31, a32, a33, a34, a35,
		               a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46,
		               a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57,
		               a58, a59, a60, a61, a62, a63, a64);
	}
}

#endif // REFLECTION_H
