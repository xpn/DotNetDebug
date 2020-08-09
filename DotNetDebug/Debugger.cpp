#include "Debugger.h"

std::atomic<bool> done = false;

Debugger::Debugger(ICorDebugManagedCallback *handler)
{
	this->handler = handler;
}

Debugger::~Debugger()
{
	if (this->process)
	{
		this->process->Stop(0);
		this->process->Detach();
	}
}

HRESULT Debugger::Init()
{
	ICLRMetaHost *metaHost = NULL;
	IEnumUnknown *runtime = NULL;
	ICLRRuntimeInfo *runtimeInfo = NULL;
	ICLRRuntimeHost *runtimeHost = NULL;
	IUnknown *enumRuntime = NULL;
	LPWSTR frameworkName = NULL;
	DWORD bytes = 2048, result = 0;
	HRESULT hr;

	if ((hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, (LPVOID *)&metaHost)) != S_OK)
	{
		return hr;
	}

	if ((hr = metaHost->EnumerateInstalledRuntimes(&runtime)) != S_OK)
	{
		return hr;
	}

	frameworkName = (LPWSTR)LocalAlloc(LPTR, 2048);
	if (frameworkName == NULL)
	{
		return E_OUTOFMEMORY;
	}

	// Enumerate through runtimes and show supported frameworks
	while (runtime->Next(1, &enumRuntime, 0) == S_OK)
	{
		if (enumRuntime->QueryInterface<ICLRRuntimeInfo>(&runtimeInfo) == S_OK)
		{
			if (runtimeInfo != NULL)
			{
				runtimeInfo->GetVersionString(frameworkName, &bytes);
				wprintf(L"[*] Supported Framework: %s\n", frameworkName);
			}
		}
	}

	// Create our debugging interface
	if ((hr = runtimeInfo->GetInterface(CLSID_CLRDebuggingLegacy, IID_ICorDebug, (LPVOID *)&this->debug)) != S_OK)
	{
		return hr;
	}

	// Set our status to running
	//this->isRunning = true;

	// Initialise the debugger
	this->debug->Initialize();
	return this->debug->SetManagedHandler(this->handler);
}

HRESULT Debugger::Stop()
{
	return this->process->Stop(0);
}

HRESULT Debugger::Continue()
{
	return this->process->Continue(0);
}

HRESULT Debugger::addHandler(ICorDebugManagedCallback *handler)
{
	// Assign our managed handler
	ICorDebugManagedCallback *pHandler = new ManagedCallback();
	return debug->SetManagedHandler(pHandler);
}

HRESULT Debugger::Attach(int pid)
{
	return debug->DebugActiveProcess(pid, false, &process);
}

HRESULT Debugger::Detach()
{
	//this->process->Detach();
	done = true;
	return S_OK;
}

HRESULT Debugger::Run(std::wstring cmdline)
{
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	HRESULT hr;

	memset(&si, 0, sizeof(STARTUPINFOA));
	si.cb = sizeof(STARTUPINFOA);

	hr = debug->CreateProcessW(
		NULL,
		const_cast<LPWSTR>(cmdline.c_str()),
		NULL,
		NULL,
		false,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi,
		(CorDebugCreateProcessFlags)0,
		&this->process);

	return hr;
}

HRESULT Debugger::GetAppDomains(std::vector<ICorDebugAppDomain *> **appDomains, ICorDebugProcess *process)
{
	ICorDebugAppDomainEnum *appDomainsEnum = NULL;
	ICorDebugAppDomain *_appDomains[10];
	DWORD _count;
	HRESULT result;

	if (process == NULL)
	{
		return E_INVALIDARG;
	}

	// Get a list of AppDomains
	if (!SUCCEEDED((result = process->EnumerateAppDomains(&appDomainsEnum))))
	{
		return result;
	}

	if (!SUCCEEDED((result = appDomainsEnum->Next(10, _appDomains, &_count))))
	{
		return result;
	}

	*appDomains = new std::vector<ICorDebugAppDomain *>(_appDomains, _appDomains + _count);

	return S_OK;
}

