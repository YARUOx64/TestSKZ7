#include "stdafx.h"


using namespace std;


#define LOOP_COUNT	500000


#ifdef _M_X64
#define LOG_FILE	_T("result_x64.txt")
#define REG_COUNT	16
#else
#define LOG_FILE	_T("result_x86.txt")
#define REG_COUNT	8
#endif

#pragma pack(push, 4)
struct TestSKZ7Param {
	size_t Count;
	size_t RegBefore[REG_COUNT];	
	size_t RegAfter[REG_COUNT];
	uint8_t InitValue[4];
};
#pragma pack(pop)

extern "C" void __cdecl TestSKZ7(TestSKZ7Param* param);


struct ThreadParam {
	thread Threrad;
	HANDLE BeginEvent = NULL;
	HANDLE EndEvent = NULL;
	volatile bool Running;
	DWORD_PTR AffinityMask;
	TestSKZ7Param Param;
	size_t Detected;
};


class TestThread
{
private:
	vector<unique_ptr<ThreadParam>> m_ThreadList;
	volatile bool m_Running = true;
	bool m_Detected = false;
public:
	TestThread(DWORD cnt);
	~TestThread();
private:
	void TestThreadProc();
	static void _TestThreadProc(TestThread* obj) { obj->TestThreadProc(); }
public:
	void Run(const wstring path, bool Silent);
};


void ThreadProc(ThreadParam* param)
{
	SetThreadAffinityMask(GetCurrentThread(), param->AffinityMask);
STATE_BEGIN:
	WaitForSingleObject(param->BeginEvent, INFINITE);
	if (!param->Running)
		return;
	TestSKZ7(&param->Param);
	SetEvent(param->EndEvent);
	goto STATE_BEGIN;
}



bool GetCPUName(wchar_t* buf, size_t len)
{
	if (len <= 1)
		return false;
	int CPUInfo[4];
	__cpuid(CPUInfo, 0x80000000);
	uint32_t max = CPUInfo[0];
	if (max >= 0x80000002)
	{
		char namebuf[49];
		__cpuid(CPUInfo, 0x80000002);
		((uint32_t*)namebuf)[0] = CPUInfo[0];
		((uint32_t*)namebuf)[1] = CPUInfo[1];
		((uint32_t*)namebuf)[2] = CPUInfo[2];
		((uint32_t*)namebuf)[3] = CPUInfo[3];
		namebuf[16] = NULL;
		if (max >= 0x80000003)
		{
			__cpuid(CPUInfo, 0x80000003);
			((uint32_t*)namebuf)[4] = CPUInfo[0];
			((uint32_t*)namebuf)[5] = CPUInfo[1];
			((uint32_t*)namebuf)[6] = CPUInfo[2];
			((uint32_t*)namebuf)[7] = CPUInfo[3];
			namebuf[32] = NULL;
			if (max >= 0x80000004)
			{
				__cpuid(CPUInfo, 0x80000004);
				((uint32_t*)namebuf)[8] = CPUInfo[0];
				((uint32_t*)namebuf)[9] = CPUInfo[1];
				((uint32_t*)namebuf)[10] = CPUInfo[2];
				((uint32_t*)namebuf)[11] = CPUInfo[3];
				namebuf[48] = NULL;
			}
		}
		if (len > 48)
		{
			len = 48;
			namebuf[48] = NULL;
		}
		else
			namebuf[len-1] = NULL;
		for (size_t i = 0; i < len; i++)
		{
			if (namebuf[i] != NULL)
				buf[i] = (wchar_t)namebuf[i];
			else
			{
				buf[i] = NULL;
				break;
			}
		}
		return true;
	}
	else
		return false;
}


//TestThread

