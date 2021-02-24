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
// 	- DATAREFW_HARD_ASSERT_FAIL		// - If defined, Assertion will fail if
// 						// DataRef can not be found

#ifndef _DATAREFW_H_
#define _DATAREFW_H_

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

#include <type_traits>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#define DATAREFW_UNUSED(a) (void)(a)

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

typedef std::vector<int> DrIntArr;
typedef std::vector<float> DrFloatArr;

namespace datarefw_utils_ {

	template <typename T>
	constexpr auto
	verify_types() -> void {
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
}

template <typename T>
class FindDataref {
public:
	void
	find_dataref(const std::string& dr_str) {
		dataref_name = dr_str;
		impl_find_dataref();
	}

	FindDataref() {
		datarefw_utils_::verify_types<T>();
	}

	FindDataref(const std::string& dr_str) {
		datarefw_utils_::verify_types<T>();
		find_dataref(dr_str);
	}

	FindDataref(const FindDataref<T>& dr_o) = default;
	FindDataref(FindDataref<T>&& dr_o) = default;
	FindDataref<T>& operator=(const FindDataref<T>& dr_o) = default;
	FindDataref<T>& operator=(FindDataref<T>&& dr_o) = default;

	friend std::ostream&
	operator<<(std::ostream& os, const FindDataref<T>& val) {
		DATAREFW_ASSERT(val.found());
		// TODO: array datarefs

		switch(val.dataref_types) {
			case xplmType_Int:
				os << val.get_dr_val_i();
				break;
			case xplmType_Float:
				os << val.get_dr_val_f();
				break;
			case xplmType_Double:
				os << val.get_dr_val_d();
				break;
			case xplmType_Data:
				os << val.get_dr_val_str();
				break;
		};

		return os;
	}

	friend FindDataref<T>&
	operator+(const int val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_i();
		nval += val;
		obj.set_dr_val_i(nval);
		return obj;
	}

	friend FindDataref<T>&
	operator+(const float val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_f();
		nval += val;
		obj.set_dr_val_f(nval);
		return obj;
	}

	friend FindDataref<T>&
	operator+(const double val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_d();
		nval += val;
		obj.set_dr_val_d(nval);
		return obj;
	}

	friend FindDataref<T>&
	operator+(const std::string& val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_str();
		nval += val;
		obj.set_dr_val_str(nval);
		return obj;
	}

	FindDataref<T>&
	operator+=(const int val) {
		auto nval = get_dr_val_i();
		nval += val;
		set_dr_val_i(nval);
		return *this;
	}

	FindDataref<T>&
	operator+=(const float val) {
		auto nval = get_dr_val_f();
		nval += val;
		set_dr_val_f(nval);
		return *this;
	}

	FindDataref<T>&
	operator+=(const double val) {
		auto nval = get_dr_val_d();
		nval += val;
		set_dr_val_d(nval);
		return *this;
	}

	FindDataref<T>&
	operator+=(const std::string& val) {
		auto nval = get_dr_val_str();
		nval += val;
		set_dr_val_str(nval);
		return *this;
	}
	
	friend FindDataref<T>&
	operator-(const int val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_i();
		nval -= val;
		obj.set_dr_val_i(nval);
		return obj;
	}

	friend FindDataref<T>&
	operator-(const float val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_f();
		nval -= val;
		obj.set_dr_val_f(nval);
		return obj;
	}

	friend FindDataref<T>&
	operator-(const double val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_d();
		nval -= val;
		obj.set_dr_val_d(nval);
		return obj;
	}

	constexpr FindDataref<T>&
	operator=(const int val) {
		set_dr_val_i(val);
		return *this;
	}

	constexpr FindDataref<T>&
	operator=(const float val) {
		set_dr_val_f(val);
		return *this;
	}

	constexpr FindDataref<T>&
	operator=(const double val) {
		set_dr_val_d(val);
		return *this;
	}

	constexpr
	operator int() const noexcept {
		return get_dr_val_i();
	}

	constexpr
	operator float() const noexcept {
		return get_dr_val_f();
	}

	constexpr
	operator double() const noexcept {
		return get_dr_val_d();
	}

	constexpr
	operator std::string() const {
		return get_dr_val_str();
	}

	constexpr explicit
	operator bool() const noexcept {
		return found();
	}

	bool found() const noexcept {
		return dataref_found;
	}

	bool is_writable() const noexcept {
		return dataref_writable;
	}