HRESULT Debugger::GetAssemblies(std::vector<ICorDebugAssembly *> **assemblies, ICorDebugAppDomain *appDomain)
{
	ICorDebugAssemblyEnum *assemblyEnum;
	ICorDebugAssembly *_assemblies[100];
	DWORD _count;
	HRESULT result;

	if (assemblies == NULL || appDomain == NULL)
	{
		return E_INVALIDARG;
	}

	// Get list of assemblies
	if (!SUCCEEDED((result = appDomain->EnumerateAssemblies(&assemblyEnum))))
	{
		return result;
	}

	if (!SUCCEEDED((result = assemblyEnum->Next(100, _assemblies, &_count))))
	{
		return result;
	}

	*assemblies = new std::vector<ICorDebugAssembly *>(_assemblies, _assemblies + _count);

	return S_OK;
}

HRESULT Debugger::GetModules(std::vector<ICorDebugModule *> **modules, ICorDebugAssembly *assembly)
{
	ICorDebugModuleEnum *modulesEnum;
	ICorDebugModule *_modules[100];
	DWORD _count;
	HRESULT result;

	if (modules == NULL || assembly == NULL)
	{
		return E_INVALIDARG;
	}

	// Get list of modules
	if (!SUCCEEDED((result = assembly->EnumerateModules(&modulesEnum))))
	{
		//return result;
	}

	if (!SUCCEEDED((result = modulesEnum->Next(10, _modules, &_count))))
	{
		//return result;
	}

	*modules = new std::vector<ICorDebugModule *>(_modules, _modules + _count);

	return S_OK;
}

HRESULT Debugger::GetMetaDataInterface(IMetaDataImport **metadata, ICorDebugModule *module)
{
	if (module == NULL || metadata == NULL)
	{
		return E_INVALIDARG;
	}

	return module->GetMetaDataInterface(IID_IMetaDataImport, (IUnknown **)&metadata);
}

HRESULT Debugger::GetProcess(ICorDebugProcess **process)
{
	*process = this->process;

	return S_OK;
}

HRESULT Debugger::GetSteppers(std::vector<ICorDebugStepper *> **steppers, ICorDebugAppDomain *appDomain)
{
	ICorDebugStepperEnum *stepperEnum;
	HRESULT result;
	ICorDebugStepper *_steppers[100];
	DWORD _count;

	if (steppers == NULL || appDomain == NULL)
	{
		return E_INVALIDARG;
	}

	if (!SUCCEEDED((result = appDomain->EnumerateSteppers(&stepperEnum))))
	{
		return result;
	}

	if (!SUCCEEDED((result = stepperEnum->Next(100, _steppers, &_count))))
	{
		return result;
	}

	*steppers = new std::vector<ICorDebugStepper *>(_steppers, _steppers + _count);
	return S_OK;
}

HRESULT Debugger::GetThreads(std::vector<ICorDebugThread *> **threads, ICorDebugProcess *process)
{
	ICorDebugThreadEnum *threadEnum;
	ICorDebugThread *_threads[100];
	DWORD _count;
	HRESULT result;

	if (threads == NULL || process == NULL)
	{
		return E_INVALIDARG;
	}

	if (!SUCCEEDED((result = process->EnumerateThreads(&threadEnum))))
	{
		return result;
	}

	if (!SUCCEEDED((result = threadEnum->Next(100, _threads, &_count))))
	{
		return result;
	}

	*threads = new std::vector<ICorDebugThread *>(_threads, _threads + _count);
	return S_OK;
}

HRESULT Debugger::CreateStepper(ICorDebugStepper **stepper, ICorDebugThread *thread)
{
	return thread->CreateStepper(stepper);
}

HRESULT Debugger::Join()
{
	std::vector<ICorDebugStepper *> *steppers;
	std::vector<ICorDebugAppDomain *> *appDomains;

	HRESULT hr;
	while (!done.load())
	{
		Sleep(2000);
	}

	this->process->Stop(0);

	// Get an AppDomain
	Debugger::GetAppDomains(&appDomains, this->process);

	// Get current steppers
	Debugger::GetSteppers(&steppers, appDomains->at(0));

	for (auto stepper : *steppers)
	{
		stepper->Deactivate();
	}

	if (!SUCCEEDED((hr = this->process->Detach())))
	{
		printf("[!] Error detaching: HR [%x]\n", hr);
		return E_FAIL;
	}

	std::cout << "[*] Detached..." << std::endl;

	return S_OK;
}