TestThread::TestThread(DWORD cnt)
{
	DWORD_PTR AffinityMask = 1;

	for (size_t i = 0; i < cnt; i++)
	{
		unique_ptr<ThreadParam> param(new ThreadParam);	
		param->BeginEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		param->EndEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		param->Running = true;
		param->AffinityMask = AffinityMask;
		AffinityMask <<= 1;
		param->Param.Count = LOOP_COUNT;
		param->Detected = 0;
		thread Threrad(ThreadProc, param.get());
		param->Threrad.swap(Threrad);
		m_ThreadList.push_back(unique_ptr<ThreadParam>(param.release()));	
	}
}

TestThread::~TestThread()
{
	for (size_t i = 0; i < m_ThreadList.size(); i++)
	{
		auto &param = m_ThreadList[i];
		param->Running = false;
		SetEvent(param->BeginEvent);
		param->Threrad.join();
		CloseHandle(param->BeginEvent);
		CloseHandle(param->EndEvent);		
	}
}

void TestThread::TestThreadProc()
{
	vector<HANDLE> WaitList(m_ThreadList.size());

	DWORD_PTR AffinityMask = 1;

	for (size_t i = 0; i < m_ThreadList.size(); i++)
	{
		auto &param = m_ThreadList[i];
		WaitList[i] = param->EndEvent;
	}

	size_t Count = 0;

	while (m_Running)
	{
		uint8_t InitValue = static_cast<uint8_t>(Count);
		Count++;

		for (size_t i = 0; i < m_ThreadList.size(); i++)
		{
			auto &param = m_ThreadList[i];
			param->Param.InitValue[0] = InitValue;
			param->Param.InitValue[1] = InitValue + 1;
			param->Param.InitValue[2] = InitValue + 2;
			param->Param.InitValue[3] = InitValue + 3;
			InitValue++;
		}

		for (size_t i = 0; i < m_ThreadList.size(); i++)
		{
			SetEvent(m_ThreadList[i]->BeginEvent);
		}

		WaitForMultipleObjects(static_cast<DWORD>(WaitList.size()), WaitList.data(), TRUE, INFINITE);

		for (size_t i = 0; i < m_ThreadList.size(); i++)
		{
			auto &param = m_ThreadList[i];

			for (size_t j = 0; j < 4; j++)
			{
				if ((param->Param.RegAfter[j] >> 16) != (param->Param.RegAfter[j] >> 16))
				{
					m_Detected = true;
					param->Detected |= static_cast<size_t>(1) << j;
				}
				else if ((param->Param.RegAfter[j] & 0xFF) != ((param->Param.RegAfter[j] >> 8) & 0xFF))
				{
					m_Detected = true;
					param->Detected |= static_cast<size_t>(1) << j;
				}
			}
			for (size_t j = 6; j < _countof(param->Param.RegAfter); j++)
			{
				if (param->Param.RegBefore[j] != param->Param.RegAfter[j])
				{
					m_Detected = true;
					param->Detected |= static_cast<size_t>(1) << j;
				}
			}
		}

		if (m_Detected)
			break;
	}
}

