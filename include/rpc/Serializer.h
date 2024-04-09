/********************************************************************************
 * @author: Huang Pisong
 * @email: huangpisong@foxmail.com
 * @date: 2024/04/03 09:38:19
 * @version: 1.0
 * @description:
 ********************************************************************************/
#ifndef SERIALIZER_H
#define SERIALIZER_H

#include "base/ByteArray.h"
#include "base/Reflection.hpp"
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
class Serializer {
public:
	using ptr = std::shared_ptr<ByteArray>;
	Serializer() { byte_array_ = std::make_shared<ByteArray>(); }
	Serializer(ByteArray::ptr byte_array) { byte_array_ = byte_array; }
	Serializer(const std::string& in) {
		byte_array_ = std::make_shared<ByteArray>();
		writeRowData(&in[0], in.size());
		reset();
	}

	Serializer(const char* in, int len) {
		byte_array_ = std::make_shared<ByteArray>();
		writeRowData(in, len);
		reset();
	}

public:
	int size() { return byte_array_->getSize(); }

	void reset() { byte_array_->setPosition(0); }

	void offset(int off) {
		int old = byte_array_->getPosition();
		byte_array_->setPosition(old + off);
	}

	std::string toString() { return byte_array_->toString(); }
	/**
	 * @brief 直接写入原生数据
	 *
	 * @param in
	 * @param len
	 */
	void writeRowData(const char* in, int len) { byte_array_->write(in, len); }

	/**
	 * @brief 写入无压缩的数字
	 *
	 * @tparam T
	 * @param value
	 */
	template <class T> void writeFint(T value) {
		byte_array_->writeFint(value);
	}
	void clear() { byte_array_->clear(); }
	template <typename Type> void read(Type& t) {
		using T = std::remove_cvref_t<Type>;
		static_assert(!std::is_pointer_v<T>);
		if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, char> ||
		              std::is_same_v<T, unsigned char>) {
			t = byte_array_->readFint8();
		} else if constexpr (std::is_same_v<T, float>) {
			t = byte_array_->readFloat();
		} else if constexpr (std::is_same_v<T, double>) {
			t = byte_array_->readDouble();
		} else if constexpr (std::is_same_v<T, int8_t>) {
			t = byte_array_->readFint8();
		} else if constexpr (std::is_same_v<T, uint8_t>) {
			t = byte_array_->readFuint8();
		} else if constexpr (std::is_same_v<T, int16_t>) {
			t = byte_array_->readFint16();
		} else if constexpr (std::is_same_v<T, uint16_t>) {
			t = byte_array_->readFuint16();
		} else if constexpr (std::is_same_v<T, int32_t>) {
			t = byte_array_->readInt32();
		} else if constexpr (std::is_same_v<T, uint32_t>) {
			t = byte_array_->readUint32();
		} else if constexpr (std::is_same_v<T, int64_t>) {
			t = byte_array_->readInt64();
		} else if constexpr (std::is_same_v<T, uint64_t>) {
			t = byte_array_->readUint64();
		} else if constexpr (std::is_same_v<T, std::string>) {
			t = byte_array_->readStringVint();
		} else if constexpr (std::is_enum_v<T>) {
			t = static_cast<T>(byte_array_->readInt32());
		} else if constexpr (std::is_class_v<T>) { // 针对一些类类型
			static_assert(std::is_aggregate_v<T>);
			VisitMembers(
			    t, [&](auto&&... items) { (void)((*this) >> ... >> items); });
		}
	}

	template <typename Type> void write(const Type& t) {
		using T = std::remove_cvref_t<Type>;
		static_assert(!std::is_pointer_v<T>);
		if constexpr (std::is_same_v<T, bool> || std::is_same_v<T, char> ||
		              std::is_same_v<T, unsigned char>) {
			byte_array_->writeFint8(t);
		} else if constexpr (std::is_same_v<T, float>) {
			byte_array_->writeFloat(t);
		} else if constexpr (std::is_same_v<T, double>) {
			byte_array_->writeDouble(t);
		} else if constexpr (std::is_same_v<T, int8_t>) {
			byte_array_->writeFint8(t);
		} else if constexpr (std::is_same_v<T, uint8_t>) {
			byte_array_->writeFuint8(t);
		} else if constexpr (std::is_same_v<T, int16_t>) {
			byte_array_->writeFint16(t);
		} else if constexpr (std::is_same_v<T, uint16_t>) {
			byte_array_->writeFuint16(t);
		} else if constexpr (std::is_same_v<T, int32_t>) {
			byte_array_->writeInt32(t);
		} else if constexpr (std::is_same_v<T, uint32_t>) {
			byte_array_->writeUint32(t);
		} else if constexpr (std::is_same_v<T, int64_t>) {
			byte_array_->writeInt64(t);
		} else if constexpr (std::is_same_v<T, uint64_t>) {
			byte_array_->writeUint64(t);
		} else if constexpr (std::is_same_v<T, std::string>) {
			byte_array_->writeStringVint(t);
		} else if constexpr (std::is_same_v<T, char*>) {
			byte_array_->writeStringVint(std::string(t));
		} else if constexpr (std::is_same_v<T, const char*>) {
			byte_array_->writeStringVint(std::string(t));
		} else if constexpr (std::is_enum_v<T>) {
			byte_array_->writeInt32(static_cast<int32_t>(t));
		} else if constexpr (std::is_class_v<T>) {
			static_assert(std::is_aggregate_v<T>);
			VisitMembers(
			    t, [&](auto&&... items) { (void)((*this) << ... << items); });
		}
	}