HRESULT Debugger::FindAssemblyByName(ICorDebugAssembly **assembly, std::vector<ICorDebugAssembly *> *assemblies, std::wstring name)
{
	ULONG32 inputLen = 1024;
	WCHAR assemblyName[1024];
	ULONG32 outputLen = 0;

	for (int i = 0; i < assemblies->size(); i++)
	{
		if (assemblies->at(i)->GetName(inputLen, &outputLen, assemblyName) == S_OK)
		{
			std::wstring asmName(assemblyName);
			if (asmName.find(name.c_str(), 0) != std::string::npos)
			{
				// We have found our target assembly
				*assembly = assemblies->at(i);
				return S_OK;
			}
		}
	}

	return E_FAIL;
}

HRESULT Debugger::FindModuleByName(ICorDebugModule **module, std::vector<ICorDebugModule *> *_modules, ICorDebugAppDomain *appDomain, std::vector<ICorDebugAssembly *> *assemblies, std::wstring name)
{
	ULONG32 inputLen = 1024;
	WCHAR moduleName[1024];
	ULONG32 outputLen = 0;
	std::vector<ICorDebugModule *> *modules = NULL;

	for (int y = 0; y < assemblies->size(); y++)
	{
		if (Debugger::GetModules(&modules, assemblies->at(y)) == S_OK)
		{
			for (int i = 0; i < modules->size(); i++)
			{
				if (modules->at(i)->GetName(inputLen, &outputLen, moduleName) == S_OK)
				{
					std::wstring modName(moduleName);
					if (modName.find(name.c_str(), 0) != std::string::npos)
					{
						// We have found our target assembly
						*module = modules->at(i);
						return S_OK;
					}
				}
			}
		}

		
	}
	return E_FAIL;
}

	HRESULT Debugger::FindMethod(ICorDebugFunction * *function, ICorDebugAppDomain * appDomain, std::wstring assemblyName, std::wstring typeName, std::wstring methodName, DWORD _sigLen)
	{
		ICorDebugModule *module = NULL;
		std::vector<ICorDebugAssembly *> *assemblies = NULL;
		std::vector<ICorDebugModule *> *modules = NULL;
		mdTypeDef typeDef;
		WCHAR name[1024];
		DWORD nameLen;
		DWORD flags;
		PCCOR_SIGNATURE sig;
		DWORD sigLen;
		DWORD rva;
		DWORD implFlags;
		HRESULT hr;
		IMetaDataImport *metadata;
		HCORENUM enumnum = 0;
		mdMethodDef methodDefs[1000];
		DWORD count;

		if (GetAssemblies(&assemblies, appDomain) != S_OK)
		{
			return E_INVALIDARG;
		}

		for (auto ass : *assemblies) {
			if (Debugger::FindModuleByName(&module, modules, appDomain, assemblies, assemblyName) == S_OK) {
				break;
			}
		}

		if (module == NULL) {
			return E_FAIL;
		}

		module->GetMetaDataInterface(IID_IMetaDataImport, (IUnknown **)&metadata);
		
		if (!SUCCEEDED((hr = metadata->FindTypeDefByName(typeName.c_str(), NULL, &typeDef)))) {
			return E_FAIL;
		}

		if (!SUCCEEDED((hr = metadata->EnumMethods(&enumnum, typeDef, methodDefs, 1000, &count)))) {
			return E_FAIL;
		}

		for (auto methodDef : methodDefs)
		{
			metadata->GetMethodProps(methodDef, &typeDef, name, 1024, &nameLen, &flags, &sig, &sigLen, &rva, &implFlags);
			if (wcsncmp(methodName.c_str(), name, methodName.size() + 1) == 0)
			{
				if (_sigLen == 0 || _sigLen == sigLen)
				{
					module->GetFunctionFromToken(methodDef, function);
					return S_OK;
				}
			}
		}

		return E_FAIL;
	}