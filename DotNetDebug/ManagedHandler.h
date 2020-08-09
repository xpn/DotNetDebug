#pragma once
#include "Common.h"

class ManagedCallback : public ICorDebugManagedCallback, public ICorDebugManagedCallback2 /*, public ICorDebugManagedCallback3*/
{
private:
	std::queue<std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugStepper *pStepper, CorDebugStepReason reason)>> stepCompleteCallback;
	std::queue<std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval)>> evalCompleteCallback;
	std::queue<std::function<bool(ICorDebugProcess *pProcess, ICorDebugAppDomain *pAppDomain)>> createAppDomainCallback;
	std::queue<std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread)>> createThreadCallback;

public:
	void SubscribeStepComplete(std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugStepper *pStepper, CorDebugStepReason reason)> callback);
	void SubscribeEvalComplete(std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval)> callback);
	void SubscribeCreateAppDomain(std::function<bool(ICorDebugProcess *pProcess, ICorDebugAppDomain *pAppDomain)> callback);
	void SubscribeCreateThread(std::function<bool(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread)> callback);

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject);
	ULONG STDMETHODCALLTYPE AddRef(void);
	ULONG STDMETHODCALLTYPE Release(void);

	// ICorDebugManagedCallback
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::Exception(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, BOOL unhandled);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::Breakpoint(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugBreakpoint *pBreakpoint);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::StepComplete(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugStepper *pStepper, CorDebugStepReason reason);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::Break(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::EvalComplete(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::EvalException(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugEval *pEval);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::CreateProcess(ICorDebugProcess *pProcess);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::ExitProcess(ICorDebugProcess *pProcess);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::CreateThread(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::ExitThread(ICorDebugAppDomain *pAppDomain, ICorDebugThread *thread);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::LoadModule(ICorDebugAppDomain *pAppDomain, ICorDebugModule *pModule);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::UnloadModule(ICorDebugAppDomain *pAppDomain, ICorDebugModule *pModule);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::LoadClass(ICorDebugAppDomain *pAppDomain, ICorDebugClass *c);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::UnloadClass(ICorDebugAppDomain *pAppDomain, ICorDebugClass *c);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::DebuggerError(ICorDebugProcess *pProcess, HRESULT errorHR, DWORD errorCode);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::LogMessage(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, LONG lLevel, WCHAR *pLogSwitchName, WCHAR *pMessage);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::LogSwitch(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, LONG lLevel, ULONG ulReason, WCHAR *pLogSwitchName, WCHAR *pParentName);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::CreateAppDomain(ICorDebugProcess *pProcess, ICorDebugAppDomain *pAppDomain);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::ExitAppDomain(ICorDebugProcess *pProcess, ICorDebugAppDomain *pAppDomain);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::LoadAssembly(ICorDebugAppDomain *pAppDomain, ICorDebugAssembly *pAssembly);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::UnloadAssembly(ICorDebugAppDomain *pAppDomain, ICorDebugAssembly *pAssembly);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::ControlCTrap(ICorDebugProcess *pProcess);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::NameChange(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::UpdateModuleSymbols(ICorDebugAppDomain *pAppDomain, ICorDebugModule *pModule, IStream *pSymbolStream);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::EditAndContinueRemap(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFunction *pFunction, BOOL fAccurate);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback::BreakpointSetError(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugBreakpoint *pBreakpoint, DWORD dwError);

	//ICorDebugManagedCallback2
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::FunctionRemapOpportunity(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread,	ICorDebugFunction *pOldFunction, ICorDebugFunction *pNewFunction, ULONG32 oldILOffset);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::CreateConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId, WCHAR *pConnName);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::ChangeConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::DestroyConnection(ICorDebugProcess *pProcess, CONNID dwConnectionId);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::Exception(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFrame *pFrame, ULONG32 nOffset, CorDebugExceptionCallbackType dwEventType, DWORD dwFlags);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::ExceptionUnwind(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, CorDebugExceptionUnwindCallbackType dwEventType, DWORD dwFlags);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::FunctionRemapComplete(ICorDebugAppDomain *pAppDomain, ICorDebugThread *pThread, ICorDebugFunction *pFunction);
	HRESULT STDMETHODCALLTYPE ICorDebugManagedCallback2::MDANotification(ICorDebugController *pController, ICorDebugThread *pThread, ICorDebugMDA *pMDA);
};
