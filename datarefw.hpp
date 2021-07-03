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
// The main types associated with DataRefs are what are supported, if you try
// to use an unsupported type, you'll get a compile-time error. A string DataRef is,
// under the hood, a byte DataRef.
//
// Supported types:
//		int
//		float
//		double
//		std::vector<int>
//		std::vector<float>
//		std::string

#ifndef DATAREFW_H
#define DATAREFW_H

#include <XPLMDataAccess.h>
#include <XPLMUtilities.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace datarefw {

namespace intls {

template <typename U>
struct dr_type_is_int : std::integral_constant<bool, std::is_same<int, U>::value> {};

template <typename U>
struct dr_type_is_float : std::integral_constant<bool, std::is_same<float, U>::value> {};

template <typename U>
struct dr_type_is_double : std::integral_constant<bool, std::is_same<double, U>::value> {};

template <typename U>
struct dr_type_is_int_array
    : std::integral_constant<bool, std::is_same<std::vector<int>, U>::value> {};

template <typename U>
struct dr_type_is_float_array
    : std::integral_constant<bool, std::is_same<std::vector<float>, U>::value> {};

template <typename U>
struct dr_type_is_byte : std::integral_constant<bool, std::is_same<std::string, U>::value> {};

template <typename U>
struct dr_type_is_valid : std::integral_constant<bool,
                              dr_type_is_int<U>::value || dr_type_is_float<U>::value ||
                                  dr_type_is_double<U>::value || dr_type_is_int_array<U>::value ||
                                  dr_type_is_float_array<U>::value || dr_type_is_byte<U>::value> {};

template <typename U>
using enable_if_array_t =
    typename std::enable_if_t<dr_type_is_int_array<U>::value || dr_type_is_float_array<U>::value,
        U>;

template <typename U>
using enable_if_byte_t = typename std::enable_if_t<dr_type_is_byte<U>::value, U>;

template <typename U>
using enable_if_number_t = typename std::enable_if_t<
    dr_type_is_int<U>::value || dr_type_is_float<U>::value || dr_type_is_double<U>::value, U>;

template <typename U>
using enable_if_int_t = typename std::enable_if_t<dr_type_is_int<U>::value, U>;

template <typename U>
using enable_if_float_t = typename std::enable_if_t<dr_type_is_float<U>::value, U>;

template <typename U>
using enable_if_double_t = typename std::enable_if_t<dr_type_is_double<U>::value, U>;

template <typename U>
using enable_if_int_array_t = typename std::enable_if_t<dr_type_is_int_array<U>::value, U>;

template <typename U>
using enable_if_float_array_t = typename std::enable_if_t<dr_type_is_float_array<U>::value, U>;

template <typename U>
using enable_if_byte_t = typename std::enable_if_t<dr_type_is_byte<U>::value, U>;

template <typename U, typename... Args>
struct pack_contains {
    static constexpr bool value{(std::is_same_v<U, Args> || ...)};
};

template <typename Us, typename... Args>
using enable_if_listed_t = typename std::enable_if_t<pack_contains<Us, Args...>::value>;

template <typename T>
constexpr void verify_template_types() {
    static_assert((std::is_same_v<int, T> || std::is_same_v<float, T> ||
                      std::is_same_v<double, T> || std::is_same_v<std::vector<int>, T> ||
                      std::is_same_v<std::vector<float>, T> || std::is_same_v<std::string, T>),
        "Unsupported Type");
}

constexpr std::string_view dataref_type_to_str(const XPLMDataTypeID dataref_type) {
    switch (dataref_type) {
        case xplmType_Unknown:
            return "xplmType_Unknown";
        case xplmType_Int:
            return "xplmType_Int";
        case xplmType_Float:
            return "xplmType_Float";
        case xplmType_Double:
            return "xplmType_Double";
        case xplmType_FloatArray:
            return "xplmType_FloatArray";
        case xplmType_IntArray:
            return "xplmType_IntArray";
        case xplmType_Data:
            return "xplmType_Data";
    }

    return {};
}

inline void verify_dataref_string(const std::string &str) {
    if (str.empty()) {
        throw std::runtime_error("String is empty");
    }

    if (str.find(' ') != std::string::npos) {
        throw std::runtime_error("String can't have any white space");
    }
}

template <typename CurType>
constexpr void verify_dataref_type(const XPLMDataTypeID dataref_type) {
    auto type_verifier = [dataref_type](bool p) {
        if (!p) {
            const std::string err_msg =
                "Data type mismatch: expected " + std::string(typeid(CurType).name()) +
                " but instead got: " + std::string(dataref_type_to_str(dataref_type));
            throw std::runtime_error(err_msg);
        }
    };

    switch (dataref_type) {
        case xplmType_Unknown:
            throw std::runtime_error("Unknown DataRef type");
        case xplmType_Int:
            type_verifier(std::is_same_v<int, CurType>);
            break;
        case xplmType_Float:
            type_verifier(std::is_same_v<float, CurType>);
            break;
        case xplmType_Double:
            type_verifier(std::is_same_v<double, CurType>);
            break;
        case xplmType_FloatArray:
            type_verifier(std::is_same_v<std::vector<float>, CurType>);
            break;
        case xplmType_IntArray:
            type_verifier(std::is_same_v<std::vector<int>, CurType>);
            break;
        case xplmType_Data:
            type_verifier(std::is_same_v<std::string, CurType>);
            break;
    }
}

}  // namespace intls

