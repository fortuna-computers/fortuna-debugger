/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_TO_DEBUGGER_PB_H_INCLUDED
#define PB_TO_DEBUGGER_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _AckResponse {
    uint32_t id;
} AckResponse;

typedef struct _WriteMemoryConfirmation {
    bool error;
    uint64_t first_failed_pos;
} WriteMemoryConfirmation;

typedef struct _MemoryUpdate {
    uint64_t initial_pos;
    pb_callback_t bytes;
} MemoryUpdate;

typedef struct _ToDebugger {
    pb_size_t which_message;
    union {
        AckResponse ack_response;
        WriteMemoryConfirmation write_memory_confirmation;
        MemoryUpdate memory_update;
    } message;
} ToDebugger;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define AckResponse_init_default                 {0}
#define WriteMemoryConfirmation_init_default     {0, 0}
#define MemoryUpdate_init_default                {0, {{NULL}, NULL}}
#define ToDebugger_init_default                  {0, {AckResponse_init_default}}
#define AckResponse_init_zero                    {0}
#define WriteMemoryConfirmation_init_zero        {0, 0}
#define MemoryUpdate_init_zero                   {0, {{NULL}, NULL}}
#define ToDebugger_init_zero                     {0, {AckResponse_init_zero}}

/* Field tags (for use in manual encoding/decoding) */
#define AckResponse_id_tag                       1
#define WriteMemoryConfirmation_error_tag        1
#define WriteMemoryConfirmation_first_failed_pos_tag 2
#define MemoryUpdate_initial_pos_tag             1
#define MemoryUpdate_bytes_tag                   2
#define ToDebugger_ack_response_tag              1
#define ToDebugger_write_memory_confirmation_tag 2
#define ToDebugger_memory_update_tag             3

/* Struct field encoding specification for nanopb */
#define AckResponse_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT32,   id,                1)
#define AckResponse_CALLBACK NULL
#define AckResponse_DEFAULT NULL

#define WriteMemoryConfirmation_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, BOOL,     error,             1) \
X(a, STATIC,   SINGULAR, UINT64,   first_failed_pos,   2)
#define WriteMemoryConfirmation_CALLBACK NULL
#define WriteMemoryConfirmation_DEFAULT NULL

#define MemoryUpdate_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, UINT64,   initial_pos,       1) \
X(a, CALLBACK, SINGULAR, BYTES,    bytes,             2)
#define MemoryUpdate_CALLBACK pb_default_field_callback
#define MemoryUpdate_DEFAULT NULL

#define ToDebugger_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,ack_response,message.ack_response),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,write_memory_confirmation,message.write_memory_confirmation),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (message,memory_update,message.memory_update),   3)
#define ToDebugger_CALLBACK NULL
#define ToDebugger_DEFAULT NULL
#define ToDebugger_message_ack_response_MSGTYPE AckResponse
#define ToDebugger_message_write_memory_confirmation_MSGTYPE WriteMemoryConfirmation
#define ToDebugger_message_memory_update_MSGTYPE MemoryUpdate

extern const pb_msgdesc_t AckResponse_msg;
extern const pb_msgdesc_t WriteMemoryConfirmation_msg;
extern const pb_msgdesc_t MemoryUpdate_msg;
extern const pb_msgdesc_t ToDebugger_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define AckResponse_fields &AckResponse_msg
#define WriteMemoryConfirmation_fields &WriteMemoryConfirmation_msg
#define MemoryUpdate_fields &MemoryUpdate_msg
#define ToDebugger_fields &ToDebugger_msg

/* Maximum encoded size of messages (where known) */
/* MemoryUpdate_size depends on runtime parameters */
/* ToDebugger_size depends on runtime parameters */
#define AckResponse_size                         6
#define TO_DEBUGGER_PB_H_MAX_SIZE                WriteMemoryConfirmation_size
#define WriteMemoryConfirmation_size             13

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif