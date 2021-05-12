// Copyright (c) 2021 Bennett Anderson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// You may choose to define on your own the following:
//
// 	- DATAREFW_ASSERT(cond)			// - Custom assert function
//
// The main types associated with datarefs are what are supported, if you try
// to use an unsupported type, you'll get a compile-time assertion failure.
//
// Supported types:
//		int
//		float
//		double
//		DrIntArr (std::vector<int>)
//		DrFloatArr (std::vector<float>)
//		std::string

#ifndef DATAREFW_H
#define DATAREFW_H

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

#include <type_traits>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#define DATAREFW_UNUSED(a) (void)(a)

#if (defined(__GNUC__) || defined(__clang__))
# define DATAREFW_NODISCARD __attribute__ ((warn_unused_result))
#elif (defined(_MSC_VER))
# define DATAREFW_NODISCARD _Check_return_
#else
# define DATAREFW_NODISCARD
#endif // (defined(__GNUC__) || defined(__clang__))

#ifndef DATAREFW_ASSERT
# if (defined(__GNUC__) || defined(__clang__))
#  include <cstdlib>
#  define DATAREFW_COND_UNLIKELY(x) __builtin_expect(x, 0)
#  define DATAREFW_ASSERT(x) \
	do { \
		if (DATAREFW_COND_UNLIKELY(!(x))) { \
			XPLMDebugString(std::string("Assertion " + \
				std::string(#x) + " failed\n").c_str()); \
			abort(); \
		} \
	} while (0)
# else
#  include <cassert>
#  define DATAREFW_ASSERT(cond) assert(cond)
# endif // defined(__GNUC__) || defined(__clang__)
#endif // DATAREFW_ASSERT

namespace datarefw {

using DrIntArr = std::vector<int>;
using DrFloatArr = std::vector<float>;

template <typename U>
struct dr_type_is_array :
	std::integral_constant<bool,
		std::is_same<DrIntArr, U>::value ||
		std::is_same<DrFloatArr, U>::value> {};

template <typename U>
struct dr_type_is_byte :
	std::integral_constant<bool,
		std::is_same<std::string, U>::value> {};

template <typename U>
struct dr_type_is_number :
	std::integral_constant<bool,
		std::is_same<int, U>::value ||
		std::is_same<float, U>::value ||
		std::is_same<double, U>::value> {};

template <typename T>
constexpr void
verify_types() {
	static_assert(
		(
		std::is_same<int, T>::value        ||
		std::is_same<float, T>::value      ||
		std::is_same<double, T>::value     ||
		std::is_same<DrIntArr, T>::value   ||
		std::is_same<DrFloatArr, T>::value ||
		std::is_same<std::string, T>::value
		),
		"Unsupported Type"
	);
}

template <typename T>
class FindDataref {
public:
	using value_type = T;

	FindDataref() = default;

	FindDataref(const std::string& dr_str) {
		find_dataref(dr_str);
	}

	FindDataref(const FindDataref<T>& dr_o) = default;
	FindDataref(FindDataref<T>&& dr_o) = default;
	FindDataref<T>& operator=(const FindDataref<T>& dr_o) = default;
	FindDataref<T>& operator=(FindDataref<T>&& dr_o) = default;

	void
	find_dataref(const std::string& dr_str) {
		dataref_name = dr_str;
		impl_find_dataref();
	}

	template <typename U = T, typename val_type = typename U::value_type,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD auto
	operator[](const std::size_t index) -> val_type {
		return at(index);
	}

	template <typename U = T,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD std::size_t
	size() const noexcept {
		return impl_get_array_size();
	}

	template <typename U = T, typename val_type = typename U::value_type,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD auto
	at(std::size_t index) -> val_type {
		DATAREFW_ASSERT(index < impl_get_array_size());
		return impl_arr_get_val(index);
	}

	// Prefix increment
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	U
	operator++() noexcept {
		auto st = impl_dr_get() + 1;
		impl_dr_set(st);
		return st;
	}

	// Postfix increment
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	U
	operator++(int) noexcept {
		return operator++();
	}

	// Prefix decrement
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	U
	operator--() noexcept {
		auto st = impl_dr_get() - 1;
		impl_dr_set(st);
		return st;
	}

	// Postfix decrement
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	U
	operator--(int) noexcept {
		return operator--();
	}

	// Assignment operator
	T
	operator=(const T& value) {
		impl_dr_set(value);
		return value;
	}

	// Compound assignment +=
	T
	operator+=(const T& value) {
		auto st = impl_dr_get() + value;
		impl_dr_set(st);
		return st;
	}

	// Compound assignment -=
	T
	operator-=(const T& value) {
		auto st = impl_dr_get() - value;
		impl_dr_set(st);
		return st;
	}

	// Compound assignment *=
	T
	operator*=(const T& value) {
		auto st = impl_dr_get() * value;
		impl_dr_set(st);
		return st;
	}

	// Compound assignment /=
	T
	operator/=(const T& value) {
		auto st = impl_dr_get() / value;
		impl_dr_set(st);
		return st;
	}

	bool
	operator==(const T& rhs) {
		return (impl_dr_get() == rhs);
	}

	bool
	operator!=(const T& rhs) {
		return (impl_dr_get() != rhs);
	}

	bool
	operator<(const T& rhs) {
		return (impl_dr_get() < rhs);
	}

	bool
	operator>(const T& rhs) {
		return (impl_dr_get() > rhs);
	}

	bool
	operator<=(const T& rhs) {
		return (impl_dr_get() <= rhs);
	}

	bool
	operator>=(const T& rhs) {
		return (impl_dr_get() >= rhs);
	}

	friend std::ostream&
	operator<<(std::ostream& os, const FindDataref<T>& obj) {
		os << obj.impl_dr_get();
		return os;
	}

	DATAREFW_NODISCARD bool
	found() const noexcept {
		return dataref_found;
	}

	DATAREFW_NODISCARD bool
	writable() const noexcept {
		return dataref_writable;
	}

	operator T() const {
		return impl_dr_get();
	}

	explicit
	operator bool() const noexcept {
		return dataref_found;
	}

	DATAREFW_NODISCARD auto
	path() const {
		DATAREFW_ASSERT(dataref_found);
		return dataref_name;
	}

	~FindDataref() = default;
private:
	// Int value
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD int
	impl_dr_get() const noexcept {
		impl_verify_dataref_found();
		return XPLMGetDatai(dataref_loc);
	}

	// Float value
	template <typename U = T,
		typename std::enable_if<std::is_same<U, float>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD float
	impl_dr_get() const noexcept {
		impl_verify_dataref_found();
		return XPLMGetDataf(dataref_loc);
	}

	// Double value
	template <typename U = T,
		typename std::enable_if<std::is_same<U, double>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD double
	impl_dr_get() const noexcept {
		impl_verify_dataref_found();
		return XPLMGetDatad(dataref_loc);
	}

	// Int array vector
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrIntArr>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD DrIntArr
	impl_dr_get() const {
		impl_verify_dataref_found();
		auto sz = impl_get_array_size();
		DrIntArr arr_val(sz);
		XPLMGetDatavi(dataref_loc, arr_val.data(), 0, sz);
		return arr_val;
	}

	// Int array Element
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrIntArr>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD DrIntArr::value_type
	impl_arr_get_val(const std::size_t index) const noexcept {
		impl_verify_dataref_found();
		DrIntArr::value_type arr_val {};
		XPLMGetDatavi(dataref_loc, &arr_val, index, 1);
		return arr_val;
	}

	// Float array vector
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrFloatArr>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD DrFloatArr
	impl_dr_get() const {
		impl_verify_dataref_found();
		auto sz = impl_get_array_size();
		DrFloatArr arr_val(sz);
		XPLMGetDatavf(dataref_loc, arr_val.data(), 0, sz);
		return arr_val;
	}

	// String value
	template <typename U = T,
		typename std::enable_if<std::is_same<U, std::string>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD std::string
	impl_dr_get() const {
		impl_verify_dataref_found();

		auto sz = impl_get_array_size();

		if (sz == 0) { return {}; }

		char *dr_val = new char[sz + 1];

		XPLMGetDatab(dataref_loc, dr_val, 0, sz);

		if (dr_val[sz] != '\0') {
			dr_val[sz] = '\0';
		}

		std::string ret_str(dr_val);
		delete[] dr_val;
		return ret_str;
	}

	// Float array Element
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrFloatArr>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD DrFloatArr::value_type
	impl_arr_get_val(const std::size_t index) const {
		impl_verify_dataref_found();
		DrFloatArr::value_type arr_val {};
		XPLMGetDatavf(dataref_loc, &arr_val, index, 1);
		return arr_val;
	}

	// Int array size
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrIntArr>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD std::size_t
	impl_get_array_size() const noexcept {
		impl_verify_dataref_found();
		return XPLMGetDatavi(dataref_loc, nullptr, 0, 0);
	}

	// Float array size
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrFloatArr>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD std::size_t
	impl_get_array_size() const noexcept {
		impl_verify_dataref_found();
		return XPLMGetDatavf(dataref_loc, nullptr, 0, 0);
	}

	// String size
	template <typename U = T,
		typename std::enable_if<std::is_same<U, std::string>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD std::size_t
	impl_get_array_size() const noexcept {
		impl_verify_dataref_found();
		return XPLMGetDatab(dataref_loc, nullptr, 0, 0);
	}

	// Int value set
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	void
	impl_dr_set(const int value) const noexcept {
		impl_verify_dataref_found();
		XPLMSetDatai(dataref_loc, value);
	}

	// Float value set
	template <typename U = T,
		typename std::enable_if<std::is_same<U, float>::value, U>::type* = nullptr>
	void
	impl_dr_set(const float value) const noexcept {
		impl_verify_dataref_found();
		XPLMSetDataf(dataref_loc, value);
	}

	// Double value set
	template <typename U = T,
		typename std::enable_if<std::is_same<U, double>::value, U>::type* = nullptr>
	void
	impl_dr_set(const float value) const noexcept {
		impl_verify_dataref_found();
		XPLMSetDatad(dataref_loc, value);
	}

	// Int array vector set
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrIntArr>::value, U>::type* = nullptr>
	void
	impl_dr_set(const DrIntArr& value) const {
		impl_verify_dataref_found();
		XPLMSetDatavi(dataref_loc, const_cast<int*> (value.data()), 0, value.size());
	}

	// Float array vector set
	template <typename U = T,
		typename std::enable_if<std::is_same<U, DrFloatArr>::value, U>::type* = nullptr>
	void
	impl_dr_set(const DrFloatArr& value) const {
		impl_verify_dataref_found();
		XPLMSetDatavf(dataref_loc, const_cast<float*> (value.data()), 0, value.size());
	}

	// String value set
	template <typename U = T,
		typename std::enable_if<std::is_same<U, std::string>::value, U>::type* = nullptr>
	void
	impl_dr_set(const std::string& value) const {
		impl_verify_dataref_found();
		XPLMSetDatab(dataref_loc, const_cast<char *> (value.data()), 0, value.length());
	}

	void
	impl_find_dataref() {
		DATAREFW_ASSERT(dataref_name != "");
		DATAREFW_ASSERT(dataref_name.find(' ') == std::string::npos);
		verify_types<T>();

		dataref_loc = XPLMFindDataRef(dataref_name.c_str());

		if (dataref_loc == nullptr) {
			return;			
		}

		dataref_types = XPLMGetDataRefTypes(dataref_loc);

		// Verify our template-provided type is correct (don't want anything weird
		// trying to access/set data).
		switch (dataref_types) {
			case xplmType_Int:
				DATAREFW_ASSERT((std::is_same<int, T>::value));
				break;
			case xplmType_Float:
				DATAREFW_ASSERT((std::is_same<float, T>::value));
				break;
			case xplmType_Double:
				DATAREFW_ASSERT((std::is_same<double, T>::value));
				break;
			case (xplmType_Int | xplmType_Float | xplmType_Double):
				DATAREFW_ASSERT(dr_type_is_number<T>::value);
				break;
			case xplmType_IntArray:
				DATAREFW_ASSERT((std::is_same<DrIntArr, T>::value));
				break;
			case xplmType_FloatArray:
				DATAREFW_ASSERT((std::is_same<DrFloatArr, T>::value));
				break;
			case xplmType_Data:
				DATAREFW_ASSERT((std::is_same<std::string, T>::value));
				break;
			case xplmType_Unknown:
				DATAREFW_ASSERT(dataref_types != xplmType_Unknown);
				break;
		};

		dataref_writable = XPLMCanWriteDataRef(dataref_loc);
		dataref_found = true;
	}

	void
	impl_verify_dataref_found() const noexcept {
		DATAREFW_ASSERT(dataref_loc != nullptr);
	}

	void
	impl_verify_dataref_writable() const noexcept {
		DATAREFW_ASSERT(dataref_writable != false);
	}

	std::string dataref_name;
	XPLMDataRef dataref_loc { nullptr };
	XPLMDataTypeID dataref_types { xplmType_Unknown };
	bool dataref_writable { false };
	bool dataref_found { false };
};

	
template <typename T, std::size_t ARRAY_SIZE = 0>
class CreateDataref {
public:
	using value_type = T;
	using size_type = std::size_t;

	CreateDataref() = default;

	CreateDataref(const std::string& pdr_path, bool pis_writable = false) {
		create_dataref(pdr_path, pis_writable);
	}

	void
	create_dataref(const std::string& pdr_path, bool pis_writable = false) {
		dataref_name = pdr_path;
		dataref_writable = pis_writable;
		impl_create_dataref();
	}

	CreateDataref(const CreateDataref<T, ARRAY_SIZE>& dr_o) = default;
	CreateDataref(CreateDataref<T, ARRAY_SIZE>&& dr_o) = default;
	CreateDataref<T, ARRAY_SIZE>& operator=(const CreateDataref<T, ARRAY_SIZE>& dr_o) = default;
	CreateDataref<T, ARRAY_SIZE>& operator=(CreateDataref<T, ARRAY_SIZE>&& dr_o) = default;

	template <typename U = T, typename val_type = typename U::value_type,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD auto
	operator[](const std::size_t index) -> val_type& {
		return at(index);
	}

	template <typename U = T,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD size_type
	size() const noexcept {
		return dataref_storage.size();
	}

	template <typename U = T,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD size_type
	max_size() const noexcept {
		return dataref_storage_max_size;
	}

	template <typename U = T, typename val_type = typename U::value_type,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD auto
	at(size_type index) -> val_type& {
		DATAREFW_ASSERT(index < ARRAY_SIZE);
		return dataref_storage[index];
	}

	// Prefix increment
	T&
	operator++() noexcept {
		dataref_storage++;
		return dataref_storage;
	}

	// Postfix increment
	T
	operator++(int) noexcept {
		return operator++();
	}

	// Prefix decrement
	T&
	operator--() noexcept {
		dataref_storage--;
		return dataref_storage;
	}

	// Postfix decrement
	T
	operator--(int) noexcept {
		return operator--();
	}

	// Assignment operator
	T
	operator=(const T& value) {
		dataref_storage = value;
		return dataref_storage;
	}
	
	// Compound assignment +=
	T&
	operator+=(const T& value) {
		dataref_storage += value;
		return dataref_storage;
	}

	// Compound assignment -=
	T&
	operator-=(const T& value) {
		dataref_storage -= value;
		return dataref_storage;
	}

	// Compound assignment *=
	T&
	operator*=(const T& value) {
		dataref_storage *= value;
		return dataref_storage;
	}

	// Compound assignment /=
	T&
	operator/=(const T& value) {
		dataref_storage /= value;
		return dataref_storage;
	}

	bool
	operator==(const T& rhs) {
		return (dataref_storage == rhs);
	}

	bool
	operator!=(const T& rhs) {
		return (dataref_storage != rhs);
	}

	bool
	operator<(const T& rhs) {
		return (dataref_storage < rhs);
	}

	bool
	operator>(const T& rhs) {
		return (dataref_storage > rhs);
	}

	bool
	operator<=(const T& rhs) {
		return (dataref_storage <= rhs);
	}

	bool
	operator>=(const T& rhs) {
		return (dataref_storage >= rhs);
	}

	friend std::ostream&
	operator<<(std::ostream& os, const CreateDataref<T, ARRAY_SIZE>& obj) {
		os << obj.dataref_storage;
		return os;
	}

	operator T() const noexcept {
		return dataref_storage;
	}

	explicit
	operator bool() const noexcept {
		return (dataref_loc != nullptr);
	}

	DATAREFW_NODISCARD auto
	path() const {
		return dataref_name;
	}

	~CreateDataref() {
		impl_dr_cleanup();
	}
private:
	template <typename U, std::size_t ARR_SIZE = 0>
	static CreateDataref<U, ARR_SIZE> *
	impl_proc_ref(void *refcon) {
		DATAREFW_ASSERT(refcon != nullptr);
		return reinterpret_cast<CreateDataref<U, ARR_SIZE> *> (refcon);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_byte<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_byte(void *refcon, void *values, int offset, int count) {
		if (values == nullptr || offset >= count || count == 0) {
			return;
		}

		DATAREFW_ASSERT(offset >= 0);

		auto ncount = count - offset;
		char *cvalues = static_cast<char *> (values);
		char *temp_val = new char[count + 1];
		const auto odr = impl_proc_ref<std::string>(refcon);

		cvalues += offset; // Read from offset
		std::memcpy(temp_val, cvalues, ncount);

		// Ensure proper null-termination on c-strings
		if (temp_val[ncount] != '\0') {
			temp_val[ncount] = '\0';
		}

		odr->dataref_storage = std::string(temp_val);
		delete[] temp_val;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_byte<U>::value, U>::type* = nullptr>
	DATAREFW_NODISCARD static int
	impl_dr_read_byte(void *refcon, void *values, int offset, int max) {
		const auto odr = impl_proc_ref<std::string>(refcon);
		const int a_sz = static_cast<int> (odr->dataref_storage.size());

		if (values == nullptr) {
			return a_sz;
		}

		DATAREFW_ASSERT(offset >= 0);

		int upper_limit;
		if ((offset + max) < a_sz) {
			upper_limit = max;
		} else {
			upper_limit = a_sz;
		}

		char *cvalues = static_cast<char *>(values);
		char *dr_str = &odr->dataref_storage[0];

		dr_str += offset; // Offset
		std::memcpy(values, dr_str, upper_limit);

		// Ensure proper null-termination on c-strings
		if (cvalues[upper_limit] != '\0') {
			cvalues[upper_limit] = '\0';
		}

		return upper_limit;
	}

	template <typename U, typename V = T, std::size_t ARR_SIZE = ARRAY_SIZE,
		typename std::enable_if<dr_type_is_array<V>::value, V>::type* = nullptr>
	static void
	impl_dr_write_tmplt_arr(void *refcon, U *values, int offset, int count) {
		const auto odr = impl_proc_ref<T, ARR_SIZE>(refcon);

		if (values == nullptr) {
			return;
		}

		DATAREFW_ASSERT(offset >= 0);

		for (auto i = 0; i < count; ++i) {
			U *value_ptr = values;
			value_ptr += offset;
			odr->dataref_storage[i] = *value_ptr;
		}
	}

	template <typename U, typename V = T, std::size_t ARR_SIZE = ARRAY_SIZE,
		typename std::enable_if<dr_type_is_array<V>::value, V>::type* = nullptr>
	DATAREFW_NODISCARD static int
	impl_dr_read_tmplt_arr(void *refcon, U *values, int offset, int max) {
		const auto odr = impl_proc_ref<T, ARR_SIZE>(refcon);
		const int a_sz = static_cast<int> (odr->dataref_storage.size());

		if (values == nullptr) {
			return a_sz;
		}

		DATAREFW_ASSERT(offset >= 0);

		int upper_limit;
		if ((offset + max) < a_sz) {
			upper_limit = max;
		} else {
			upper_limit = a_sz;
		}

		for (auto i = 0; i < upper_limit; ++i) {
			values[i] = odr->dataref_storage[i + offset];
		}

		return upper_limit;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	static int
	impl_dr_read_i(void *refcon) {
		return impl_proc_ref<T>(refcon)->dataref_storage;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_i(void *refcon, int val) {
		impl_proc_ref<T>(refcon)->dataref_storage = val;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	static float
	impl_dr_read_f(void *refcon) {
		return impl_proc_ref<T>(refcon)->dataref_storage;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_f(void *refcon, float val) {
		impl_proc_ref<T>(refcon)->dataref_storage = val;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	static double
	impl_dr_read_d(void *refcon) {
		return impl_proc_ref<T>(refcon)->dataref_storage;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_d(void *refcon, double val) {
		impl_proc_ref<T>(refcon)->dataref_storage = val;
	}

	template <typename U = T, typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	static int
	impl_dr_read_vi(void *refcon, int *values, int offset, int max) {
		return impl_dr_read_tmplt_arr<int>(refcon, values, offset, max);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_vi(void *refcon, int *values, int offset, int max) {
		impl_dr_write_tmplt_arr<int>(refcon, values, offset, max);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	static int
	impl_dr_read_vf(void *refcon, float *values, int offset, int max) {
		return impl_dr_read_tmplt_arr<float>(refcon, values, offset, max);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_vf(void *refcon, float *values, int offset, int max) {
		impl_dr_write_tmplt_arr<float>(refcon, values, offset, max);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_byte<U>::value, U>::type* = nullptr>
	static int
	impl_dr_read_b(void *refcon, void *values, int offset, int max) {
		return impl_dr_read_byte(refcon, values, offset, max);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_byte<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_b(void *refcon, void *values, int offset, int max) {
		impl_dr_write_byte(refcon, values, offset, max);
	}

	void
	impl_dr_get_datatype() {
		if (std::is_same<T, int>::value) {
			dataref_types = xplmType_Int;
		} else if (std::is_same<T, float>::value) {
			dataref_types = xplmType_Float;
		} else if (std::is_same<T, double>::value) {
			dataref_types = xplmType_Double;
		} else if (std::is_same<T, DrIntArr>::value) {
			dataref_types = xplmType_IntArray;
		} else if (std::is_same<T, DrFloatArr>::value) {
			dataref_types = xplmType_FloatArray;
		} else if (std::is_same<T, std::string>::value) {
			dataref_types = xplmType_Data;
		}
	}

	template <typename U = T, typename std::enable_if<dr_type_is_number<U>::value, U>::type* = nullptr>
	void
	register_dataref_accessor() {
		dataref_loc = XPLMRegisterDataAccessor(
				dataref_name.c_str(),
				dataref_types, dataref_writable,
				impl_dr_read_i, impl_dr_write_i,
				impl_dr_read_f, impl_dr_write_f,
				impl_dr_read_d, impl_dr_write_d,
				nullptr, nullptr,
				nullptr, nullptr,
				nullptr, nullptr,
				this, this);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	void
	register_dataref_accessor() {
		dataref_loc = XPLMRegisterDataAccessor(
				dataref_name.c_str(),
				dataref_types, dataref_writable,
				nullptr, nullptr,
				nullptr, nullptr,
				nullptr, nullptr,
				impl_dr_read_vi, impl_dr_write_vi,
				impl_dr_read_vf, impl_dr_write_vf,
				nullptr, nullptr,
				this, this);
		dataref_storage.resize(ARRAY_SIZE);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_byte<U>::value, U>::type* = nullptr>
	void
	register_dataref_accessor() {
		dataref_loc = XPLMRegisterDataAccessor(
				dataref_name.c_str(),
				dataref_types, dataref_writable,
				nullptr, nullptr,
				nullptr, nullptr,
				nullptr, nullptr,
				nullptr, nullptr,
				nullptr, nullptr,
				impl_dr_read_b, impl_dr_write_b,
				this, this);
	}

	// I'm sure there's a more *elaborate* way to do the following of which I'm not aware of:
	template <std::size_t ARR_SIZE = ARRAY_SIZE, typename U = T,
		typename std::enable_if<dr_type_is_array<U>::value, U>::type* = nullptr>
	constexpr void
	array_verif() const noexcept {
		static_assert(ARR_SIZE > 0, "Array size can't be zero.");
	}

	template <typename U = T, typename std::enable_if<!dr_type_is_array<U>::value, U>::type* = nullptr>
	constexpr void
	array_verif() const noexcept {}

	void
	impl_create_dataref() {
		DATAREFW_ASSERT(dataref_name != "");
		DATAREFW_ASSERT(dataref_name.find(' ') == std::string::npos);

		verify_types<T>();
		array_verif();
		impl_dr_get_datatype();
		register_dataref_accessor();
	}

	void
	impl_dr_cleanup() {
		if (dataref_loc) {
			XPLMUnregisterDataAccessor(dataref_loc);
			dataref_loc = nullptr;
		}
	}

	std::string dataref_name;
	XPLMDataRef dataref_loc { nullptr };
	XPLMDataTypeID dataref_types { xplmType_Unknown };
	bool dataref_writable { false };
	T dataref_storage { };
	static constexpr size_type dataref_storage_max_size = ARRAY_SIZE;
};

} // namespace datarefw

#endif // DATAREFW_H