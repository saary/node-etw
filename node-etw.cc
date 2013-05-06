#include <node.h>
#include <v8.h>
#include <evntprov.h>

using namespace v8;

void LogEvents_LogDebugEvent(LPCSTR message);
void LogEvents_LogVerboseEvent(LPCSTR message);
void LogEvents_LogInfoEvent(LPCSTR message);
void LogEvents_LogWarningEvent(LPCSTR message);
void LogEvents_LogErrorEvent(LPCSTR message);

Handle<Value> LogDebug(const Arguments& args) 
{
  HandleScope scope;
  const char *str = (args.Length() > 0) ? *v8::String::Utf8Value(args[0]->ToString()) : "";
  LogEvents_LogDebugEvent(str);
  return scope.Close(Undefined());
}

Handle<Value> LogVerbose(const Arguments& args) 
{
  HandleScope scope;
  const char *str = (args.Length() > 0) ? *v8::String::Utf8Value(args[0]->ToString()) : "";
  LogEvents_LogVerboseEvent(str);
  return scope.Close(Undefined());
}

Handle<Value> LogInfo(const Arguments& args) 
{
  HandleScope scope;
  const char *str = (args.Length() > 0) ? *v8::String::Utf8Value(args[0]->ToString()) : "";
  LogEvents_LogInfoEvent(str);
  return scope.Close(Undefined());
}

Handle<Value> LogWarning(const Arguments& args) 
{
  HandleScope scope;
  const char *str = (args.Length() > 0) ? *v8::String::Utf8Value(args[0]->ToString()) : "";
  LogEvents_LogWarningEvent(str);
  return scope.Close(Undefined());
}

Handle<Value> LogError(const Arguments& args) 
{
  HandleScope scope;
  const char *str = (args.Length() > 0) ? *v8::String::Utf8Value(args[0]->ToString()) : "";
  LogEvents_LogErrorEvent(str);
  return scope.Close(Undefined());
}

/***
 * ETW HELPERS
***/
#define LOG_EVENT_ID 1
#define LOG_EVENT_VERSION 0
#define LOG_EVENT_CHANNEL 0
#define LOG_EVENT_TASK 0
#define LOG_EVENT_KEYWORD 1

#define ETW_PROVIDER_GUID(l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
const GUID ProviderGuid = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

ETW_PROVIDER_GUID(0x10535c9f, 0xbf2e, 0x4a80, 0x84, 0x10, 0xeb, 0xbe, 0xba, 0x2c, 0x74, 0x00);

extern "C"
{
	ULONG
	EVNTAPI
	EventRegister(
		_In_ LPCGUID ProviderId,
		_In_opt_ PENABLECALLBACK EnableCallback,
		_In_opt_ PVOID CallbackContext,
		_Out_ PREGHANDLE RegHandle
		);

	ULONG
	EVNTAPI
	EventWrite(
		_In_ REGHANDLE RegHandle,
		_In_ PCEVENT_DESCRIPTOR EventDescriptor,
		_In_ ULONG UserDataCount,
		_In_reads_opt_(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
		);
}

REGHANDLE RegistrationHandle = 0;

void EtwInitProvider()
{
	if (0 != RegistrationHandle)
		return;

	EventRegister(
			&ProviderGuid,      // GUID that identifies the provider
			NULL,               // Callback not used
			NULL,               // Context noot used
			&RegistrationHandle // Used when calling EventWrite and EventUnregister
			);
}

FORCEINLINE
VOID
etw_CreateDescriptor_LPCSTR(
    EVENT_DATA_DESCRIPTOR* pEventDataDescriptor,
    LPCSTR* value)
{
    pEventDataDescriptor->Ptr = (ULONGLONG)(*value);
    pEventDataDescriptor->Size =(ULONG) (*value ? strlen((*value)) + 1 : 0);
    pEventDataDescriptor->Reserved = 0;
    return;
}

static void Etw_Trace1_LPCSTR (EVENT_DESCRIPTOR* eventDescriptor, LPCSTR a0) 
{                                                              
    DWORD status = ERROR_SUCCESS;                              
    EVENT_DATA_DESCRIPTOR Descriptors[1];                      
                                                               
    EtwInitProvider();                                         
    etw_CreateDescriptor_LPCSTR (&Descriptors[0], &a0);     
                                                               
    status = EventWrite(                                       
        RegistrationHandle,                                    
        eventDescriptor,                                       
        1,                                                     
        Descriptors                                            
        );                                                     
}

void Log(USHORT id, UCHAR level, LPCSTR message)				     
{
	EVENT_DESCRIPTOR descriptor = { 
	  id,						/* USHORT    Id;       */  
	  LOG_EVENT_VERSION,	    /* UCHAR     Version;  */  
	  LOG_EVENT_CHANNEL,	    /* UCHAR     Channel;  */  
	  level,					/* UCHAR     Level;    */  
	  0,						/* UCHAR     Opcode;   */  
	  LOG_EVENT_TASK,	        /* USHORT    Task;     */  
	  LOG_EVENT_KEYWORD,	    /* ULONGLONG Keyword;  */  
	};

    Etw_Trace1_LPCSTR(&descriptor, message);                     
};

void LogEvents_LogDebugEvent(LPCSTR message)
{
	Log(5, 5, message);
}

void LogEvents_LogVerboseEvent(LPCSTR message)
{
	Log(4, 4, message);
}

void LogEvents_LogInfoEvent(LPCSTR message)
{
	Log(3, 3, message);
}

void LogEvents_LogWarningEvent(LPCSTR message)
{
	Log(2, 2, message);
}

void LogEvents_LogErrorEvent(LPCSTR message)
{
	Log(1, 1, message);
}

void init(Handle<Object> exports) 
{
  exports->Set(String::NewSymbol("debug"),
      FunctionTemplate::New(LogDebug)->GetFunction());

  exports->Set(String::NewSymbol("log"),
      FunctionTemplate::New(LogVerbose)->GetFunction());

  exports->Set(String::NewSymbol("info"),
      FunctionTemplate::New(LogInfo)->GetFunction());

  exports->Set(String::NewSymbol("warn"),
      FunctionTemplate::New(LogWarning)->GetFunction());

  exports->Set(String::NewSymbol("warning"),
      FunctionTemplate::New(LogWarning)->GetFunction());

  exports->Set(String::NewSymbol("error"),
      FunctionTemplate::New(LogError)->GetFunction());
}

NODE_MODULE(etw, init)