void TestThread::Run(const wstring path, bool Silent)
{
	wchar_t buf[1024];
	wchar_t CPUName[100];
	GetCPUName(CPUName, _countof(CPUName));

	if (!Silent)
	{
		wcout << CPUName;
		wcout << "\n\n";
	}

	//wcout << "\033[>5h";

	m_Running = true;
	thread TestThread(_TestThreadProc, this);

	DWORD Start = GetTickCount();
	while (true)
	{
		DWORD t = GetTickCount() - Start;
		DWORD h = t / 3600000;
		t -= h * 3600000;
		DWORD m = t / 60000;
		DWORD s = (t - h * 60000) /1000;

		swprintf_s(buf, _countof(buf), L"\r Elapsed Time: %0u:%2.2u:%2.2u\r", h, m, s);
		wcout << buf;


		Sleep(1000);
		if (_kbhit())
		{
			m_Running = false;
			break;
		}
		if (m_Detected)
			break;
	};

	TestThread.join();

	DWORD ElapsedTime = GetTickCount() - Start;

	wofstream strm(path);

	strm << CPUName;
	strm << "\n";

	if (!m_Detected)
	{
		strm << "NO ERROR";
		return;
	}

	{
		DWORD t = GetTickCount() - Start;
		DWORD h = t / 3600000;
		t -= h * 3600000;
		DWORD m = t / 60000;
		DWORD s = (t - h * 60000) /1000;

		swprintf_s(buf, _countof(buf), L"Elapsed Time: %0u:%2.2u:%2.2u\n", h, m, s);
		strm << buf;
	}


#ifdef _M_X64
	const wchar_t* RegName[14] = { L"RAX", L"RBX", L"RCX", L"RDX", L"RSP", L"RBP", L"R8", L"R9", L"R10", L"R11",L"R12", L"R13",L"R14", L"R15" };
#define REGFMT	L"  %-16s"
#define ADDRFMT		L"  %16.16llx"
#define ADDRFMT_HIT	L" *%16.16llx"
#else
	const wchar_t* RegName[6] = { L"EAX", L"EBX", L"ECX", L"EDX", L"ESP", L"EBP" };
#define REGFMT	L"  %-8s"
#define ADDRFMT		L"  %8.8x"
#define ADDRFMT_HIT	L" *%8.8x"
#endif

	strm << L"CORE      ";
	for (size_t i = 0; i < _countof(RegName); i++)
	{
		swprintf_s(buf, _countof(buf), REGFMT, RegName[i]);
		strm << buf;
	}
	strm << "\n";

	for (size_t i = 0; i < m_ThreadList.size(); i++)
	{
		auto &param = m_ThreadList[i];

		swprintf_s(buf, _countof(buf), L"#%-2d BEFORE", static_cast<int>(i));
		strm << buf;

		for (size_t j = 0; j < 4; j++)
		{
			swprintf_s(buf, _countof(buf), ADDRFMT, param->Param.RegBefore[j]);
			strm << buf;
		}
		for (size_t j = 6; j < _countof(param->Param.RegBefore); j++)
		{
			swprintf_s(buf, _countof(buf), ADDRFMT, param->Param.RegBefore[j]);
			strm << buf;
		}
		strm << "\n";

		swprintf_s(buf, _countof(buf), L"    AFTER ");
		strm << buf;

		for (size_t j = 0; j < 4; j++)
		{
			if ((param->Detected & ((size_t)1 << j)) != 0)
				swprintf_s(buf, _countof(buf), ADDRFMT_HIT, param->Param.RegAfter[j]);
			else
				swprintf_s(buf, _countof(buf), ADDRFMT, param->Param.RegAfter[j]);
			strm << buf;
		}
		for (size_t j = 6; j < _countof(param->Param.RegAfter); j++)
		{
			if ((param->Detected & ((size_t)1 << j)) != 0)
				swprintf_s(buf, _countof(buf), ADDRFMT_HIT, param->Param.RegAfter[j]);
			else
				swprintf_s(buf, _countof(buf), ADDRFMT, param->Param.RegAfter[j]);
			strm << buf;
		}
		strm << "\n";
	}

//STEATE_END:
//	if (!Silent)
//	{
//		wcout << "Press any key for exit";
//		while (true)
//		{
//			Sleep(100);
//			if (_kbhit())
//				break;
//		}
//	}
}


int _tmain(int argc, TCHAR* argv[])
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);

	//TestThread TestThread(1);
	TestThread TestThread(info.dwNumberOfProcessors);

	bool Silent = false;
	const TCHAR* filename = LOG_FILE;
	for (size_t i = 1; i < argc; i++)
	{
		if (argv[i][0] == _T('-'))
		{
			if (_tcsicmp(argv[i], _T("-S")) == 0)
			{
				Silent = true;
			}
		}
		else
			filename = argv[i];
	}



	wstring path;

	if (_tcschr(filename, _T('\\')) == nullptr)
	{
		TCHAR* p = _tcsrchr(argv[0], _T('\\'));
		if (p != nullptr)
			path = wstring(argv[0], p - argv[0] + 1) + filename;
	}
	
	if (path.empty())
		path = filename;

	TestThread.Run(path, Silent);

    return 0;
}