public:
	template <typename T> Serializer& operator>>(T& i) {
		read(i);
		return *this;
	}
	template <typename T> Serializer& operator<<(const T& i) {
		write(i);
		return *this;
	}
	template <typename... Args> Serializer& operator>>(std::tuple<Args...>& t) {
		/**
		 * @brief 实际的反序列化函数，利用折叠表达式展开参数包
         * 模板化的lambda
		 */
		const auto& deserializer = [this]<typename Tuple, std::size_t... Index>(
		                               Tuple& t,
		                               std::index_sequence<Index...>) {
			(void)((*this) >> ... >> std::get<Index>(t));
		};
		deserializer(t, std::index_sequence_for<Args...>{});
		return *this;
	}
	template <typename T> Serializer& operator>>(std::list<T>& v) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			T t;
			(*this) >> t;
			v.template emplace_back(t);
		}
		return *this;
	}
	template <typename T> Serializer& operator<<(const std::list<T>& v) {
		write(v.size());
		for (auto& t : v) {
			(*this) << t;
		}
		return *this;
	}
	template <typename T> Serializer& operator>>(std::vector<T>& v) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			T t;
			(*this) >> t;
			v.template emplace_back(t);
		}
		return *this;
	}
    
	template <typename T> Serializer& operator<<(const std::vector<T>& v) {
		write(v.size());
		for (auto& t : v) {
			(*this) << t;
		}
		return *this;
	}
	template <typename T> Serializer& operator>>(std::set<T>& v) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			T t;
			(*this) >> t;
			v.template emplace(t);
		}
		return *this;
	}
	template <typename T> Serializer& operator<<(const std::set<T>& v) {
		write(v.size());
		for (auto& t : v) {
			(*this) << t;
		}
		return *this;
	}
	template <typename T> Serializer& operator>>(std::multiset<T>& v) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			T t;
			(*this) >> t;
			v.template emplace(t);
		}
		return *this;
	}
	template <typename T> Serializer& operator<<(const std::multiset<T>& v) {
		write(v.size());
		for (auto& t : v) {
			(*this) << t;
		}
		return *this;
	}
	template <typename T> Serializer& operator>>(std::unordered_set<T>& v) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			T t;
			(*this) >> t;
			v.template emplace(t);
		}
		return *this;
	}

	template <typename T>
	Serializer& operator<<(const std::unordered_set<T>& v) {
		write(v.size());
		for (auto& t : v) {
			(*this) << t;
		}
		return *this;
	}
	template <typename T>
	Serializer& operator>>(std::unordered_multiset<T>& v) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			T t;
			(*this) >> t;
			v.template emplace(t);
		}
		return *this;
	}

	template <typename T>
	Serializer& operator<<(const std::unordered_multiset<T>& v) {
		write(v.size());
		for (auto& t : v) {
			(*this) << t;
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator<<(const std::pair<K, V>& m) {
		(*this) << m.first << m.second;
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator>>(std::pair<K, V>& m) {
		(*this) >> m.first >> m.second;
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator>>(std::map<K, V>& m) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			std::pair<K, V> p;
			(*this) >> p;
			m.template emplace(p);
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator<<(const std::map<K, V>& m) {
		write(m.size());
		for (auto& t : m) {
			(*this) << t;
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator>>(std::unordered_map<K, V>& m) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			std::pair<K, V> p;
			(*this) >> p;
			m.template emplace(p);
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator<<(const std::unordered_map<K, V>& m) {
		write(m.size());
		for (auto& t : m) {
			(*this) << t;
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator>>(std::multimap<K, V>& m) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			std::pair<K, V> p;
			(*this) >> p;
			m.template emplace(p);
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator<<(const std::multimap<K, V>& m) {
		write(m.size());
		for (auto& t : m) {
			(*this) << t;
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator>>(std::unordered_multimap<K, V>& m) {
		size_t size;
		read(size);
		for (size_t i = 0; i < size; ++i) {
			std::pair<K, V> p;
			(*this) >> p;
			m.template emplace(p);
		}
		return *this;
	}
	template <typename K, typename V>
	Serializer& operator<<(const std::unordered_multimap<K, V>& m) {
		write(m.size());
		for (auto& t : m) {
			(*this) << t;
		}
		return *this;
	}

private:
	ByteArray::ptr byte_array_;
};

#endif // SERIALIZER_H
