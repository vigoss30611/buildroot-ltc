#if !defined(IMG_DAtiny_c_h_)
#define IMG_DAtiny_c_h_

// DAtinyscript include header version 3.2.1.10

/* exporting methods */
#if (__GNUC__ >= 4) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)
#  ifndef IMG_GCC_HASCLASSVISIBILITY
#    define IMG_GCC_HASCLASSVISIBILITY
#  endif
#endif

#if defined(IMG_DLL_TYPEDEF)
#  define IMG_EXPORT typedef
#elif !defined(DLLEXPORT)
# if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
#   if defined(STATIC_LINKED)
#     define IMG_EXPORT
#   else
#     define IMG_EXPORT __declspec(dllexport)
#   endif
# else
#   if defined(__GNUC__) && defined(IMG_GCC_HASCLASSVISIBILITY)
#     define IMG_EXPORT __attribute__ ((visibility("default")))
#   else
#     define IMG_EXPORT
#   endif
# endif
#endif


#if defined(__cplusplus)
extern "C"
{
#endif

struct DAtiny_Connection;
typedef struct DAtiny_Connection* DAtiny_connection;

// Initialise the DAtinyscript library. This function always returns a connection object.
// Test for success by calling DAtiny_GetLastError(conn).

IMG_EXPORT DAtiny_connection (DAtiny_Create)();

// get the last error, the pointer returned is only valid until the next call
// to DAtiny_ on this connection. The user should not deallocate this string.
// DAtiny_GetLastError returns 0 if no error occurred in the last call.
IMG_EXPORT const char * (DAtiny_GetLastError)(DAtiny_connection Connection);

// Clean up the DAtinyscript library.
IMG_EXPORT void (DAtiny_Delete)(DAtiny_connection Connection);

IMG_EXPORT void (DAtiny_UseTarget)(DAtiny_connection Connection, const char* Serial);
IMG_EXPORT void (DAtiny_Reset)(DAtiny_connection Connection, int Halt);
IMG_EXPORT unsigned int (DAtiny_ReadMemory)(DAtiny_connection Connection, unsigned int Address, int ElementSize, int MemoryType);
IMG_EXPORT void (DAtiny_WriteMemory)(DAtiny_connection Connection, unsigned int Address, unsigned int Value, int ElementSize, int MemoryType);

// The following functions were added in version 3.1 of DAtinyscript
IMG_EXPORT void (DAtiny_ForceDisconnectOwner)(DAtiny_connection Connection, const char* Serial);

// The user should not deallocate these strings, they are valid until the next
// call to a DAtiny function on the same connection
IMG_EXPORT const char * (DAtiny_GetSerialNumber)(DAtiny_connection Connection);
IMG_EXPORT const char * (DAtiny_GetVersion)(DAtiny_connection Connection);
IMG_EXPORT const char * (DAtiny_GetName)(DAtiny_connection Connection);

IMG_EXPORT void (DAtiny_ReadMemoryBlock)(DAtiny_connection Connection, unsigned int Address, unsigned int ElementCount, int ElementSize, int MemoryType, void* buf);
IMG_EXPORT void (DAtiny_WriteMemoryBlock)(DAtiny_connection Connection, unsigned int Address, unsigned int ElementCount, int ElementSize, int MemoryType, const void* buf);
IMG_EXPORT int  (DAtiny_GetTargetCount)(DAtiny_connection Connection);
IMG_EXPORT int  (DAtiny_GetTarget)(DAtiny_connection Connection);
IMG_EXPORT void (DAtiny_SetTarget)(DAtiny_connection Connection, int Target);
IMG_EXPORT int  (DAtiny_GetThreadCount)(DAtiny_connection Connection);
IMG_EXPORT int  (DAtiny_GetThread)(DAtiny_connection Connection);
IMG_EXPORT void (DAtiny_SetThread)(DAtiny_connection Connection, int Thread);
IMG_EXPORT int  (DAtiny_GetTargetFamily)(DAtiny_connection Connection);

enum DAtiny_target_family
{
	DAtiny_Meta=3,
	DAtiny_UCC=4,
};

IMG_EXPORT double (DAtiny_SetJTAGClock)(DAtiny_connection Connection, double Frequency);
IMG_EXPORT double (DAtiny_GetJTAGClock)(DAtiny_connection Connection);

IMG_EXPORT void (DAtiny_DAReset)(DAtiny_connection Connection);
IMG_EXPORT void (DAtiny_TapReset)(DAtiny_connection Connection);
IMG_EXPORT void (DAtiny_DRScan)(DAtiny_connection Connection, void * Buf, unsigned int ScanLength);
IMG_EXPORT void (DAtiny_IRScan)(DAtiny_connection Connection, void * Buf, unsigned int ScanLength);

// An 8k buffer is sufficient here
IMG_EXPORT void (DAtiny_ReadDAConfigurationData)(DAtiny_connection Connection, void * Buf, unsigned int Size);
IMG_EXPORT void (DAtiny_WriteDAConfigurationData)(DAtiny_connection Connection, const void * Buf, unsigned int Size);

// The user should not deallocate these strings, they are valid until the next
// call to a DAtiny function on the same connection
// This is a new line separated list of filenames that can be passed to GetDiagnosticFile
IMG_EXPORT const char* (DAtiny_GetDiagnosticFileList)(DAtiny_connection Connection);
IMG_EXPORT const char* (DAtiny_GetDiagnosticFile)(DAtiny_connection Connection, const char* Filename);

// The user should not deallocate this string, they are valid until the next
// call to a DAtiny function on the same connection
// This is a new line separated list of settings that can be passed to ReadDASetting and WriteDASetting
IMG_EXPORT const char *	(DAtiny_GetDASettingList)(DAtiny_connection Connection);
IMG_EXPORT unsigned int (DAtiny_ReadDASetting)(DAtiny_connection Connection, const char * Name);
IMG_EXPORT void (DAtiny_WriteDASetting)(DAtiny_connection Connection, const char * Name, unsigned int Value);

// Added in DAtinyscript 3.2
IMG_EXPORT void (DAtiny_ReadRegistersByName)(DAtiny_connection Connection, int count, const char * const* regnames, void * buf);
IMG_EXPORT void (DAtiny_WriteRegistersByName)(DAtiny_connection Connection, int count, const char * const* regnames, const void * buf);
IMG_EXPORT void (DAtiny_ReadRegistersByIndex)(DAtiny_connection Connection, int count, const unsigned int * indexes, void * buf);
IMG_EXPORT void (DAtiny_WriteRegistersByIndex)(DAtiny_connection Connection, int count, const unsigned int * indexes, const void * buf);
IMG_EXPORT unsigned int (DAtiny_GetJTAGId)(DAtiny_connection Connection);
IMG_EXPORT unsigned int (DAtiny_ReadModWrite)(DAtiny_connection Connection, unsigned int Address, unsigned int NewValue, unsigned int ModifyMask, const char* Operation, int ElementCount, int MemoryType);
IMG_EXPORT unsigned int (DAtiny_ReadModWriteAtomic)(DAtiny_connection Connection, unsigned int Address, unsigned int NewValue, unsigned int ModifyMask, const char* Operation, int ElementCount, int MemoryType);


#if defined(__cplusplus)
}
#endif

#endif

