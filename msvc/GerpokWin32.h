#/*include <windows.h>
#include <boost/static_assert.hpp>

template <class _Type>
class DebugPolicy
{
public:
	void DoOutput(const _Type* output) { BOOST_STATIC_ASSERT(false); };
};

template <>
void DebugPolicy<char>::DoOutput(const char* output) 
{
	OutputDebugStringA(output);
	OutputDebugStringA("\n");
};

template <>
void DebugPolicy<wchar_t>::DoOutput(const wchar_t* output) 
{
	OutputDebugStringW(output);
	OutputDebugStringW(L"\n");
};



template <class _Type>
class InfoPolicy {
public:
	void DoOutput(const _Type* output) {
		MessageBox(0,output, "Gerpok Info", MB_OK);
		//OutputDebugStringA("\n");
	};
};

template <class _Type>
class LogPolicy {
public:
	void DoOutput(const _Type* output) {
		OutputDebugStringA("[LOG]");
		OutputDebugStringA(output);
		OutputDebugStringA("\n");
	};
};*/