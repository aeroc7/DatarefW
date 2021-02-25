#include <datarefw.hpp>

#include <XPLMPlugin.h>
#include <XPLMUtilities.h>

#include <cstring>
#include <memory>
#include <string>

using namespace datarefw;

class DatarefDatabase {
public:
	DatarefDatabase() {
		my_int_array_dataref.create_dataref("testing/test_int_array_dr");
		my_string_dataref.create_dataref("testing/test_string_dr", true);
		my_string_dataref = "abcdefghijklmnopqrstuvwxyz";
			// Equality operator
			DATAREFW_ASSERT(my_string_dataref == "abcdefghijklmnopqrstuvwxyz");
		find_my_int_dataref.find_dataref("testing/test_int_dr");
		find_my_string.find_dataref("testing/test_string_d");
		
		// same as if (find_my_string.found()) {
		if (find_my_string) {
			DATAREFW_ASSERT(find_my_string == "abcdefghijklmnopqrstuvwxyz");
		}

		int a = 1, b = 2, c = 3;

		if (find_my_int_dataref.found() && find_my_int_dataref.writable()) {
			find_my_int_dataref = 0;
			// Increment/decrement
			find_my_int_dataref++;
				DATAREFW_ASSERT(find_my_int_dataref == 1);
			find_my_int_dataref--;
				DATAREFW_ASSERT(find_my_int_dataref == 0);
			// Compound assignment
			find_my_int_dataref += (99 + a + (b - c));
				DATAREFW_ASSERT(find_my_int_dataref == 99);
			find_my_int_dataref *= (7 + a);
				DATAREFW_ASSERT(find_my_int_dataref == 792);
			find_my_int_dataref /= (4 + b) - c;
				DATAREFW_ASSERT(find_my_int_dataref == 264);
			find_my_int_dataref -= 73 - c;
				DATAREFW_ASSERT(find_my_int_dataref == 194);
		}

		// Use as normal string
		std::string my_dr_string = my_string_dataref;

		if (my_dr_string.find('b') != std::string::npos) {
			// etc.
			my_dr_string += "123456789";
			// Set dataref from string value
			my_string_dataref = my_dr_string;
		}

		// Array functionality (built on top of vector)
		for (size_t i = 0; i < my_int_array_dataref.size(); ++i) {
			my_int_array_dataref[i] = i;
			// OR alternatively (if using pointer to the class or something)
			// my_int_array_dataref.at(i) = i // (returns reference)
		}

		// my_int_array_dataref[25] = 56; // assertion tripped, trying to overstep our bounds
	}

private:
	// Create using constructor with dataref path and optional 'pis_writable' (defaults to FALSE)
	CreateDataref<int> my_int_dataref { "testing/test_int_dr", true };
	// Create using default constructor (requires separate call to create it)
	CreateDataref<DrIntArr, 25> my_int_array_dataref; // Create with 25 array elements (0...24)
	CreateDataref<std::string> my_string_dataref;

	FindDataref<int> find_my_int_dataref;
	FindDataref<std::string> find_my_string;
};

DatarefDatabase dr_dbase;

PLUGIN_API int
XPluginStart(char *outName, char *outSig, char *outDesc) {
	strcpy(outName, "Dataref Test");
	strcpy(outSig, "aeroc7.dataref.tests");
	strcpy(outDesc, "Testing Dataref Wrapper Functionality");

	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);

	return 1;
}

PLUGIN_API void
XPluginStop() {
	
}

PLUGIN_API void
XPluginDisable() {

}

PLUGIN_API int
XPluginEnable() {
	return 1;
}

PLUGIN_API void
XPluginReceiveMessage(XPLMPluginID /*inFrom*/, int /*inMsg*/, void * /*inParam*/) {

}