	~FindDataref() {

	}
private:
	void impl_find_dataref() {
		DATAREFW_ASSERT(dataref_name != "");

		dataref_loc = XPLMFindDataRef(dataref_name.c_str());

		if (dataref_loc == nullptr) {
#ifdef DATAREFW_HARD_ASSERT_FAIL
			DATAREFW_ASSERT(dataref_loc != nullptr);
#endif // DATAREFW_HARD_ASSERT_FAIL
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
				DATAREFW_ASSERT(
					(std::is_same<int, T>::value)  ||
					(std::is_same<float, T>::value)||
					(std::is_same<double, T>::value)
				);
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
	verify_dr_can_write() {
		DATAREFW_ASSERT(is_writable != false);
	}

	// Get Integer DataRef
	int
	get_dr_val_i() const {
		return XPLMGetDatai(dataref_loc);
	}
	// Set Integer DataRef
	void
	set_dr_val_i(const int val) {
		verify_dr_can_write();
		XPLMSetDatai(dataref_loc, val);
	}
	// Get Float DataRef
	float
	get_dr_val_f() const {
		return XPLMGetDataf(dataref_loc);
	}
	// Set Float DataRef
	void
	set_dr_val_f(const float val) {
		verify_dr_can_write();
		XPLMSetDataf(dataref_loc, val);
	}
	// Get Double DataRef
	double
	get_dr_val_d() const {
		return XPLMGetDatad(dataref_loc);
	}
	// Set Double DataRef
	void
	set_dr_val_d(const double val) {
		verify_dr_can_write();
		XPLMSetDatad(dataref_loc, val);
	}
	// Get String DataRef
	std::string
	get_dr_val_str() const {
		int byte_dr_sz = XPLMGetDatab(dataref_loc, nullptr, 0, 0);
		char *byte_dr_data = new char[byte_dr_sz];
		XPLMGetDatab(dataref_loc, byte_dr_data, 0, byte_dr_sz);

		if (byte_dr_data[byte_dr_sz + 1] != '\0') {
			byte_dr_data[byte_dr_sz + 1] = '\0';
		}

		std::string byte_dr_str(byte_dr_data);
		delete[] byte_dr_data;
		return byte_dr_str;
	}
	// Set String DataRef
	void
	set_dr_val_str(const std::string& str) {
		verify_dr_can_write();
		XPLMSetDatab(dataref_loc, const_cast<char*> (str.c_str()), 0, str.length() + 1);
	}

	std::string dataref_name;
	XPLMDataRef dataref_loc { nullptr };
	XPLMDataTypeID dataref_types { xplmType_Unknown };
	bool dataref_writable { false };
	bool dataref_found { false };
};

template <typename U>
	struct dr_type_is_array :
		std::integral_constant<bool,
			std::is_same<DrIntArr, U>::value ||
			std::is_same<DrFloatArr, U>::value> {};


template <typename T, std::size_t ARRAY_SIZE = 0>
class CreateDataref {
public:
	CreateDataref() {

	}

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
	constexpr auto
	operator[](std::size_t index) -> val_type const {
		static_assert(index < ARRAY_SIZE, "Out of bounds access to an array is Undefined Behavior");
		return dataref_storage[index];
	}

	// Prefix increment (int only)
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	CreateDataref<T, ARRAY_SIZE>&
	operator++() {
		dataref_storage++;
		return *this;
	}

	// Postfix increment (int only)
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	CreateDataref<T, ARRAY_SIZE>
	operator++(int) {
		auto old = *this;
		operator++();
		return old;
	}

	// Prefix decrement (int only)
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	CreateDataref<T, ARRAY_SIZE>&
	operator--() {
		dataref_storage--;
		return *this;
	}

	// Postfix decrement (int only)
	template <typename U = T,
		typename std::enable_if<std::is_same<U, int>::value, U>::type* = nullptr>
	CreateDataref<T, ARRAY_SIZE>
	operator--(int) {
		auto old = *this;
		operator--();
		return old;
	}

	constexpr friend std::ostream&
	operator<<(std::ostream& os, const CreateDataref<T, ARRAY_SIZE>& obj) {
		os << obj.dataref_storage;
		return os;
	}

	constexpr
	operator T() const noexcept {
		return dataref_storage;
	}

	constexpr explicit
	operator bool() const noexcept {
		return (dataref_loc != nullptr);
	}

	~CreateDataref() {
		impl_dr_cleanup();
	}
private:
	
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

	template <typename U, std::size_t ARR_SIZE = 0>
	static CreateDataref<U, ARR_SIZE> *
	impl_proc_ref(void *refcon) {
		DATAREFW_ASSERT(refcon != nullptr);
		return reinterpret_cast<CreateDataref<U, ARR_SIZE> *> (refcon);
	}

	template <typename U = T, typename std::enable_if<dr_type_is_byte<U>::value, U>::type* = nullptr>
	static void
	impl_dr_write_byte(void *refcon, void *values, int offset, int count) {
		if (values == nullptr || offset >= count) {
			return;
		}

		DATAREFW_ASSERT(offset >= 0);

		auto ncount = count - offset;
		char *cvalues = static_cast<char *> (values);
		char *temp_val = new char[count];
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
	static int
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
	static int
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
	constexpr auto
	array_verif() -> void {
		static_assert(ARR_SIZE > 0, "Array size can't be zero.");
	}

	template <typename U = T, typename std::enable_if<!dr_type_is_array<U>::value, U>::type* = nullptr>
	constexpr auto
	array_verif() -> void {

	}

	void
	impl_create_dataref() {
		DATAREFW_ASSERT(dataref_name != "");
		DATAREFW_ASSERT(dataref_name.find(' ') == std::string::npos);

		datarefw_utils_::verify_types<T>();
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
};

} // namespace datarefw

#endif // _DATAREFW_H_