class BaseDataRef {
public:
    virtual bool writable() const noexcept { return dataref_writable; }
    virtual ~BaseDataRef() = default;

protected:
    virtual void verify_dataref_is_good() const {
        if (!dataref_loc) {
            throw std::runtime_error("DataRef not valid");
        }
    }

    virtual void verify_dataref_is_writable() const {
        if (!writable()) {
            throw std::runtime_error("DataRef not writable");
        }
    }

    std::string dataref_name;
    XPLMDataRef dataref_loc{nullptr};
    XPLMDataTypeID dataref_types{xplmType_Unknown};
    bool dataref_writable{false};
};

template <typename DrType>
class FindDataRef : public BaseDataRef {
public:
    FindDataRef() = default;
    explicit FindDataRef(const std::string &dataref) {
        intls::verify_template_types<DrType>();
        intls::verify_dataref_string(dataref);

        dataref_loc = XPLMFindDataRef(dataref.c_str());

        if (!dataref_loc) {
            const std::string msg = "Failed to find DataRef " + dataref;
            throw std::runtime_error(msg);
        }

        dataref_types = XPLMGetDataRefTypes(dataref_loc);
        intls::verify_dataref_type<DrType>(dataref_types);

        dataref_writable = XPLMCanWriteDataRef(dataref_loc);
        dataref_found = true;
    }
    FindDataRef(const FindDataRef<DrType> &dr) = default;
    FindDataRef(FindDataRef<DrType> &&dr) = default;

    FindDataRef<DrType> &operator=(const FindDataRef<DrType> &dr) = default;
    FindDataRef<DrType> &operator=(FindDataRef<DrType> &&dr) = default;
    operator DrType() const { return dataref_get_value(); }
    explicit operator bool() const noexcept { return dataref_found; }
    friend std::ostream &operator<<(std::ostream &os, const FindDataRef<DrType> &obj) {
        os << obj.dataref_get_value();
        return os;
    }

    template <typename U = DrType, typename = intls::enable_if_array_t<U>>
    std::size_t size() const {
        return dataref_get_size();
    }
    template <typename U = DrType, typename = intls::enable_if_array_t<U>>
    typename DrType::value_type get_index_value(const std::size_t index) {
        return dataref_get_value(index);
    }

    ~FindDataRef() override = default;

private:
    template <typename U = DrType, typename = intls::enable_if_int_t<U>>
    int dataref_get_value() const {
        verify_dataref_is_good();
        return XPLMGetDatai(dataref_loc);
    }

    template <typename U = DrType, typename = intls::enable_if_float_t<U>>
    float dataref_get_value() const {
        verify_dataref_is_good();
        return XPLMGetDataf(dataref_loc);
    }

