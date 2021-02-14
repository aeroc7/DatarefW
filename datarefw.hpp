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
//
// This DataRef wrapper makes use of several 'utility' functions, of which you may
// define your own;
// 
//      - DATAREFW_ASSERT(cond)             // - Custom assert function
//      - DATAREFW_LOGGER(msg)              // - Logger function, called with a
// 											// single null-terminated c-string
// 
// This DataRef wrapper also makes use of several features that you may not want; 
// 
//      - DATAREFW_DISABLE_EXCEPTIONS       // - If defined, Disable C++ exceptions
// 											// explicitly thrown by us
//		- DATAREFW_HARD_ASSERT_FAIL			// - If defined, Assertion will fail if
//											// DataRef can not be found

#ifndef _DATAREFW_H_
#define _DATAREFW_H_

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

#include <string>
#include <cstring>
#include <iostream>
#include <vector>

#define DATAREFW_UNUSED(a) (void)(a)

#ifndef DATAREFW_LOGGER
# define __FILENAME__ (strrchr(__FILE__, '/') ? \
	strrchr(__FILE__, '/') + 1 : __FILE__)
namespace {
	void datarefw_log_msg(const std::string& msg, int line_number,
		const char *filename)
	{
		std::string fini_msg = "[DATAREFW_LOGGER] [";
		fini_msg += filename;
		fini_msg += ":";
		fini_msg += std::to_string(line_number);
		fini_msg += "]: " + msg;
		fini_msg += "\n";
		
		std::cout << fini_msg;
		std::cout << std::flush;
		XPLMDebugString(fini_msg.c_str());
	}
} // namespace
# define DATAREFW_LOGGER(msg) datarefw_log_msg(msg, __LINE__, __FILENAME__)
#endif // DATAREFW_LOGGER

#ifndef DATAREFW_ASSERT
# if defined(__GNUC__) || defined(__clang__)
#  include <cstdlib>
#  define DATAREFW_COND_UNLIKELY(x) __builtin_expect(x, 0)
#  define DATAREFW_ASSERT(x) \
	do { \
		if (DATAREFW_COND_UNLIKELY(!(x))) { \
			DATAREFW_LOGGER(std::string("Assertion " + std::string(#x) + " failed")); \
			abort(); \
		} \
	} while (0)
# else
# include <cassert>
# define DATAREFW_ASSERT(cond) assert(cond)
# endif // defined(__GNUC__) || defined(__clang__)
#endif // DATAREFW_ASSERT

namespace datarefw {

template <typename T>
class FindDataref {
public:
	using DrIntArr = std::vector<int>;
	using DrFloatArr = std::vector<float>;

	static_assert(
		(
			std::is_same<int, T>::value         ||
			std::is_same<float, T>::value       ||
			std::is_same<double, T>::value      ||
			std::is_same<DrIntArr, T>::value    ||
			std::is_same<DrFloatArr, T>::value  ||
			std::is_same<std::string, T>::value
		),
		"Unsupported type"
	);

	void
	find_dataref(const std::string& dr_str) {
		dataref_name = dr_str;
		impl_find_dataref();
	}

	FindDataref() {

	}

	FindDataref(const std::string& dr_str) {
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

	constexpr friend FindDataref<T>&
	operator+(const int val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_i();
		nval += val;
		obj.set_dr_val_i(nval);
		return obj;
	}

	constexpr friend FindDataref<T>&
	operator+(const float val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_f();
		nval += val;
		obj.set_dr_val_f(nval);
		return obj;
	}

	constexpr friend FindDataref<T>&
	operator+(const double val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_d();
		nval += val;
		obj.set_dr_val_d(nval);
		return obj;
	}

	constexpr friend FindDataref<T>&
	operator+(const std::string& val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_str();
		nval += val;
		obj.set_dr_val_str(nval);
		return obj;
	}

	constexpr friend FindDataref<T>&
	operator-(const int val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_i();
		nval -= val;
		obj.set_dr_val_i(nval);
		return obj;
	}

	constexpr friend FindDataref<T>&
	operator-(const float val, const FindDataref<T>& obj) {
		auto nval = obj.get_dr_val_f();
		nval -= val;
		obj.set_dr_val_f(nval);
		return obj;
	}

	constexpr friend FindDataref<T>&
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
	operator int() const {
		return get_dr_val_i();
	}

	constexpr
	operator float() const {
		return get_dr_val_f();
	}

	constexpr
	operator double() const {
		return get_dr_val_d();
	}

	constexpr
	operator std::string() const {
		return get_dr_val_str();
	}

	constexpr
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
#ifndef DATAREFW_DISABLE_EXCEPTIONS
			throw std::runtime_error(std::string("Failed to find DataRef " +
				dataref_name).c_str());
#else
			DATAREFW_LOGGER(std::string("Failed to find DataRef " +
				dataref_name).c_str())
# ifdef DATAREFW_HARD_ASSERT_FAIL
			DATAREFW_ASSERT(dataref_loc != nullptr);
# endif // DATAREFW_HARD_ASSERT_FAIL
			return;
#endif // DATAREFW_DISABLE_EXCEPTIONS
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
					(std::is_same<int, T>::value) 		||
					(std::is_same<float, T>::value)		||
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
#ifndef DATAREFW_DISABLE_EXCEPTIONS
		if (!is_writable()) {
			throw std::runtime_error("DataRef is not writable");
		}
#else
		DATAREFW_ASSERT(is_writable());
#endif // DATAREFW_DISABLE_EXCEPTIONS
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

} // namespace datarefw

#endif // _DATAREFW_H_