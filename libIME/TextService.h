#ifndef IME_TEXT_SERVICE_H
#define IME_TEXT_SERVICE_H

#include "libIME.h"
#include <msctf.h>
#include "EditSession.h"

namespace Ime {

class TextService:
	// TSF interfaces
	public ITfTextInputProcessor,
	public ITfDisplayAttributeProvider,
	// event sinks
	public ITfThreadMgrEventSink,
	public ITfTextEditSink,
	public ITfKeyEventSink,
	public ITfCompositionSink {
public:

	TextService(void);
	virtual ~TextService(void);

	// public methods

	// virtual functions that IME implementors may need to override
	virtual void onActivate();
	virtual void onDeactivate();

	virtual void onFocus();

	virtual bool onKeyDown(long key);
	virtual bool onKeyUp(long key);

	// COM related stuff
public:
    // IUnknown
    STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

    // ITfTextInputProcessor
    STDMETHODIMP Activate(ITfThreadMgr *pThreadMgr, TfClientId tfClientId);
    STDMETHODIMP Deactivate();

    // ITfThreadMgrEventSink
    STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr);
    STDMETHODIMP OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus);
    STDMETHODIMP OnPushContext(ITfContext *pContext);
    STDMETHODIMP OnPopContext(ITfContext *pContext);

    // ITfTextEditSink
    STDMETHODIMP OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

    // ITfKeyEventSink
    STDMETHODIMP OnSetFocus(BOOL fForeground);
    STDMETHODIMP OnTestKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyDown(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnTestKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnKeyUp(ITfContext *pContext, WPARAM wParam, LPARAM lParam, BOOL *pfEaten);
    STDMETHODIMP OnPreservedKey(ITfContext *pContext, REFGUID rguid, BOOL *pfEaten);

    // ITfCompositionSink
    STDMETHODIMP OnCompositionTerminated(TfEditCookie ecWrite, ITfComposition *pComposition);

    // ITfDisplayAttributeProvider
    STDMETHODIMP EnumDisplayAttributeInfo(IEnumTfDisplayAttributeInfo **ppEnum);
    STDMETHODIMP GetDisplayAttributeInfo(REFGUID guidInfo, ITfDisplayAttributeInfo **ppInfo);

protected:
	// edit session classes, used with TSF
	class KeyEditSession: public EditSession {
	public:
		KeyEditSession(TextService* service, ITfContext* context, WPARAM wParam, LPARAM lParam):
			EditSession(service, context),
			wParam_(wParam),
			lParam_(lParam) {
		}
		STDMETHODIMP DoEditSession(TfEditCookie ec);

		WPARAM wParam_;
		LPARAM lParam_;
	};

	class StartCompositionEditSession: public EditSession {
	public:
		StartCompositionEditSession(TextService* service, ITfContext* context):
			EditSession(service, context){
		}
		STDMETHODIMP DoEditSession(TfEditCookie ec);
	};

	class EndCompositionEditSession: public EditSession {
	public:
		EndCompositionEditSession(TextService* service, ITfContext* context):
			EditSession(service, context){
		}
		STDMETHODIMP DoEditSession(TfEditCookie ec);
	};

	HRESULT onKeyEditSession(TfEditCookie cookie, KeyEditSession* session);
	HRESULT onStartCompositionEditSession(TfEditCookie cookie, StartCompositionEditSession* session);
	HRESULT onEndCompositionEditSession(TfEditCookie cookie, EndCompositionEditSession* session);

private:
	ITfThreadMgr *threadMgr_;
	TfClientId clientId_;

	// event sink cookies
	DWORD threadMgrEventSinkCookie_;
	DWORD textEditSinkCookie_;
	DWORD compositionSinkCookie_;

	long refCount_;
};

}

#endif