    template <typename U = DrType, typename = intls::enable_if_double_t<U>>
    double dataref_get_value() const {
        verify_dataref_is_good();
        return XPLMGetDatad(dataref_loc);
    }

    template <typename U = DrType, typename = intls::enable_if_int_array_t<U>>
    std::vector<int> dataref_get_value() const {
        verify_dataref_is_good();
        const auto arr_size = XPLMGetDatavi(dataref_loc, nullptr, 0, 0);
        U arr_val(arr_size);
        XPLMGetDatavi(dataref_loc, arr_val.data(), 0, arr_size);
        return arr_val;
    }

    template <typename U = DrType, typename = intls::enable_if_float_array_t<U>>
    std::vector<float> dataref_get_value() const {
        verify_dataref_is_good();
        const auto arr_size = XPLMGetDatavf(dataref_loc, nullptr, 0, 0);
        U arr_val(arr_size);
        XPLMGetDatavf(dataref_loc, arr_val.data(), 0, arr_size);
        return arr_val;
    }

    template <typename U = DrType, typename = intls::enable_if_array_t<U>>
    typename DrType::value_type dataref_get_value(const std::size_t index) const {
        verify_dataref_is_good();
        typename DrType::value_type arr_val;
        std::size_t arr_size;

        if constexpr (intls::dr_type_is_int_array<U>::value) {
            arr_size = XPLMGetDatavi(dataref_loc, nullptr, 0, 0);
        } else {
            arr_size = XPLMGetDatavf(dataref_loc, nullptr, 0, 0);
        }

        if (index >= arr_size) {
            throw std::out_of_range("Array index is out of bounds");
        }

        if constexpr (intls::dr_type_is_int_array<U>::value) {
            XPLMGetDatavi(dataref_loc, &arr_val, index, 1);
        } else {
            XPLMGetDatavf(dataref_loc, &arr_val, index, 1);
        }

        return arr_val;
    }

    template <typename U = DrType, typename = intls::enable_if_byte_t<U>>
    std::string dataref_get_value() const {
        verify_dataref_is_good();
        const auto arr_size = XPLMGetDatab(dataref_loc, nullptr, 0, 0);

        if (arr_size == 0) {
            return {};
        }

        std::unique_ptr<char[]> dr_val = std::make_unique<char[]>(arr_size + 1);
        XPLMGetDatab(dataref_loc, dr_val.get(), 0, arr_size);

        if (dr_val[arr_size] != '\0') {
            dr_val[arr_size] = '\0';
        }

        return std::string(dr_val.get());
    }

    template <typename U = DrType, typename = intls::enable_if_array_t<U>>
    std::size_t dataref_get_size() {
        verify_dataref_is_good();
        std::size_t arr_size{};

        if constexpr (intls::dr_type_is_int_array<U>::value) {
            arr_size = XPLMGetDatavi(dataref_loc, nullptr, 0, 0);
        } else {
            arr_size = XPLMGetDatavf(dataref_loc, nullptr, 0, 0);
        }

        return arr_size;
    }

    template <typename U = DrType>
    void dataref_set_value(U value) {
        verify_dataref_is_good();
        verify_dataref_is_writable();

        if constexpr (std::is_same_v<U, intls::dr_type_is_int>) {
            XPLMSetDatai(dataref_loc, value);
        } else if constexpr (std::is_same_v<U, intls::dr_type_is_float>) {
            XPLMSetDataf(dataref_loc, value);
        } else if constexpr (std::is_same_v<U, intls::dr_type_is_double>) {
            XPLMSetDatad(dataref_loc, value);
        } else if constexpr (std::is_same_v<U, intls::dr_type_is_int_array>) {
            XPLMSetDatavi(dataref_loc, value.data(), 0, value.size());
        } else if constexpr (std::is_same_v<U, intls::dr_type_is_float_array>) {
            XPLMSetDatavf(dataref_loc, value.data(), 0, value.size());
        } else if constexpr (std::is_same_v<U, intls::dr_type_is_byte>) {
            XPLMSetDatab(dataref_loc, static_cast<void *>(value.data()), 0, value.size());
        }
    }

    bool dataref_found{false};
};

}  // namespace datarefw

#endif  // DATAREFW_H