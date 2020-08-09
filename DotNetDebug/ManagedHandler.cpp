#include "ManagedHandler.h"

#define COM_METHOD HRESULT STDMETHODCALLTYPE
ULONG m_ref = 1;

HRESULT ManagedCallback::QueryInterface(REFIID iid, void **ppv)
{
	if (iid == IID_IUnknown || iid == IID_ICorDebugManagedCallback)
	{
		*ppv = static_cast<ICorDebugManagedCallback *>(this);
	}
	else if (iid == IID_ICorDebugManagedCallback2)
	{
		*ppv = static_cast<ICorDebugManagedCallback2 *>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	reinterpret_cast<IUnknown *>(*ppv)->AddRef();

	return S_OK;
}

ULONG ManagedCallback::AddRef(void)
{
	InterlockedIncrement(&m_ref);
	return m_ref;
}

ULONG ManagedCallback::Release(void)
{
	ULONG ulRefCount = InterlockedDecrement(&m_ref);

	if (0 == m_ref)
	{
		delete this;
	}

	return ulRefCount;
}

HRESULT ManagedCallback::Exception(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, BOOL unhandled)
{
	DEBUG_OUT("[!] Exception");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::Exception(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFrame *pFrame, ULONG32 nOffset, CorDebugExceptionCallbackType dwEventType, DWORD dwFlags)
{
	DEBUG_OUT("[!] Exception");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::ExceptionUnwind(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, CorDebugExceptionUnwindCallbackType dwEventType, DWORD dwFlags)
{
	DEBUG_OUT("[!] ExceptionUnwind");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::Breakpoint(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugBreakpoint *pBreakpoint)
{
	DEBUG_OUT("[!] Breakpoint");

	pAppDomain->Continue(false);
	return S_OK;
}

void ManagedCallback::SubscribeStepComplete(std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugStepper *pStepper, CorDebugStepReason reason)> callback)
{
	this->stepCompleteCallback.push(callback);
}

HRESULT ManagedCallback::StepComplete(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugStepper *pStepper, CorDebugStepReason reason)
{
	DEBUG_OUT("[!] StepComplete");

	bool stepAgain = true;

	DEBUG_OUT("[!] EvalComplete");

	if (!stepCompleteCallback.empty())
	{
		auto callback = stepCompleteCallback.front();
		if (callback(pAppDomain, pThread, pStepper, reason))
		{
			stepCompleteCallback.pop();
		}
	}

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::Break(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread)
{
	DEBUG_OUT("[!] Break");

	pAppDomain->Continue(false);
	return S_OK;
}

void ManagedCallback::SubscribeEvalComplete(std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval)> callback)
{
	this->evalCompleteCallback.push(callback);
}

HRESULT ManagedCallback::EvalComplete(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval)
{
	DEBUG_OUT("[!] EvalComplete");

	if (!evalCompleteCallback.empty())
	{
		auto callback = evalCompleteCallback.front();
		if (callback(pAppDomain, pThread, pEval))
		{
			evalCompleteCallback.pop();
		}
	}

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::EvalException(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval)
{
	DEBUG_OUT("[!] EvalException");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::CreateProcess(ICorDebugProcess *pProcess)
{
	DEBUG_OUT("[!] CreateProcess");

	pProcess->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::ExitProcess(ICorDebugProcess *pProcess)
{
	DEBUG_OUT("[!] ExitProcess");

	pProcess->Continue(false);
	return S_OK;
}

void ManagedCallback::SubscribeCreateThread(std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread)> callback)
{
	this->createThreadCallback.push(callback);
}

HRESULT ManagedCallback::CreateThread(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread)
{

	DEBUG_OUT("[!] CreateThread");

	if (!createThreadCallback.empty())
	{
		auto callback = createThreadCallback.front();
		if (callback(pAppDomain, thread))
		{
			createThreadCallback.pop();
		}
	}

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::ExitThread(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread)
{
	DEBUG_OUT("[!] ExitThread");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::LoadModule(ICorDebugAppDomain *pAppDomain, ICorDebugModule *pModule)
{
	DEBUG_OUT("[!] LoadModule");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::UnloadModule(ICorDebugAppDomain *pAppDomain, ICorDebugModule *pModule)
{
	DEBUG_OUT("[!] UnloadModule");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::LoadClass(ICorDebugAppDomain *pAppDomain, ICorDebugClass *c)
{
	DEBUG_OUT("[!] LoadClass");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::UnloadClass(ICorDebugAppDomain *pAppDomain, ICorDebugClass *c)
{
	DEBUG_OUT("[!] UnloadClass");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::DebuggerError(ICorDebugProcess *pProcess, HRESULT errorHR, DWORD errorCode)
{
	DEBUG_OUT("[!] DebuggerError");

	pProcess->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::LogMessage(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, LONG lLevel, WCHAR *pLogSwitchName, WCHAR *pMessage)
{
	DEBUG_OUT("[!] LogMessage");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::LogSwitch(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, LONG lLevel, ULONG ulReason, WCHAR *pLogSwitchName, WCHAR *pParentName)
{
	DEBUG_OUT("[!] LogSwitch");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::CreateAppDomain(ICorDebugProcess *pProcess, ICorDebugAppDomain *pAppDomain)
{
	DEBUG_OUT("[!] CreateAppDomain");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::ExitAppDomain(ICorDebugProcess *pProcess, ICorDebugAppDomain *pAppDomain)
{
	DEBUG_OUT("[!] ExitAppDomain");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::LoadAssembly(ICorDebugAppDomain *pAppDomain, ICorDebugAssembly *pAssembly)
{
	DEBUG_OUT("[!] LoadAssembly");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::UnloadAssembly(ICorDebugAppDomain *pAppDomain, ICorDebugAssembly *pAssembly)
{
	DEBUG_OUT("[!] UnloadAssembly");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::ControlCTrap(ICorDebugProcess *pProcess)
{
	DEBUG_OUT("[!] ControlCTrap");

	pProcess->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::NameChange(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread)
{
	DEBUG_OUT("[!] NameChange");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::UpdateModuleSymbols(ICorDebugAppDomain *pAppDomain, ICorDebugModule *pModule, IStream *pSymbolStream)
{
	DEBUG_OUT("[!] UpdateModuleSymbols");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::EditAndContinueRemap(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFunction *pFunction, BOOL fAccurate)
{
	DEBUG_OUT("[!] EditAndContinueRemap");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::BreakpointSetError(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugBreakpoint *pBreakpoint, DWORD dwError)
{
	DEBUG_OUT("[!] BreakpointSetError");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::FunctionRemapOpportunity(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFunction *pOldFunction, ICorDebugFunction *pNewFunction, ULONG32 oldILOffset)
{
	DEBUG_OUT("[!] FunctionRemapOpportunity");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::CreateConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId, WCHAR *pConnName)
{
	DEBUG_OUT("[!] CreateConnection");

	pProcess->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::ChangeConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId)
{
	DEBUG_OUT("[!] ChangeConnection");

	pProcess->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::DestroyConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId)
{
	DEBUG_OUT("[!] DestroyConnection");

	pProcess->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::FunctionRemapComplete(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFunction *pFunction)
{
	DEBUG_OUT("[!] FunctionRemapComplete");

	pAppDomain->Continue(false);
	return S_OK;
}

HRESULT ManagedCallback::MDANotification(ICorDebugController *pController, ICorDebugThread *pThread, ICorDebugMDA *pMDA)
{
	DEBUG_OUT("[!] MDANotification");

	pController->Continue(false);
	return S_OK;
}