#pragma once

#include "Common.h"
#include "ManagedHandler.h"

class Debugger {
public:
	Debugger(ICorDebugManagedCallback* handler);
	~Debugger();

	// Handling lifecyle of debugger
	HRESULT Init();
	HRESULT Attach(int pid);
	HRESULT Run(std::wstring cmdline);
	HRESULT Stop();
	HRESULT Continue();
	HRESULT Join();
	HRESULT Detach();
	
	// Handling .NET assembly enumeration
	HRESULT GetProcess(ICorDebugProcess **process);

	// Static methods
	static HRESULT GetAppDomains(std::vector<ICorDebugAppDomain*>** appDomains, ICorDebugProcess* process);
	static HRESULT GetAssemblies(std::vector<ICorDebugAssembly*>** assemblies, ICorDebugAppDomain* appDomain);
	static HRESULT GetModules(std::vector<ICorDebugModule*>** modules, ICorDebugAssembly* assembly);
	static HRESULT GetMetaDataInterface(IMetaDataImport **metadata, ICorDebugModule *module);
	static HRESULT GetThreads(std::vector<ICorDebugThread *> **threads, ICorDebugProcess *process);
	static HRESULT CreateStepper(ICorDebugStepper **steper, ICorDebugThread *thread);
	static HRESULT FindAssemblyByName(ICorDebugAssembly **assembly, std::vector<ICorDebugAssembly *> *assemblies, std::wstring name);
	static HRESULT FindMethod(ICorDebugFunction **function, ICorDebugAppDomain *appDomain, std::wstring assemblyName, std::wstring typeName, std::wstring methodName, DWORD _sigLen);
	static HRESULT GetSteppers(std::vector<ICorDebugStepper *> **steppers, ICorDebugAppDomain *appDomain);
	static HRESULT FindModuleByName(ICorDebugModule **module, std::vector<ICorDebugModule *> *_modules, ICorDebugAppDomain *appDomain, std::vector<ICorDebugAssembly *> *assemblies, std::wstring name);

private:
	HRESULT addHandler(ICorDebugManagedCallback* handler);

	ICorDebug* debug = NULL;
	ICorDebugProcess* process = NULL;
	ICorDebugManagedCallback* handler = NULL;
};