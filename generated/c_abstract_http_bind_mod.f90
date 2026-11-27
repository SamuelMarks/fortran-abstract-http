!> @file c_abstract_http_bind_mod.f90
!> @module c_abstract_http_bind_mod
!> @brief Auto-generated Fortran 2003+ (ISO_C_BINDING) bindings for c-abstract-http.
MODULE c_abstract_http_bind_mod
  USE, INTRINSIC :: ISO_C_BINDING
  IMPLICIT NONE

  ENUM, BIND(C)
    ENUMERATOR :: abstract_http_msg_http_send = 1_C_INT
    ENUMERATOR :: abstract_http_msg_http_response = 2_C_INT
    ENUMERATOR :: abstract_http_msg_shutdown = 3_C_INT
    ENUMERATOR :: abstract_http_msg_custom = 1000_C_INT
  END ENUM

  !> @brief Opaque Actor type.
  !
  TYPE, BIND(C) :: abstracthttpactor
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpactor

  TYPE, BIND(C) :: abstracthttpmessage
    INTEGER(C_INT32_T) :: type_
    !< The message type from AbstractHttpMessageType or custom
    TYPE(C_PTR) :: payload
    !< Owned by the sender until freed by the receiver
    TYPE(C_PTR) :: sender
    !< The actor that sent this message
    TYPE(C_PTR) :: receiver
  END TYPE abstracthttpmessage

  ENUM, BIND(C)
    ENUMERATOR :: c_abstract_http_success = 0_C_INT
    ENUMERATOR :: c_abstract_http_err_inval = -1_C_INT
    ENUMERATOR :: c_abstract_http_err_nomem = -2_C_INT
    ENUMERATOR :: c_abstract_http_err_io = -3_C_INT
    ENUMERATOR :: c_abstract_http_err_notsup = -4_C_INT
    ENUMERATOR :: c_abstract_http_err_timeout = -5_C_INT
    ENUMERATOR :: c_abstract_http_err_syscall = -6_C_INT
    ENUMERATOR :: c_abstract_http_err_ws_framing = -1001_C_INT
    ENUMERATOR :: c_abstract_http_err_ws_handshake = -1002_C_INT
    ENUMERATOR :: c_abstract_http_err_sse_parse = -1003_C_INT
    ENUMERATOR :: c_abstract_http_err_sse_oom = -1004_C_INT
    ENUMERATOR :: c_abstract_http_err_ws_closed = -1005_C_INT
    ENUMERATOR :: c_abstract_http_err_acces = 13_C_INT
    ENUMERATOR :: c_abstract_http_err_exist = 17_C_INT
    ENUMERATOR :: c_abstract_http_err_loop = 40_C_INT
    ENUMERATOR :: c_abstract_http_err_msgsize = 90_C_INT
    ENUMERATOR :: c_abstract_http_err_connrefused = 111_C_INT
    ENUMERATOR :: c_abstract_http_err_hostunreach = 113_C_INT
    ENUMERATOR :: c_abstract_http_err_canceled = 22_C_INT
  END ENUM

  ! Typedef alias: c_abstract_http_error_t
  !> @brief Opaque Message Bus type.
  !
  TYPE, BIND(C) :: abstracthttpmessagebus
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpmessagebus

  ABSTRACT INTERFACE
    FUNCTION abstract_http_actor_handler_cb_cb(actor, msg) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: actor
      TYPE(C_PTR), VALUE :: msg
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstract_http_actor_handler_cb_cb
  END INTERFACE

  TYPE, BIND(C) :: abstracthttpactorhooks
    TYPE(C_FUNPTR) :: bus_init
    !< Hook for bus initialization
    TYPE(C_FUNPTR) :: bus_free
    !< Hook for bus destruction
    TYPE(C_FUNPTR) :: bus_process
    !< Hook to process messages
    TYPE(C_FUNPTR) :: actor_spawn
    !< Hook for spawning actors
    TYPE(C_FUNPTR) :: actor_send
    !< Hook for sending messages
    TYPE(C_FUNPTR) :: actor_get_state
    !< Hook for retrieving state
    TYPE(C_FUNPTR) :: actor_get_name
  END TYPE abstracthttpactorhooks

  !> @brief Opaque structure representing a Thread Local Storage key.
  !
  TYPE, BIND(C) :: abstracthttptlskey
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttptlskey

  ENUM, BIND(C)
    ENUMERATOR :: cmp_modality_sync_single = 0_C_INT
    ENUMERATOR :: cmp_modality_sync_multi = 1_C_INT
    ENUMERATOR :: cmp_modality_async_single = 2_C_INT
    ENUMERATOR :: cmp_modality_async_multi = 3_C_INT
    ENUMERATOR :: cmp_modality_greenthreads = 4_C_INT
    ENUMERATOR :: cmp_modality_multiprocess_actor = 5_C_INT
  END ENUM

  TYPE, BIND(C) :: cmpappconfig
    INTEGER(C_INT32_T) :: modality
    !< cast from cmp_modality_t
    INTEGER(C_SIZE_T) :: min_threads
    !< minimum threads for pools
    INTEGER(C_SIZE_T) :: max_threads
  END TYPE cmpappconfig

  TYPE, BIND(C) :: cmpprogressbinding
    TYPE(C_PTR) :: ui_component
    !< Pointer to UI component state
    TYPE(C_FUNPTR) :: update_progress
    !< Callback for updating progress
    INTEGER(C_INT32_T) :: cancel_requested
    !< 1 if the user closed the UI view and wants to abort
    !
    TYPE(C_PTR) :: framework_cb
    !< Generic pointer referencing framework hook
    TYPE(C_PTR) :: context
  END TYPE cmpprogressbinding

  ABSTRACT INTERFACE
    FUNCTION cmpframeworkuihookfn_cb(context, ui_component, percentage) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FLOAT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: context
      TYPE(C_PTR), VALUE :: ui_component
      REAL(C_FLOAT), VALUE :: percentage
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION cmpframeworkuihookfn_cb
  END INTERFACE

  ENUM, BIND(C)
    ENUMERATOR :: modality_sync = 0_C_INT
    ENUMERATOR :: modality_async = 1_C_INT
    ENUMERATOR :: modality_thread_pool = 2_C_INT
    ENUMERATOR :: modality_multiprocess = 3_C_INT
    ENUMERATOR :: modality_greenthread = 4_C_INT
    ENUMERATOR :: modality_message_passing = 5_C_INT
  END ENUM

  ENUM, BIND(C)
    ENUMERATOR :: http_retry_none = 0_C_INT
    ENUMERATOR :: http_retry_linear = 1_C_INT
    ENUMERATOR :: http_retry_exponential = 2_C_INT
  END ENUM

  TYPE, BIND(C) :: httpcookie
    TYPE(C_PTR) :: name
    !< Cookie name (allocated)
    TYPE(C_PTR) :: value_
    !< Cookie value (allocated)
    TYPE(C_PTR) :: domain
    !< Domain attribute (allocated, optional)
    TYPE(C_PTR) :: path
    !< Path attribute (allocated, optional)
    TYPE(C_PTR) :: same_site
    !< SameSite attribute (allocated, optional)
    TYPE(C_PTR) :: expires
    !< Expiration timestamp (unix epoch), 0 if session cookie
    INTEGER(C_INT32_T) :: secure
    !< Secure flag (1 = true, 0 = false)
    INTEGER(C_INT32_T) :: http_only
  END TYPE httpcookie

  TYPE, BIND(C) :: httpcookiejar
    TYPE(C_PTR) :: cookies
    !< Dynamic array of cookies
    INTEGER(C_SIZE_T) :: count
    !< Number of cookies stored
    INTEGER(C_SIZE_T) :: capacity
  END TYPE httpcookiejar

  TYPE, BIND(C) :: httpconfig
    TYPE(C_PTR) :: timeout_ms
    !< General timeout in milliseconds
    TYPE(C_PTR) :: connect_timeout_ms
    !< Connect timeout in milliseconds (0 to use
    ! timeout_ms)
    TYPE(C_PTR) :: read_timeout_ms
    !< Receive/Read timeout in milliseconds (0 to use
    ! timeout_ms)
    TYPE(C_PTR) :: write_timeout_ms
    !< Send/Write timeout in milliseconds (0 to use
    ! timeout_ms)
    INTEGER(C_INT32_T) :: verify_peer
    !< 1 to verify SSL peer, 0 to ignore
    INTEGER(C_INT32_T) :: verify_host
    !< 1 to verify SSL host, 0 to ignore
    INTEGER(C_INT32_T) :: follow_redirects
    !< 1 to automatically follow 3xx redirects, 0 to
    ! disable
    TYPE(C_PTR) :: user_agent
    !< Custom User-Agent string
    TYPE(C_PTR) :: proxy_url
    !< Proxy URL (e.g. "http://10.0.0.1:8080")
    TYPE(C_PTR) :: proxy_username
    !< Proxy basic auth username (optional)
    TYPE(C_PTR) :: proxy_password
    !< Proxy basic auth password (optional)
    INTEGER(C_INT32_T) :: retry_count
    !< Max retries on failure (default 0)
    INTEGER(C_INT) :: retry_policy
    !< Backoff strategy
    TYPE(C_PTR) :: cookie_jar
    !< Optional shared cookie jar for
    ! persistence across requests
    INTEGER(C_INT) :: modality
    !< Execution paradigm
    INTEGER(C_SIZE_T) :: min_threads
    !< Min threads for thread pool modality
    INTEGER(C_SIZE_T) :: max_threads
    !< Max threads for thread pool modality
    INTEGER(C_INT32_T) :: version_mask
    !< Preferred HTTP version(s) (bitmask of enum
    ! HttpVersion)
    INTEGER(C_INT32_T) :: tls_version_mask
    !< Allowed TLS version(s) (bitmask of enum
    ! HttpTlsVersion)
    INTEGER(C_INT32_T) :: http3_fallback
    !< 1 to fallback to HTTP/1.1/2 if HTTP/3 fails, 0
    ! otherwise
    TYPE(C_PTR) :: ca_path
    !< Optional PEM/DER CA bundle file path
    TYPE(C_PTR) :: ca_buffer
    !< Optional in-memory PEM/DER CA certificate
    INTEGER(C_SIZE_T) :: ca_buffer_len
    !< Byte length of ca_buffer
    TYPE(C_PTR) :: client_cert_path
    !< Optional mTLS client certificate path
    TYPE(C_PTR) :: client_key_path
    !< Optional mTLS client private key path
    TYPE(C_PTR) :: cert_pin_sha256
    !< Optional SHA-256 SPKI pin (sha256//b64 or hex)
    INTEGER(C_INT32_T) :: keep_alive_enabled
    !< 1 to enable HTTP/1.1 keep-alive connection
    ! pooling, 0 to close per request
    TYPE(C_PTR) :: keep_alive_idle_ms
    !< Max idle lifetime in ms for pooled connections
    ! (0 for backend default)
    INTEGER(C_SIZE_T) :: max_idle_conns
  END TYPE httpconfig

  ENUM, BIND(C)
    ENUMERATOR :: http_get = 0_C_INT
    ENUMERATOR :: http_post = 1_C_INT
    ENUMERATOR :: http_put = 2_C_INT
    ENUMERATOR :: http_delete = 3_C_INT
    ENUMERATOR :: http_patch = 4_C_INT
    ENUMERATOR :: http_head = 5_C_INT
    ENUMERATOR :: http_options = 6_C_INT
    ENUMERATOR :: http_trace = 7_C_INT
    ENUMERATOR :: http_query = 8_C_INT
    ENUMERATOR :: http_connect = 9_C_INT
  END ENUM

  TYPE, BIND(C) :: httpheader
    TYPE(C_PTR) :: key
    !< Header name (allocated)
    TYPE(C_PTR) :: value_
  END TYPE httpheader

  TYPE, BIND(C) :: httpheaders
    TYPE(C_PTR) :: headers
    !< Dynamic array of headers
    INTEGER(C_SIZE_T) :: count
    !< Number of headers used
    INTEGER(C_SIZE_T) :: capacity
  END TYPE httpheaders

  TYPE, BIND(C) :: httppart
    TYPE(C_PTR) :: name
    !< Form field name
    TYPE(C_PTR) :: filename
    !< Filename (optional, implies file upload)
    TYPE(C_PTR) :: content_type
    !< Content-Type of the part (e.g. "application/json"),
    ! optional
    TYPE(httpheaders) :: headers
    !< Optional per-part headers
    TYPE(C_PTR) :: data_
    !< Pointer to data buffer
    INTEGER(C_SIZE_T) :: data_len
  END TYPE httppart

  TYPE, BIND(C) :: httpparts
    TYPE(C_PTR) :: parts
    !< Dynamic array of parts
    INTEGER(C_SIZE_T) :: count
    !< Number of parts used
    INTEGER(C_SIZE_T) :: capacity
  END TYPE httpparts

  ABSTRACT INTERFACE
    FUNCTION http_on_chunk_fn_cb(user_data, chunk, len) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR), VALUE :: chunk
      INTEGER(C_SIZE_T), VALUE :: len
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_on_chunk_fn_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION http_read_chunk_fn_cb(user_data, buf, buf_len, out_len) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR), VALUE :: buf
      INTEGER(C_SIZE_T), VALUE :: buf_len
      TYPE(C_PTR), VALUE :: out_len
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_read_chunk_fn_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION http_on_progress_fn_cb(current_bytes, total_bytes, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_SIZE_T, C_PTR
      IMPLICIT NONE
      INTEGER(C_SIZE_T), VALUE :: current_bytes
      INTEGER(C_SIZE_T), VALUE :: total_bytes
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_on_progress_fn_cb
  END INTERFACE

  TYPE, BIND(C) :: httprequest
    TYPE(C_PTR) :: url
    !< Full destination URL
    INTEGER(C_INT) :: method
    !< HTTP Verb
    TYPE(httpheaders) :: headers
    !< Request headers
    TYPE(C_PTR) :: body
    !< Raw body payload (mutually exclusive with parts generally,
    ! but flattened parts end up here)
    INTEGER(C_SIZE_T) :: body_len
    !< Length of raw body
    TYPE(httpparts) :: parts
    !< Multipart segments (if any)
    TYPE(C_FUNPTR) :: on_chunk
    !< Optional callback for streaming response bodies
    TYPE(C_PTR) :: on_chunk_user_data
    !< User data passed to the on_chunk callback
    TYPE(C_FUNPTR) :: read_chunk
    !< Optional callback for streaming request bodies
    TYPE(C_PTR) :: read_chunk_user_data
    !< User data for the read_chunk callback
    INTEGER(C_SIZE_T) :: expected_body_len
    !< Expected total upload size (set to 0 for
    ! unknown/chunked)
    TYPE(C_FUNPTR) :: on_progress
    !< Optional upload/download progress callback
    TYPE(C_PTR) :: on_progress_user_data
    !< User data passed to on_progress
    TYPE(C_PTR) :: cancel_token
    !< Optional cooperative cancellation flag
    ! (non-zero aborts)
    TYPE(C_PTR) :: ws_ctx
    !< Opaque WebSocket stream context
    TYPE(C_PTR) :: sse_ctx
  END TYPE httprequest

  !> @brief Opaque Coroutine state.
  !
  TYPE, BIND(C) :: abstracthttpcoroutine
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpcoroutine

  ABSTRACT INTERFACE
    FUNCTION abstract_http_coroutine_cb_cb(arg) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: arg
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_cb_cb
  END INTERFACE

  TYPE, BIND(C) :: abstracthttpcoroutinehooks
    TYPE(C_FUNPTR) :: init
    !< Hook for coroutine initialization
    TYPE(C_FUNPTR) :: free
    !< Hook for coroutine destruction
    TYPE(C_FUNPTR) :: resume
    !< Hook for resuming execution
    TYPE(C_FUNPTR) :: yield
    !< Hook for yielding execution
    TYPE(C_FUNPTR) :: is_done
  END TYPE abstracthttpcoroutinehooks

  ENUM, BIND(C)
    ENUMERATOR :: http_loop_read = 1_C_INT
    ENUMERATOR :: http_loop_write = 2_C_INT
    ENUMERATOR :: http_loop_error = 4_C_INT
  END ENUM

  ENUM, BIND(C)
    ENUMERATOR :: http_loop_backend_auto = 0_C_INT
    ENUMERATOR :: http_loop_backend_select = 1_C_INT
    ENUMERATOR :: http_loop_backend_epoll = 2_C_INT
    ENUMERATOR :: http_loop_backend_kqueue = 3_C_INT
    ENUMERATOR :: http_loop_backend_iocp = 4_C_INT
  END ENUM

  !> @brief Opaque structure representing the event loop.
  !
  TYPE, BIND(C) :: modalityeventloop
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE modalityeventloop

  ABSTRACT INTERFACE
    FUNCTION http_loop_cb_cb(loop, fd, events, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT32_T), VALUE :: fd
      INTEGER(C_INT32_T), VALUE :: events
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_loop_cb_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION http_timer_cb_cb(loop, timer_id, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT32_T), VALUE :: timer_id
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_timer_cb_cb
  END INTERFACE

  TYPE, BIND(C) :: httploophooks
    TYPE(C_PTR) :: external_context
    ! Polling Hooks
    TYPE(C_FUNPTR) :: add_fd
    !< Register file descriptor
    TYPE(C_FUNPTR) :: mod_fd
    !< Modify file descriptor events
    TYPE(C_FUNPTR) :: remove_fd
    ! Timer Hooks
    TYPE(C_FUNPTR) :: add_timer
    !< Register a timer
    TYPE(C_FUNPTR) :: cancel_timer
    ! Wakeup Hook
    TYPE(C_FUNPTR) :: wakeup
  END TYPE httploophooks

  !> @brief Opaque transport context definition.
  !
  TYPE, BIND(C) :: httptransportcontext
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE httptransportcontext

  TYPE, BIND(C) :: httpresponse
    INTEGER(C_INT32_T) :: status_code
    !< HTTP Status Code (e.g. 200, 404)
    TYPE(httpheaders) :: headers
    !< Response Headers
    TYPE(C_PTR) :: body
    !< Response Body Payload
    INTEGER(C_SIZE_T) :: body_len
  END TYPE httpresponse

  TYPE, BIND(C) :: httpmultirequest
    TYPE(C_PTR) :: requests
    !< Array of pointers to requests
    INTEGER(C_SIZE_T) :: count
    !< Number of requests
    INTEGER(C_SIZE_T) :: capacity
  END TYPE httpmultirequest

  TYPE, BIND(C) :: httpfuture
    INTEGER(C_INT32_T) :: is_ready
    !< 1 if completed, 0 otherwise
    INTEGER(C_INT32_T) :: error_code
    !< Error code if request failed
    TYPE(C_PTR) :: response
    !< The resulting response, if successful
    TYPE(C_PTR) :: internal_state
  END TYPE httpfuture

  TYPE, BIND(C) :: httprawtlshooks
    !> @brief Opaque provider context passed to `open`
    TYPE(C_PTR) :: provider_data
    !> @brief Allocate and attach a TLS session to connected `sock_fd`
    TYPE(C_FUNPTR) :: open
    !> @brief Drive TLS client handshake (`*out_want`: 0=done, 1=read, 2=write)
    !
    TYPE(C_FUNPTR) :: handshake
    !> @brief Read decrypted bytes (`*out_want`: 0=ok/eof, 1=read, 2=write)
    TYPE(C_FUNPTR) :: read
    !> @brief Write plaintext bytes (`*out_want`: 0=ok, 1=read, 2=write)
    TYPE(C_FUNPTR) :: write
    !> @brief Shut down and free a TLS session
    TYPE(C_FUNPTR) :: close
  END TYPE httprawtlshooks

  ENUM, BIND(C)
    ENUMERATOR :: c_abstract_http_sse_state_connecting = 0_C_INT
    ENUMERATOR :: c_abstract_http_sse_state_connected = 1_C_INT
    ENUMERATOR :: c_abstract_http_sse_state_reconnecting = 2_C_INT
    ENUMERATOR :: c_abstract_http_sse_state_closed = 3_C_INT
  END ENUM

  TYPE, BIND(C) :: c_abstract_http_sse_event
    TYPE(C_PTR) :: id
    !< Event ID, or NULL if none
    TYPE(C_PTR) :: event
    !< Event type (e.g., "message")
    TYPE(C_PTR) :: data_
    !< Event data payload
    INTEGER(C_SIZE_T) :: data_len
  END TYPE c_abstract_http_sse_event

  TYPE, BIND(C) :: c_abstract_http_sse_config
    TYPE(C_PTR) :: last_event_id
    !< Optional Last-Event-ID for resuming streams
    INTEGER(C_INT32_T) :: retry_timeout_ms
  END TYPE c_abstract_http_sse_config

  ABSTRACT INTERFACE
    FUNCTION c_abstract_http_sse_on_event_cb(ev, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: c_abstract_http_sse_event, C_PTR
      IMPLICIT NONE
      TYPE(c_abstract_http_sse_event), INTENT(IN) :: ev
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_on_event_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION c_abstract_http_sse_on_error_cb(error_code, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_INT32_T, C_PTR
      IMPLICIT NONE
      INTEGER(C_INT32_T), VALUE :: error_code
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_on_error_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION c_abstract_http_sse_on_close_cb(user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_on_close_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION http_send_fn_cb(ctx, req, res) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), VALUE :: res
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_send_fn_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION http_send_multi_fn_cb(ctx, loop, multi, futures) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), VALUE :: futures
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_send_multi_fn_cb
  END INTERFACE

  !> @brief Opaque structure representing the thread pool.
  !
  TYPE, BIND(C) :: abstracthttpthreadpool
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpthreadpool

  TYPE, BIND(C) :: httpclient
    TYPE(C_PTR) :: transport
    !< Backend-specific context (CURL*, HINTERNET, etc.)
    TYPE(C_FUNPTR) :: send
    !< Function pointer to execute requests
    TYPE(C_FUNPTR) :: send_multi
    !< Function pointer for multi/async requests
    TYPE(C_PTR) :: base_url
    !< Base URL for API calls
    TYPE(httpconfig) :: config
    ! Runtime State
    TYPE(C_PTR) :: loop
    !< Event loop for async mode
    TYPE(C_PTR) :: thread_pool
    !< Thread pool for multi-threaded mode
    TYPE(C_PTR) :: message_bus
    !< Message bus for actor message-passing mode
    INTEGER(C_INT32_T) :: use_multiprocess_ipc
  END TYPE httpclient

  TYPE, BIND(C) :: modalitycontext
    INTEGER(C_INT) :: modality
    !< The specific modality selected
    TYPE(C_PTR) :: internal_ctx
  END TYPE modalitycontext

  ENUM, BIND(C)
    ENUMERATOR :: http_version_default = 0_C_INT
    ENUMERATOR :: http_version_1_0 = 1_C_INT
    ENUMERATOR :: http_version_1_1 = 2_C_INT
    ENUMERATOR :: http_version_2 = 4_C_INT
    ENUMERATOR :: http_version_3 = 8_C_INT
    ENUMERATOR :: http_version_any = 15_C_INT
  END ENUM

  ENUM, BIND(C)
    ENUMERATOR :: http_tls_version_default = 0_C_INT
    ENUMERATOR :: http_tls_version_1_0 = 1_C_INT
    ENUMERATOR :: http_tls_version_1_1 = 2_C_INT
    ENUMERATOR :: http_tls_version_1_2 = 4_C_INT
    ENUMERATOR :: http_tls_version_1_3 = 8_C_INT
    ENUMERATOR :: http_tls_version_any = 15_C_INT
  END ENUM

  ABSTRACT INTERFACE
    FUNCTION http_multi_progress_cb_cb(current_bytes, total_bytes, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_SIZE_T, C_PTR
      IMPLICIT NONE
      INTEGER(C_SIZE_T), VALUE :: current_bytes
      INTEGER(C_SIZE_T), VALUE :: total_bytes
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION http_multi_progress_cb_cb
  END INTERFACE

  INTEGER(C_INT64_T), PARAMETER :: inline = 0_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: f_bool = 0_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: f_forceinline = 0_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: enotsup = 0_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: enosys = 0_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: ecanceled = 0_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: eloop = 40_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: econnrefused = 111_C_INT64_T

  INTEGER(C_INT64_T), PARAMETER :: ehostunreach = 113_C_INT64_T

  CHARACTER(LEN=*), PARAMETER :: c_abstract_http_num_format = "%I64d"

  CHARACTER(LEN=*), PARAMETER :: abstract_http_int64_fmt = "%I64d"


  CHARACTER(LEN=*), PARAMETER :: num_long_fmt = "l"


  !> @brief 64-bit signed integer for legacy MSVC
  ! Typedef alias: abstract_http_int64_t
  !> @brief 64-bit unsigned integer for legacy MSVC
  ! Typedef alias: abstract_http_uint64_t
  ENUM, BIND(C)
    ENUMERATOR :: c_abstract_http_ws_opcode_continuation = 0_C_INT
    ENUMERATOR :: c_abstract_http_ws_opcode_text = 1_C_INT
    ENUMERATOR :: c_abstract_http_ws_opcode_binary = 2_C_INT
    ENUMERATOR :: c_abstract_http_ws_opcode_close = 8_C_INT
    ENUMERATOR :: c_abstract_http_ws_opcode_ping = 9_C_INT
    ENUMERATOR :: c_abstract_http_ws_opcode_pong = 10_C_INT
  END ENUM

  ENUM, BIND(C)
    ENUMERATOR :: c_abstract_http_ws_state_connecting = 0_C_INT
    ENUMERATOR :: c_abstract_http_ws_state_open = 1_C_INT
    ENUMERATOR :: c_abstract_http_ws_state_closing = 2_C_INT
    ENUMERATOR :: c_abstract_http_ws_state_closed = 3_C_INT
  END ENUM

  TYPE, BIND(C) :: c_abstract_http_ws_event
    INTEGER(C_INT) :: opcode
    !< Frame opcode
    TYPE(C_PTR) :: payload
    !< Pointer to payload data
    INTEGER(C_SIZE_T) :: payload_len
    !< Length of payload data
    INTEGER(C_INT32_T) :: is_fin
  END TYPE c_abstract_http_ws_event

  TYPE, BIND(C) :: c_abstract_http_ws_config
    TYPE(C_PTR) :: custom_headers
    !< Null-terminated array of custom headers
    TYPE(C_PTR) :: subprotocols
    !< Comma-separated list of subprotocols
    INTEGER(C_INT32_T) :: masking_preference
  END TYPE c_abstract_http_ws_config

  ABSTRACT INTERFACE
    FUNCTION c_abstract_http_ws_on_message_cb(ev, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: c_abstract_http_ws_event, C_PTR
      IMPLICIT NONE
      TYPE(c_abstract_http_ws_event), INTENT(IN) :: ev
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_on_message_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION c_abstract_http_ws_on_error_cb(error_code, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_INT32_T, C_PTR
      IMPLICIT NONE
      INTEGER(C_INT32_T), VALUE :: error_code
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_on_error_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION c_abstract_http_ws_on_close_cb(status_code, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_INT32_T, C_PTR
      IMPLICIT NONE
      INTEGER(C_INT32_T), VALUE :: status_code
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_on_close_cb
  END INTERFACE

  INTEGER(C_INT64_T), PARAMETER :: log_debug = 0_C_INT64_T









  TYPE, BIND(C) :: abstracthttpipcpipe
    TYPE(C_PTR) :: read_handle
    !< Platform-specific read handle (e.g. fd or HANDLE)
    TYPE(C_PTR) :: write_handle
  END TYPE abstracthttpipcpipe

  !> @brief Opaque process handle.
  !
  TYPE, BIND(C) :: abstracthttpprocess
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpprocess

  TYPE, BIND(C) :: abstracthttpprocesshooks
    !> @brief Hook for spawning a process
    TYPE(C_FUNPTR) :: spawn
    !> @brief Hook for waiting and freeing a process
    TYPE(C_FUNPTR) :: wait_and_free
    !> @brief Hook for IPC write
    TYPE(C_FUNPTR) :: ipc_write
    !> @brief Hook for IPC read
    TYPE(C_FUNPTR) :: ipc_read
  END TYPE abstracthttpprocesshooks

  ABSTRACT INTERFACE
    FUNCTION abstract_http_thread_task_cb_cb(arg) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: arg
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstract_http_thread_task_cb_cb
  END INTERFACE

  TYPE, BIND(C) :: abstracthttpthreadpoolhooks
    TYPE(C_PTR) :: external_context
    !< Context passed to hooks
    TYPE(C_FUNPTR) :: push
  END TYPE abstracthttpthreadpoolhooks

  !> @brief Opaque mutex type.
  !
  TYPE, BIND(C) :: abstracthttpmutex
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpmutex

  !> @brief Opaque condition variable type.
  !
  TYPE, BIND(C) :: abstracthttpcond
    TYPE(C_PTR) :: ptr = C_NULL_PTR
  END TYPE abstracthttpcond

  INTEGER(C_INT64_T), PARAMETER :: snprintf = 0_C_INT64_T

  CHARACTER(LEN=*), PARAMETER :: size_t_fmt = "Iu"

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_bus_init_cb(bus) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_bus_init_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_bus_free_cb(bus) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_bus_free_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_bus_process_cb(bus, out_processed) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      TYPE(C_PTR), VALUE :: out_processed
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_bus_process_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_actor_spawn_cb(bus, name, handler, state, actor) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_CHAR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: name
      TYPE(C_PTR), VALUE :: handler
      TYPE(C_PTR), VALUE :: state
      TYPE(C_PTR), VALUE :: actor
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_actor_spawn_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_actor_send_cb(bus, msg) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, abstracthttpmessage
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      TYPE(abstracthttpmessage), INTENT(IN) :: msg
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_actor_send_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_actor_get_state_cb(actor, state) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: actor
      TYPE(C_PTR), VALUE :: state
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_actor_get_state_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpactorhooks_actor_get_name_cb(actor, name) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: actor
      TYPE(C_PTR), VALUE :: name
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpactorhooks_actor_get_name_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION cmpprogressbinding_update_progress_cb(ui_component, percentage) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FLOAT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ui_component
      REAL(C_FLOAT), VALUE :: percentage
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION cmpprogressbinding_update_progress_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpcoroutinehooks_init_cb(co, stack_size, cb, arg) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      INTEGER(C_SIZE_T), VALUE :: stack_size
      TYPE(C_PTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: arg
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpcoroutinehooks_init_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpcoroutinehooks_free_cb(co) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpcoroutinehooks_free_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpcoroutinehooks_resume_cb(co) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpcoroutinehooks_resume_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpcoroutinehooks_yield_cb() BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpcoroutinehooks_yield_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpcoroutinehooks_is_done_cb(co, out_is_done) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      TYPE(C_PTR), VALUE :: out_is_done
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpcoroutinehooks_is_done_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httploophooks_add_fd_cb(ctx, fd, events, cb, user_data) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT32_T), VALUE :: fd
      INTEGER(C_INT32_T), VALUE :: events
      TYPE(C_PTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httploophooks_add_fd_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httploophooks_mod_fd_cb(ctx, fd, events) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT32_T), VALUE :: fd
      INTEGER(C_INT32_T), VALUE :: events
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httploophooks_mod_fd_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httploophooks_remove_fd_cb(ctx, fd) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT32_T), VALUE :: fd
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httploophooks_remove_fd_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httploophooks_add_timer_cb(ctx, timeout_ms, cb, user_data, out_timer_id) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: timeout_ms
      TYPE(C_PTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR), VALUE :: out_timer_id
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httploophooks_add_timer_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httploophooks_cancel_timer_cb(ctx, timer_id) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT32_T), VALUE :: timer_id
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httploophooks_cancel_timer_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httploophooks_wakeup_cb(ctx) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httploophooks_wakeup_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httprawtlshooks_open_cb(provider_data, sock_fd, host, config, out_session) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_CHAR, httpconfig
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: provider_data
      INTEGER(C_INT32_T), VALUE :: sock_fd
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: host
      TYPE(httpconfig), INTENT(IN) :: config
      TYPE(C_PTR), VALUE :: out_session
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httprawtlshooks_open_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httprawtlshooks_handshake_cb(session, out_want) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: session
      TYPE(C_PTR), VALUE :: out_want
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httprawtlshooks_handshake_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httprawtlshooks_read_cb(session, buf, cap, out_n, out_want) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: session
      TYPE(C_PTR), VALUE :: buf
      INTEGER(C_SIZE_T), VALUE :: cap
      TYPE(C_PTR), VALUE :: out_n
      TYPE(C_PTR), VALUE :: out_want
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httprawtlshooks_read_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httprawtlshooks_write_cb(session, buf, len, out_n, out_want) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: session
      TYPE(C_PTR), VALUE :: buf
      INTEGER(C_SIZE_T), VALUE :: len
      TYPE(C_PTR), VALUE :: out_n
      TYPE(C_PTR), VALUE :: out_want
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httprawtlshooks_write_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION httprawtlshooks_close_cb(session) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: session
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION httprawtlshooks_close_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpprocesshooks_spawn_cb(proc, p2c, c2p) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: proc
      TYPE(C_PTR), VALUE :: p2c
      TYPE(C_PTR), VALUE :: c2p
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpprocesshooks_spawn_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpprocesshooks_wait_and_free_cb(proc, exit_code) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: proc
      TYPE(C_PTR), VALUE :: exit_code
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpprocesshooks_wait_and_free_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpprocesshooks_ipc_write_cb(handle, data_, len) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: handle
      TYPE(C_PTR), VALUE :: data_
      INTEGER(C_SIZE_T), VALUE :: len
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpprocesshooks_ipc_write_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpprocesshooks_ipc_read_cb(handle, data_, len) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: handle
      TYPE(C_PTR), VALUE :: data_
      INTEGER(C_SIZE_T), VALUE :: len
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpprocesshooks_ipc_read_cb
  END INTERFACE

  ABSTRACT INTERFACE
    FUNCTION abstracthttpthreadpoolhooks_push_cb(ctx, cb, arg) BIND(C) RESULT(cdd_ret_)
      IMPORT :: C_PTR
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: arg
      TYPE(C_PTR) :: cdd_ret_
    END FUNCTION abstracthttpthreadpoolhooks_push_cb
  END INTERFACE

  INTERFACE
    !> @brief Register external actor hooks.
    !> @param[in] hooks The hooks structure.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_actor_set_hooks(hooks) BIND(C, NAME="abstract_http_actor_set_hooks") RESULT(cdd_ret_)
      IMPORT :: abstracthttpactorhooks, C_INT
      IMPLICIT NONE
      TYPE(abstracthttpactorhooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_actor_set_hooks

    !> @brief Initialize a new global message bus.
    !> @param[out] bus Pointer to receive the bus handle.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_message_bus_init(bus) BIND(C, NAME="abstract_http_message_bus_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: bus
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_message_bus_init

    !> @brief Free a message bus.
    !> @param[in] bus The bus handle.
    !> @return 0 on success, or an error code.
    !
    FUNCTION abstract_http_message_bus_free(bus) BIND(C, NAME="abstract_http_message_bus_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_message_bus_free

    !> @brief Process all pending messages in the bus queue (run the event loop).
    ! Non-blocking if the queue is empty unless blocked explicitly by the
    ! implementation.
    !> @param[in] bus The bus handle.
    !> @param[out] out_processed Pointer to store the number of messages processed.
    !> @return Error code.
    !
    FUNCTION abstract_http_message_bus_process(bus, out_processed) BIND(C, NAME="abstract_http_message_bus_process")  &
      &RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      INTEGER(C_INT32_T), INTENT(OUT) :: out_processed
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_message_bus_process

    !> @brief Spawn a new Actor and register it to the bus.
    !> @param[in] bus The bus handle.
    !> @param[in] name The human-readable name of the actor (for debugging/routing).
    !> @param[in] handler The callback function for incoming messages.
    !> @param[in] state Arbitrary state payload attached to the actor.
    !> @param[out] actor Pointer to receive the actor handle.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_actor_spawn(bus, name, handler, state, actor) BIND(C, NAME="abstract_http_actor_spawn")  &
      &RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_CHAR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: name
      TYPE(C_FUNPTR), VALUE :: handler
      TYPE(C_PTR), VALUE :: state
      TYPE(C_PTR), INTENT(OUT) :: actor
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_actor_spawn

    !> @brief Send a message directly to a target Actor asynchronously.
    !> @param[in] bus The bus handle.
    !> @param[in] msg The message structure (copied internally).
    !> @return 0 on success.
    !
    FUNCTION abstract_http_actor_send(bus, msg) BIND(C, NAME="abstract_http_actor_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, abstracthttpmessage, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: bus
      TYPE(abstracthttpmessage), INTENT(IN) :: msg
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_actor_send

    !> @brief Retrieve the state of an actor.
    !> @param[in] actor The actor handle.
    !> @param[out] state Pointer to receive the state.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_actor_get_state(actor, state) BIND(C, NAME="abstract_http_actor_get_state") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: actor
      TYPE(C_PTR), INTENT(OUT) :: state
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_actor_get_state

    !> @brief Retrieve the name of an actor.
    !> @param[in] actor The actor handle.
    !> @param[out] name Pointer to receive the name.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_actor_get_name(actor, name) BIND(C, NAME="abstract_http_actor_get_name") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: actor
      TYPE(C_PTR), INTENT(OUT) :: name
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_actor_get_name

    !> @brief Sets a thread-local value for the given key.
    !
    !> @param[in] key The TLS key.
    !> @param[in] value The value to store.
    !> @return 0 on success, or an error code on failure.
    !
    FUNCTION abstract_http_tls_set(key, value_) BIND(C, NAME="abstract_http_tls_set") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: key
      TYPE(C_PTR), VALUE :: value_
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_tls_set

    !> @brief Gets the thread-local value for the given key.
    !
    !> @param[in] key The TLS key.
    !> @param[out] out_value Pointer to a void pointer to store the retrieved value.
    !> @return 0 on success, or an error code on failure.
    !
    FUNCTION abstract_http_tls_get(key, out_value) BIND(C, NAME="abstract_http_tls_get") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: key
      TYPE(C_PTR), INTENT(OUT) :: out_value
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_tls_get

    !> @brief Deletes a TLS key.
    !
    !> @param[in] key The TLS key to delete.
    !> @return 0 on success, or an error code on failure.
    !
    FUNCTION abstract_http_tls_key_delete(key) BIND(C, NAME="abstract_http_tls_key_delete") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: key
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_tls_key_delete

    !> @brief Converts a c-multiplatform modality to a c-abstract-http modality.
    !
    !> @param cmp_mod The c-multiplatform modality (e.g.,
    ! CMP_MODALITY_ASYNC_SINGLE).
    !> @param out_mod Pointer to store the resulting ExecutionModality.
    !> @return 0 on success, EINVAL on invalid input.
    !
    FUNCTION cmp_http_modality_adapter(cmp_mod, out_mod) BIND(C, NAME="cmp_http_modality_adapter") RESULT(cdd_ret_)
      IMPORT :: C_INT32_T, C_PTR, C_INT
      IMPLICIT NONE
      INTEGER(C_INT32_T), VALUE :: cmp_mod
      TYPE(C_PTR), VALUE :: out_mod
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION cmp_http_modality_adapter

    !> @brief Injects c-multiplatform configuration into the HTTP configuration.
    !
    ! Applies the framework's modality and thread pool limits to the HTTP layer.
    !
    !> @param cmp_config Pointer to the framework configuration.
    !> @param http_config Pointer to the HTTP configuration to update.
    !> @return 0 on success, EINVAL on invalid parameters.
    !
    FUNCTION cmp_http_inject_config(cmp_config, http_config) BIND(C, NAME="cmp_http_inject_config") RESULT(cdd_ret_)
      IMPORT :: cmpappconfig, C_PTR, C_INT
      IMPLICIT NONE
      TYPE(cmpappconfig), INTENT(IN) :: cmp_config
      TYPE(C_PTR), VALUE :: http_config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION cmp_http_inject_config

    !> @brief Default progress callback that hooks into the CmpProgressBinding.
    !> @param current_bytes Bytes transferred so far.
    !> @param total_bytes Total bytes expected.
    !> @param user_data Should be a pointer to CmpProgressBinding.
    !> @return 0 to continue, non-zero to abort (e.g. if cancel_requested is true).
    !
    FUNCTION cmp_http_progress_adapter(current_bytes, total_bytes, user_data) BIND(C,  &
      &NAME="cmp_http_progress_adapter") RESULT(cdd_ret_)
      IMPORT :: C_SIZE_T, C_PTR, C_INT
      IMPLICIT NONE
      INTEGER(C_SIZE_T), VALUE :: current_bytes
      INTEGER(C_SIZE_T), VALUE :: total_bytes
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION cmp_http_progress_adapter

    !> @brief Bind a `CmpProgressBinding` (progress hook + cooperative cancellation
    ! flag) onto an `HttpRequest`.
    !
    ! Configures `req->on_progress`, `req->on_progress_user_data`, and
    ! `req->cancel_token` (`&binding->cancel_requested`). Passing `binding == NULL`
    ! detaches any active CMP binding from `req`.
    !
    !> @param req Target HTTP request to configure.
    !> @param binding Optional CMP progress/cancellation state (must outlive `req`).
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or C_ABSTRACT_HTTP_ERR_INVAL if
    ! `req` is NULL.
    !
    FUNCTION cmp_http_bind_request(req, binding) BIND(C, NAME="cmp_http_bind_request") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      TYPE(C_PTR), VALUE :: binding
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION cmp_http_bind_request

    !> @brief Register external coroutine hooks.
    !> @param[in] hooks The hooks structure.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_coroutine_set_hooks(hooks) BIND(C, NAME="abstract_http_coroutine_set_hooks") RESULT(cdd_ret_)
      IMPORT :: abstracthttpcoroutinehooks, C_INT
      IMPLICIT NONE
      TYPE(abstracthttpcoroutinehooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_set_hooks

    !> @brief Initialize a new coroutine.
    !> @param[out] co Pointer to receive the allocated coroutine handle.
    !> @param[in] stack_size Stack size in bytes (0 for default).
    !> @param[in] cb The entry point function.
    !> @param[in] arg Argument to pass to the entry point.
    !> @return 0 on success, ENOMEM or EINVAL on failure.
    !
    FUNCTION abstract_http_coroutine_init(co, stack_size, cb, arg) BIND(C, NAME="abstract_http_coroutine_init")  &
      &RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: co
      INTEGER(C_SIZE_T), VALUE :: stack_size
      TYPE(C_FUNPTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: arg
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_init

    !> @brief Free resources associated with a coroutine.
    ! Must be called after the coroutine finishes.
    !> @param[in] co The coroutine handle.
    !> @return 0 on success, or an error code.
    !
    FUNCTION abstract_http_coroutine_free(co) BIND(C, NAME="abstract_http_coroutine_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_free

    !> @brief Transfer execution to the coroutine.
    ! The caller will block until the coroutine calls
    ! `abstract_http_coroutine_yield` or finishes.
    !> @param[in] co The coroutine handle.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_coroutine_resume(co) BIND(C, NAME="abstract_http_coroutine_resume") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_resume

    !> @brief Yield execution back to the resumer.
    ! MUST be called from within the currently active coroutine.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_coroutine_yield() BIND(C, NAME="abstract_http_coroutine_yield") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_yield

    !> @brief Check if the coroutine has finished executing.
    !> @param[in] co The coroutine handle.
    !> @param[out] out_is_done Pointer to store 1 if finished, 0 if still active.
    !> @return Error enum.
    !
    FUNCTION abstract_http_coroutine_is_done(co, out_is_done) BIND(C, NAME="abstract_http_coroutine_is_done") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: co
      INTEGER(C_INT32_T), INTENT(OUT) :: out_is_done
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_coroutine_is_done

    !> @brief Initialize an event loop context using the platform's optimal backend.
    !> @param[out] loop Pointer to receive the allocated loop.
    !> @return 0 on success, ENOMEM on allocation failure.
    !
    FUNCTION http_loop_init(loop) BIND(C, NAME="http_loop_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_init

    !> @brief Initialize an event loop context with an explicit I/O backend.
    !> @param[out] loop Pointer to receive the allocated loop.
    !> @param[in] backend Desired `enum HttpLoopBackend` variant.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, C_ABSTRACT_HTTP_ERR_NOTSUP if
    ! the requested backend is unavailable on the host OS, or error code.
    !
    FUNCTION http_loop_init_with_backend(loop, backend) BIND(C, NAME="http_loop_init_with_backend") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: loop
      INTEGER(C_INT), VALUE :: backend
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_init_with_backend

    !> @brief Query the active I/O multiplexing backend of an event loop.
    !> @param[in] loop Event loop context.
    !> @param[out] out_backend Pointer receiving active `enum HttpLoopBackend`.
    !> @return C_ABSTRACT_HTTP_SUCCESS or C_ABSTRACT_HTTP_ERR_INVAL.
    !
    FUNCTION http_loop_get_backend(loop, out_backend) BIND(C, NAME="http_loop_get_backend") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT), INTENT(OUT) :: out_backend
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_get_backend

    !> @brief Initialize an event loop context that proxies to an external reactor.
    !> @param[out] loop Pointer to receive the allocated loop.
    !> @param[in] hooks The external hooks to use.
    !> @return 0 on success, ENOMEM on allocation failure.
    !
    FUNCTION http_loop_init_external(loop, hooks) BIND(C, NAME="http_loop_init_external") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httploophooks, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: loop
      TYPE(httploophooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_init_external

    !> @brief Free an event loop context.
    !> @param[in] loop The event loop context.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_loop_free(loop) BIND(C, NAME="http_loop_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_free

    !> @brief Run the event loop. Mimics Node's uv_run.
    ! Blocks until no more active handles/timers exist or stop is called.
    !> @param[in] loop The event loop context.
    !> @return 0 on success.
    !
    FUNCTION http_loop_run(loop) BIND(C, NAME="http_loop_run") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_run

    !> @brief Run a single iteration of the event loop.
    ! Non-blocking by default. Useful for hooking into a framework's idle phase
    ! (e.g. cmp_run_loop).
    !> @param[in] loop The event loop context.
    !> @return 0 on success, non-zero on failure.
    !
    FUNCTION http_loop_tick(loop) BIND(C, NAME="http_loop_tick") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_tick

    !> @brief Stop the event loop from running further.
    !> @param[in] loop The event loop context.
    !> @return 0 on success.
    !
    FUNCTION http_loop_stop(loop) BIND(C, NAME="http_loop_stop") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_stop

    !> @brief Register a file descriptor/socket for polling.
    !> @param[in] loop Event loop context.
    !> @param[in] fd File descriptor to poll.
    !> @param[in] events Bitmask of HttpLoopEvent.
    !> @param[in] cb Callback to trigger.
    !> @param[in] user_data User context for callback.
    !> @return 0 on success, ENOMEM or EINVAL on error.
    !
    FUNCTION http_loop_add_fd(loop, fd, events, cb, user_data) BIND(C, NAME="http_loop_add_fd") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT32_T), VALUE :: fd
      INTEGER(C_INT32_T), VALUE :: events
      TYPE(C_FUNPTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_add_fd

    !> @brief Modify the polled events for a file descriptor.
    !> @param[in] loop Event loop context.
    !> @param[in] fd File descriptor.
    !> @param[in] events New bitmask.
    !> @return 0 on success, ENOENT if not found.
    !
    FUNCTION http_loop_mod_fd(loop, fd, events) BIND(C, NAME="http_loop_mod_fd") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT32_T), VALUE :: fd
      INTEGER(C_INT32_T), VALUE :: events
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_mod_fd

    !> @brief Remove a file descriptor from polling.
    !> @param[in] loop Event loop context.
    !> @param[in] fd File descriptor.
    !> @return 0 on success, ENOENT if not found.
    !
    FUNCTION http_loop_remove_fd(loop, fd) BIND(C, NAME="http_loop_remove_fd") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT32_T), VALUE :: fd
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_remove_fd

    !> @brief Register a timeout using the internal min-heap.
    !> @param[in] loop Event loop context.
    !> @param[in] timeout_ms Milliseconds until timeout.
    !> @param[in] cb Callback to trigger.
    !> @param[in] user_data User context for callback.
    !> @param[out] out_timer_id Pointer to receive the timer ID.
    !> @return 0 on success, ENOMEM on failure.
    !
    FUNCTION http_loop_add_timer(loop, timeout_ms, cb, user_data, out_timer_id) BIND(C, NAME="http_loop_add_timer")  &
      &RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      TYPE(C_PTR), VALUE :: timeout_ms
      TYPE(C_FUNPTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT32_T), INTENT(OUT) :: out_timer_id
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_add_timer

    !> @brief Cancel a timer.
    !> @param[in] loop Event loop context.
    !> @param[in] timer_id Timer ID to cancel.
    !> @return 0 on success, ENOENT if not found.
    !
    FUNCTION http_loop_cancel_timer(loop, timer_id) BIND(C, NAME="http_loop_cancel_timer") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT32_T), VALUE :: timer_id
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_cancel_timer

    !> @brief Signal the event loop to wake up from an external thread.
    ! Used for connecting WinHTTP's asynchronous callbacks.
    !> @param[in] loop Event loop context.
    !> @return 0 on success.
    !
    FUNCTION http_loop_wakeup(loop) BIND(C, NAME="http_loop_wakeup") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_loop_wakeup

    !> @brief Return the current monotonic/wall clock timestamp in milliseconds.
    !> @return Current timestamp in milliseconds.
    !
    FUNCTION real_math_get_current_time_ms() BIND(C, NAME="real_math_get_current_time_ms") RESULT(cdd_ret_)
      IMPORT :: C_INT64_T
      IMPLICIT NONE
      INTEGER(C_INT64_T) :: cdd_ret_
    END FUNCTION real_math_get_current_time_ms

    !> @brief Clear the stop_requested flag on an event loop for test reuse.
    !> @param[in,out] loop Event loop context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success.
    !
    FUNCTION abstract_http_event_loop_test_unstop(loop) BIND(C, NAME="abstract_http_event_loop_test_unstop") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: loop
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_event_loop_test_unstop

    !> @brief Initialize global Apple networking environment.
    !> @return 0 on success.
    !
    FUNCTION http_apple_global_init() BIND(C, NAME="http_apple_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_global_init

    !> @brief Cleanup global Apple networking environment.
    !> @return 0 on success.
    !
    FUNCTION http_apple_global_cleanup() BIND(C, NAME="http_apple_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_global_cleanup

    !> @brief Create a new Apple-backed transport context.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return 0 on success.
    !
    FUNCTION http_apple_context_init(ctx) BIND(C, NAME="http_apple_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_context_init

    !> @brief Free the transport context.
    !
    !> @param[in] ctx The context to free. Satisfies NULL-safety.
    !> @return 0 on success.
    !
    FUNCTION http_apple_context_free(ctx) BIND(C, NAME="http_apple_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_context_free

    !> @brief Apply configuration settings to the Apple session.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration to apply.
    !> @return 0 on success.
    !
    FUNCTION http_apple_config_apply(ctx, config) BIND(C, NAME="http_apple_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_config_apply

    !> @brief Select between the modern NSURLSession C-bridge and legacy CFStream.
    !
    ! When compiled with `MACOSX_DEPLOYMENT_TARGET >= 10.11`, deprecated
    ! `CFReadStreamCreateForHTTPRequest` APIs are omitted at compile time and
    ! attempting to disable `NSURLSession` returns `C_ABSTRACT_HTTP_ERR_NOTSUP`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] enabled Non-zero to dispatch via `NSURLSession`, 0 for `CFStream`.
    !> @return `C_ABSTRACT_HTTP_SUCCESS` on success, or an error enum.
    !
    FUNCTION http_apple_set_use_nsurlsession(ctx, enabled) BIND(C, NAME="http_apple_set_use_nsurlsession") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT32_T), VALUE :: enabled
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_set_use_nsurlsession

    !> @brief The send implementation for Apple.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the allocated response object.
    !> @return 0 on success.
    !
    FUNCTION http_apple_send(ctx, req, res) BIND(C, NAME="http_apple_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_send

    !> @brief The send_multi implementation for Apple.
    ! Matches `http_send_multi_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop Optional event loop for asynchronous notification.
    !> @param[in] multi Container holding multiple requests.
    !> @param[out] futures Array of future handles tracking per-request completion.
    !> @return 0 on success.
    !
    FUNCTION http_apple_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_apple_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_apple_send_multi

    !> @brief Initialize the global curl environment safely.
    !
    ! Uses an internal reference counter to ensure `curl_global_init` is called
    ! called exactly once on the first invocation, and `curl_global_cleanup` only
    ! on the last. This allows multiple independent clients to initialize/cleanup
    ! without race conditions (assuming single-threaded startup or external
    ! locking, as C89 lacks mutexes).
    !
    !> @return 0 on success, EIO on failure.
    !
    FUNCTION http_curl_global_init() BIND(C, NAME="http_curl_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_global_init

    !> @brief Decrement the global initialization reference count.
    !
    ! If the count reaches zero, `curl_global_cleanup` is invoked.
    !
    !> @return 0 on success.
    !
    FUNCTION http_curl_global_cleanup() BIND(C, NAME="http_curl_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_global_cleanup

    !> @brief Create a new Curl-backed transport context.
    ! Internal allocates a CURL handle for reuse.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return 0 on success, ENOMEM on allocation failure, EIO on Curl init failure.
    !
    FUNCTION http_curl_context_init(ctx) BIND(C, NAME="http_curl_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_context_init

    !> @brief Free the transport context.
    ! Cleans up the internal CURL handle and any cached configuration.
    !
    !> @param[in] ctx The context to free. Safe to pass NULL.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_curl_context_free(ctx) BIND(C, NAME="http_curl_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_context_free

    !> @brief Apply configuration settings to the CURL handle.
    !
    ! Maps abstract `HttpConfig` settings (timeout, verify peer, proxy, user-agent)
    ! to specific `curl_easy_setopt` calls. This should be called after
    ! context_init and before sending requests if custom settings are required.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration structure to apply.
    !> @return 0 on success, EINVAL if inputs invalid, or EIO if setopt fails.
    !
    FUNCTION http_curl_config_apply(ctx, config) BIND(C, NAME="http_curl_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_config_apply

    !> @brief The send implementation for libcurl.
    ! Matches `http_send_fn` signature.
    !
    ! Performs the HTTP request using the stored CURL handle.
    ! 1. Resets reusable handle state (except sticky options like connection
    ! cache).
    ! 2. Sets Method, URL, Body, and Headers from `req`.
    ! 3. Executes request (`curl_easy_perform`).
    ! 4. Captures Response Code and Body.
    ! 5. Maps CURL errors to system `errno` codes.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response
    ! object.
    !> @return 0 on success, or a mapped error code (e.g. ETIMEDOUT, ECONNREFUSED)
    ! on failure.
    !
    FUNCTION http_curl_send(ctx, req, res) BIND(C, NAME="http_curl_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_send

    !> @brief Asynchronous multi-send implementation for libcurl.
    ! Matches `http_send_multi_fn` signature.
    !
    ! Uses `curl_multi_socket_action` to connect to the ModalityEventLoop.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop The event loop context.
    !> @param[in] multi The multi request structure.
    !> @param[out] futures Array of futures.
    !> @return 0 on success.
    !
    FUNCTION http_curl_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_curl_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_curl_send_multi

    !> @brief Initialize the global fetch environment safely.
    !
    ! Uses an internal reference counter to ensure fetch global initialization
    ! occurs safely without race conditions.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION http_fetch_global_init() BIND(C, NAME="http_fetch_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_global_init

    !> @brief Decrement the global initialization reference count.
    !
    ! If the count reaches zero, global cleanup is invoked.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION http_fetch_global_cleanup() BIND(C, NAME="http_fetch_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_global_cleanup

    !> @brief Create a new Fetch-backed transport context.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION http_fetch_context_init(ctx) BIND(C, NAME="http_fetch_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_context_init

    !> @brief Free the transport context.
    ! Cleans up the internal FETCH handle and any cached configuration.
    !
    !> @param[in] ctx The context to free. Safe to pass NULL.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_fetch_context_free(ctx) BIND(C, NAME="http_fetch_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_context_free

    !> @brief Apply configuration settings to the FETCH context.
    !
    ! Maps abstract `HttpConfig` settings (timeout, verify peer, proxy, user-agent)
    ! to transport context options.
    !
    !> @param[in,out] ctx The transport context.
    !> @param[in] config The configuration structure to apply.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION http_fetch_config_apply(ctx, config) BIND(C, NAME="http_fetch_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_config_apply

    !> @brief The send implementation for libfetch.
    ! Matches `http_send_fn` signature.
    !
    ! Performs the HTTP request using the stored FETCH handle.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response
    ! object.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION http_fetch_send(ctx, req, res) BIND(C, NAME="http_fetch_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_send

    !> @brief Dispatch multiple HTTP requests using libfetch with ModalityEventLoop.
    !
    ! Opens concurrent `FILE *` streams and schedules body consumption across
    ! `ModalityEventLoop`.
    !
    !> @note BSD `libfetch` (`fetchReqHTTP`) performs blocking DNS/TCP/TLS
    ! connection and HTTP header negotiation before returning each `FILE *` stream;
    ! subsequent body reads and completion notifications are multiplexed through
    !> @p loop.
    !
    !> @param[in] ctx Pointer to an initialized HttpTransportContext.
    !> @param[in,out] loop Optional event loop to drive stream completion.
    !> @param[in] multi Multi-request specification.
    !> @param[out] futures Array of returned HttpFuture handles.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION http_fetch_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_fetch_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_fetch_send_multi

    !> @brief Initialize the global libevent environment safely.
    !
    ! Uses an internal reference counter to ensure global socket/library setup is
    ! performed once on the first invocation and torn down only on the last.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_libevent_global_init() BIND(C, NAME="http_libevent_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_global_init

    !> @brief Decrement the global initialization reference count.
    !
    ! Tears down global state when the reference count reaches zero.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success.
    !
    FUNCTION http_libevent_global_cleanup() BIND(C, NAME="http_libevent_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_global_cleanup

    !> @brief Create a new libevent-backed transport context.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or ENOMEM/EIO error code.
    !
    FUNCTION http_libevent_context_init(ctx) BIND(C, NAME="http_libevent_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_context_init

    !> @brief Free the transport context.
    ! Cleans up the internal libevent handle and any cached configuration.
    !
    !> @param[in] ctx The context to free. Safe to pass NULL.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code.
    !
    FUNCTION http_libevent_context_free(ctx) BIND(C, NAME="http_libevent_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_context_free

    !> @brief Apply configuration settings to the libevent transport context.
    !
    ! Stores abstract `HttpConfig` settings (timeouts, TLS verification/pinning,
    ! proxy, user-agent, redirects) for subsequent `evhttp` connections.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration structure to apply.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_libevent_config_apply(ctx, config) BIND(C, NAME="http_libevent_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_config_apply

    !> @brief Synchronous send implementation for `libevent`.
    ! Matches `http_send_fn` signature.
    !
    ! Dispatches an `evhttp_request` over an `event_base` connection (with optional
    ! OpenSSL `bufferevent` TLS wrapping, redirect following, and streaming).
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response
    ! object.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code on
    ! failure.
    !
    FUNCTION http_libevent_send(ctx, req, res) BIND(C, NAME="http_libevent_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_send

    !> @brief Asynchronous multi-request dispatcher for libevent.
    !
    ! Submits concurrent `evhttp_request` operations onto a shared `event_base` and
    ! bridges non-blocking `event_base_loop(EVLOOP_NONBLOCK)` ticks onto
    ! `ModalityEventLoop`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in,out] loop Optional event loop to drive and wake on completion.
    !> @param[in] multi Batch of HTTP requests to execute concurrently.
    !> @param[out] futures Array of futures populated with responses/errors.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or error code on failure.
    !
    FUNCTION http_libevent_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_libevent_send_multi") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libevent_send_multi

    !> @brief Initialize the global libsoup3 environment safely.
    !
    !> @return 0 on success, error code on failure.
    !
    FUNCTION http_libsoup3_global_init() BIND(C, NAME="http_libsoup3_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_global_init

    !> @brief Decrement the global initialization reference count.
    !> @return 0 on success.
    !
    FUNCTION http_libsoup3_global_cleanup() BIND(C, NAME="http_libsoup3_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_global_cleanup

    !> @brief Create a new libsoup3-backed transport context.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return 0 on success, ENOMEM on allocation failure, error on init failure.
    !
    FUNCTION http_libsoup3_context_init(ctx) BIND(C, NAME="http_libsoup3_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_context_init

    !> @brief Free the transport context.
    !
    !> @param[in] ctx The context to free. Safe to pass NULL.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_libsoup3_context_free(ctx) BIND(C, NAME="http_libsoup3_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_context_free

    !> @brief Apply configuration settings to the libsoup3 context.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration structure to apply.
    !> @return 0 on success, EINVAL if inputs invalid.
    !
    FUNCTION http_libsoup3_config_apply(ctx, config) BIND(C, NAME="http_libsoup3_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_config_apply

    !> @brief The send implementation for libsoup3.
    ! Matches `http_send_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request.
    !> @param[out] res The response.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION http_libsoup3_send(ctx, req, res) BIND(C, NAME="http_libsoup3_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_send

    !> @brief Asynchronous multi-request dispatcher for libsoup3.
    !
    ! Submits concurrent requests via `soup_session_send_and_read_async` and drives
    ! `GMainContext` non-blocking iterations bridged onto `ModalityEventLoop`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in,out] loop Optional event loop to drive and notify on completion.
    !> @param[in] multi Batch of HTTP requests to execute concurrently.
    !> @param[out] futures Array of futures populated with responses/errors.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or error code on failure.
    !
    FUNCTION http_libsoup3_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_libsoup3_send_multi") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libsoup3_send_multi

    !> @brief Initialize the global libuv environment safely.
    !
    ! Uses an internal reference counter to ensure global initialization runs once
    ! on the first invocation and tears down only on the last.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_libuv_global_init() BIND(C, NAME="http_libuv_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_global_init

    !> @brief Decrement the global initialization reference count.
    !
    ! Tears down global state when the reference count reaches zero.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success.
    !
    FUNCTION http_libuv_global_cleanup() BIND(C, NAME="http_libuv_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_global_cleanup

    !> @brief Create a new libuv-backed transport context.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or ENOMEM/EIO error code.
    !
    FUNCTION http_libuv_context_init(ctx) BIND(C, NAME="http_libuv_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_context_init

    !> @brief Free the transport context.
    ! Cleans up the internal libuv loop/context and any cached configuration.
    !
    !> @param[in] ctx The context to free. Safe to pass NULL.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code.
    !
    FUNCTION http_libuv_context_free(ctx) BIND(C, NAME="http_libuv_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_context_free

    !> @brief Apply configuration settings to the libuv transport context.
    !
    ! Stores abstract `HttpConfig` settings (timeouts, TLS verification/pinning,
    ! proxy, user-agent, redirects) for subsequent `uv_tcp_t` requests.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration structure to apply.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_libuv_config_apply(ctx, config) BIND(C, NAME="http_libuv_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_config_apply

    !> @brief Synchronous send implementation for `libuv`.
    ! Matches `http_send_fn` signature.
    !
    ! Executes an HTTP/1.1 request over `uv_tcp_t` (with optional TLS memory-BIO
    ! wrapping, HTTP proxy / `CONNECT` tunneling, redirect following, and chunked
    ! streaming).
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response
    ! object.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code on
    ! failure.
    !
    FUNCTION http_libuv_send(ctx, req, res) BIND(C, NAME="http_libuv_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_send

    !> @brief Asynchronous multi-request dispatcher for libuv.
    !
    ! Multiplexes concurrent TCP/TLS-BIO requests on a shared `uv_loop_t` bridged
    ! with `ModalityEventLoop` via `uv_run(UV_RUN_NOWAIT)`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in,out] loop Optional event loop to drive and wake on completion.
    !> @param[in] multi Batch of HTTP requests to execute concurrently.
    !> @param[out] futures Array of futures populated with responses/errors.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or error code on failure.
    !
    FUNCTION http_libuv_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_libuv_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_libuv_send_multi

    !> @brief Initialize global LiteSpeed QUIC library state.
    ! Reference-counted; safe to call multiple times.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_lsquic_global_init() BIND(C, NAME="http_lsquic_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_global_init

    !> @brief Clean up global LiteSpeed QUIC library state.
    ! Automatically cleans up when the reference count drops to 0.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_lsquic_global_cleanup() BIND(C, NAME="http_lsquic_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_global_cleanup

    !> @brief Initialize a new LiteSpeed QUIC transport context.
    !
    !> @param[out] ctx Double pointer to receive the newly allocated context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or C_ABSTRACT_HTTP_ERR_NOMEM on
    ! failure.
    !
    FUNCTION http_lsquic_context_init(ctx) BIND(C, NAME="http_lsquic_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_context_init

    !> @brief Free a LiteSpeed QUIC transport context and its associated resources.
    !
    !> @param[in] ctx The context to free.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_lsquic_context_free(ctx) BIND(C, NAME="http_lsquic_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_context_free

    !> @brief Apply configuration settings to a LiteSpeed QUIC transport context.
    !
    ! Maps abstract settings (timeouts, TLS verification, user_agent, MASQUE/proxy
    ! settings, cookie jar, and HTTP/3 TCP fallback) to the lsquic engine context.
    !
    !> @param[in,out] ctx The target context.
    !> @param[in] config The configuration settings.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code.
    !
    FUNCTION http_lsquic_config_apply(ctx, config) BIND(C, NAME="http_lsquic_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_config_apply

    !> @brief Synchronous single request dispatcher for LiteSpeed QUIC.
    ! Matches `http_send_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code.
    !
    FUNCTION http_lsquic_send(ctx, req, res) BIND(C, NAME="http_lsquic_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_send

    !> @brief Asynchronous multiplexed multi-request dispatcher for LiteSpeed QUIC.
    ! Matches `http_send_multi_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop Optional event loop context to notify on completion.
    !> @param[in] multi Batch of HTTP requests to multiplex over QUIC streams.
    !> @param[out] futures Array of futures populated with responses/errors.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code.
    !
    FUNCTION http_lsquic_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_lsquic_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_lsquic_send_multi

    !> @brief Initialize the global MsH3 API state.
    ! Reference-counted; safe to call multiple times.
    !> @return 0 on success, ENOMEM or native error code on failure.
    !
    FUNCTION http_msh3_global_init() BIND(C, NAME="http_msh3_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_global_init

    !> @brief Clean up the global MsH3 API state.
    ! Automatically cleans up when the reference count drops to 0.
    !> @return 0 on success.
    !
    FUNCTION http_msh3_global_cleanup() BIND(C, NAME="http_msh3_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_global_cleanup

    !> @brief Initialize a new MsH3 transport context.
    !
    !> @param[out] ctx Double pointer to receive the newly allocated context.
    !> @return 0 on success, ENOMEM on failure.
    !
    FUNCTION http_msh3_context_init(ctx) BIND(C, NAME="http_msh3_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_context_init

    !> @brief Free a MsH3 transport context and its associated resources.
    !
    !> @param[in] ctx The context to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_msh3_context_free(ctx) BIND(C, NAME="http_msh3_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_context_free

    !> @brief Apply configuration settings to an MsH3 transport context.
    !
    ! Maps abstract settings (like timeouts and SSL verification) to their
    ! MsH3 equivalents.
    !
    !> @param[in,out] ctx The target context.
    !> @param[in] config The configuration settings.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_msh3_config_apply(ctx, config) BIND(C, NAME="http_msh3_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_config_apply

    !> @brief Synchronous single request dispatcher for MsH3.
    ! Matches `http_send_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_msh3_send(ctx, req, res) BIND(C, NAME="http_msh3_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_send

    !> @brief Asynchronous multi-send implementation for MsH3.
    ! Matches `http_send_multi_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop The event loop context.
    !> @param[in] multi The multi request structure.
    !> @param[out] futures Array of futures.
    !> @return 0 on success.
    !
    FUNCTION http_msh3_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_msh3_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_msh3_send_multi

    !> @brief Initialize global nghttp3 + ngtcp2 library state.
    ! Reference-counted; safe to call multiple times.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_nghttp3_global_init() BIND(C, NAME="http_nghttp3_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_global_init

    !> @brief Clean up global nghttp3 + ngtcp2 library state.
    ! Automatically cleans up when the reference count drops to 0.
    !
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_nghttp3_global_cleanup() BIND(C, NAME="http_nghttp3_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_global_cleanup

    !> @brief Initialize a new nghttp3 + ngtcp2 transport context.
    !
    !> @param[out] ctx Double pointer to receive the newly allocated context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or C_ABSTRACT_HTTP_ERR_NOMEM on
    ! failure.
    !
    FUNCTION http_nghttp3_context_init(ctx) BIND(C, NAME="http_nghttp3_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_context_init

    !> @brief Free an nghttp3 + ngtcp2 transport context and its resources.
    !
    !> @param[in] ctx The context to free.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code on failure.
    !
    FUNCTION http_nghttp3_context_free(ctx) BIND(C, NAME="http_nghttp3_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_context_free

    !> @brief Apply configuration settings to an nghttp3 + ngtcp2 context.
    !
    ! Maps abstract settings (timeouts, TLS verification, user_agent, MASQUE/proxy
    ! settings, cookie jar, and HTTP/3 TCP fallback) to the nghttp3/ngtcp2 state.
    !
    !> @param[in,out] ctx The target context.
    !> @param[in] config The configuration settings.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code.
    !
    FUNCTION http_nghttp3_config_apply(ctx, config) BIND(C, NAME="http_nghttp3_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_config_apply

    !> @brief Synchronous single request dispatcher for nghttp3 + ngtcp2.
    ! Matches `http_send_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code.
    !
    FUNCTION http_nghttp3_send(ctx, req, res) BIND(C, NAME="http_nghttp3_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_send

    !> @brief Asynchronous multiplexed multi-request dispatcher for nghttp3+ngtcp2.
    ! Matches `http_send_multi_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop Optional event loop context to notify on completion.
    !> @param[in] multi Batch of HTTP requests to multiplex over HTTP/3 streams.
    !> @param[out] futures Array of futures populated with responses/errors.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or a mapped error code.
    !
    FUNCTION http_nghttp3_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_nghttp3_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_nghttp3_send_multi

    !> @brief Initialize the global Picoquic API state.
    ! Reference-counted; safe to call multiple times.
    !> @return 0 on success, ENOMEM or native error code on failure.
    !
    FUNCTION http_picoquic_global_init() BIND(C, NAME="http_picoquic_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_global_init

    !> @brief Clean up the global Picoquic API state.
    ! Automatically cleans up when the reference count drops to 0.
    !> @return 0 on success.
    !
    FUNCTION http_picoquic_global_cleanup() BIND(C, NAME="http_picoquic_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_global_cleanup

    !> @brief Initialize a new Picoquic transport context.
    !
    !> @param[out] ctx Double pointer to receive the newly allocated context.
    !> @return 0 on success, ENOMEM on failure.
    !
    FUNCTION http_picoquic_context_init(ctx) BIND(C, NAME="http_picoquic_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_context_init

    !> @brief Free a Picoquic transport context and its associated resources.
    !
    !> @param[in] ctx The context to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_picoquic_context_free(ctx) BIND(C, NAME="http_picoquic_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_context_free

    !> @brief Apply configuration settings to a Picoquic transport context.
    !
    ! Maps abstract settings to Picoquic engine settings.
    !
    !> @param[in,out] ctx The target context.
    !> @param[in] config The configuration settings.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_picoquic_config_apply(ctx, config) BIND(C, NAME="http_picoquic_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_config_apply

    !> @brief Synchronous single request dispatcher for Picoquic.
    ! Matches `http_send_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_picoquic_send(ctx, req, res) BIND(C, NAME="http_picoquic_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_send

    !> @brief Asynchronous multi-send implementation for Picoquic.
    ! Matches `http_send_multi_fn` signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop The event loop context.
    !> @param[in] multi The multi request structure.
    !> @param[out] futures Array of futures.
    !> @return 0 on success.
    !
    FUNCTION http_picoquic_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_picoquic_send_multi") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_picoquic_send_multi

    !> @brief Initialize the global raw socket environment.
    !> @return 0 on success.
    !
    FUNCTION http_raw_global_init() BIND(C, NAME="http_raw_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_global_init

    !> @brief Cleanup the global raw socket environment.
    !> @return 0 on success.
    !
    FUNCTION http_raw_global_cleanup() BIND(C, NAME="http_raw_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_global_cleanup

    !> @brief Initialize a new raw socket transport context.
    !> @param ctx Pointer to the newly allocated context.
    !> @return 0 on success.
    !
    FUNCTION http_raw_context_init(ctx) BIND(C, NAME="http_raw_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_context_init

    !> @brief Free a raw socket transport context.
    !> @param ctx Context to free.
    !> @return 0 on success.
    !
    FUNCTION http_raw_context_free(ctx) BIND(C, NAME="http_raw_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_context_free

    !> @brief Apply configuration to a raw socket transport context.
    !> @param ctx Transport context.
    !> @param config Configuration to apply.
    !> @return 0 on success.
    !
    FUNCTION http_raw_config_apply(ctx, config) BIND(C, NAME="http_raw_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_config_apply

    !> @brief Register or clear custom TLS hooks on a raw socket transport context.
    !> @param ctx Transport context.
    !> @param hooks Optional custom TLS hooks to copy, or NULL to use native
    ! OS/crypto TLS.
    !> @return 0 on success, or C_ABSTRACT_HTTP_ERR_INVAL if @p ctx or required hook
    ! pointers are invalid.
    !
    FUNCTION http_raw_set_tls_hooks(ctx, hooks) BIND(C, NAME="http_raw_set_tls_hooks") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprawtlshooks, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprawtlshooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_set_tls_hooks

    !> @brief Send an HTTP request synchronously using raw sockets.
    !> @param ctx Transport context.
    !> @param req Request to send.
    !> @param res Response to return.
    !> @return 0 on success.
    !
    FUNCTION http_raw_send(ctx, req, res) BIND(C, NAME="http_raw_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), VALUE :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_send

    !> @brief Send multiple HTTP requests asynchronously using raw sockets.
    !> @param ctx Transport context.
    !> @param loop Event loop.
    !> @param reqs Multiple requests to send.
    !> @param future Futures to return.
    !> @return 0 on success.
    !
    FUNCTION http_raw_send_multi(ctx, loop, reqs, future) BIND(C, NAME="http_raw_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: reqs
      TYPE(C_PTR), VALUE :: future
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_raw_send_multi

    !> @brief Initialize an SSE stream request.
    !
    !> @param req The HTTP request to initialize.
    !> @param config Optional SSE configuration.
    !> @return 0 on success, or a negative error code.
    !
    FUNCTION c_abstract_http_sse_init(req, config) BIND(C, NAME="c_abstract_http_sse_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, c_abstract_http_sse_config, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      TYPE(c_abstract_http_sse_config), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_init

    !> @brief Run a blocking loop to read SSE events for the given connection.
    !> @param client The HTTP client.
    !> @param req The connected request.
    !> @param on_evt The callback to trigger on events.
    !> @param on_err The callback to trigger on errors.
    !> @param on_close The callback to trigger when the stream closes permanently.
    !> @param user_data Opaque pointer to pass to the callbacks.
    !> @param exit_flag Optional volatile int pointer. Loop exits cleanly when
    ! *exit_flag != 0.
    !> @return 0 on clean exit, negative error code on failure.
    !
    FUNCTION c_abstract_http_sse_sync_read_loop(client, req, on_evt, on_err, on_close, user_data, exit_flag) BIND(C,  &
      &NAME="c_abstract_http_sse_sync_read_loop") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      TYPE(C_PTR), VALUE :: req
      TYPE(C_FUNPTR), VALUE :: on_evt
      TYPE(C_FUNPTR), VALUE :: on_err
      TYPE(C_FUNPTR), VALUE :: on_close
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR), VALUE :: exit_flag
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_sync_read_loop

    !> @brief Register an SSE connection with the underlying transport's
    ! asynchronous event loop.
    !> @param client The HTTP client.
    !> @param req The connected request.
    !> @param on_evt The callback to trigger on events.
    !> @param on_err The callback to trigger on errors.
    !> @param on_close The callback to trigger when the stream closes permanently.
    !> @param user_data Opaque pointer to pass to the callbacks.
    !> @return 0 on success, negative error code on failure.
    !
    FUNCTION c_abstract_http_sse_async_register(client, req, on_evt, on_err, on_close, user_data) BIND(C,  &
      &NAME="c_abstract_http_sse_async_register") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      TYPE(C_PTR), VALUE :: req
      TYPE(C_FUNPTR), VALUE :: on_evt
      TYPE(C_FUNPTR), VALUE :: on_err
      TYPE(C_FUNPTR), VALUE :: on_close
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_async_register

    !> @brief Clean up internal SSE resources attached to an HttpRequest.
    !
    !> @param req The HTTP request containing the SSE context.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code.
    !
    FUNCTION c_abstract_http_sse_free(req) BIND(C, NAME="c_abstract_http_sse_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_sse_free

    !> @brief Duplicate a NUL-terminated string onto the heap.
    !> @param[in] s Input NUL-terminated string.
    !> @param[out] out_s Output pointer receiving newly allocated string.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, error code on failure.
    !
    FUNCTION c_abstract_http_strdup_2(s, out_s) BIND(C, NAME="c_abstract_http_strdup") RESULT(cdd_ret_)
      IMPORT :: C_CHAR, C_PTR, C_INT
      IMPLICIT NONE
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: s
      TYPE(C_PTR), INTENT(OUT) :: out_s
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_strdup_2

    !> @brief Send a single HTTP request through the unified client middleware
    ! pipeline.
    !
    ! Resolves relative URLs against `client->base_url`, flattens multipart parts,
    ! injects matching cookies from `client->config.cookie_jar`, executes automatic
    ! retries according to `client->config.retry_count` and
    ! `client->config.retry_policy` (including `Retry-After` headers), and parses
    ! response `Set-Cookie` headers back into `client->config.cookie_jar`.
    !
    !> @param[in] client The HTTP client instance.
    !> @param[in,out] req The HTTP request to execute.
    !> @param[out] res Pointer receiving the allocated response on success.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error enum on failure.
    !
    FUNCTION http_client_send(client, req, res) BIND(C, NAME="http_client_send") RESULT(cdd_ret_)
      IMPORT :: httpclient, httprequest, C_PTR, C_INT
      IMPLICIT NONE
      TYPE(httpclient), INTENT(IN) :: client
      TYPE(httprequest), INTENT(INOUT) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_client_send

    !> @brief Dispatch multiple requests concurrently based on the client's
    ! modality.
    !> @param[in] client The HTTP client.
    !> @param[in] requests The array of requests to execute.
    !> @param[in] num_requests Number of requests.
    !> @param[out] futures Array of futures (must be pre-allocated to size
    ! `num_requests`).
    !> @param[in] progress_cb Optional progress callback.
    !> @param[in] user_data User data for the progress callback.
    !> @param[in] fail_fast 1 to abort all remaining requests if one fails, 0 to
    ! continue.
    !> @return 0 on successful dispatch (futures may still report individual
    ! errors), error code otherwise.
    !
    FUNCTION http_client_send_multi(client, requests, num_requests, futures, progress_cb, user_data, fail_fast) BIND( &
      &C, NAME="http_client_send_multi") RESULT(cdd_ret_)
      IMPORT :: httpclient, C_PTR, C_SIZE_T, C_FUNPTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(httpclient), INTENT(IN) :: client
      TYPE(C_PTR), VALUE :: requests
      INTEGER(C_SIZE_T), VALUE :: num_requests
      TYPE(C_PTR), INTENT(OUT) :: futures
      TYPE(C_FUNPTR), VALUE :: progress_cb
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT32_T), VALUE :: fail_fast
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_client_send_multi

    !> @brief Executes the http_headers_init operation.
    !> @param headers The headers parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_headers_init(headers) BIND(C, NAME="http_headers_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: headers
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_headers_init

    !> @brief Executes the http_headers_free operation.
    !> @param headers The headers parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_headers_free(headers) BIND(C, NAME="http_headers_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: headers
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_headers_free

    !> @brief Executes the http_headers_add operation.
    !> @param headers The headers parameter.
    !> @param key The key parameter.
    !> @param value The value parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_headers_add(headers, key, value_) BIND(C, NAME="http_headers_add") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: headers
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: key
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: value_
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_headers_add

    !> @brief Retrieves the value for a specific header key.
    !> @param headers The headers container.
    !> @param key The header key to search for (case-insensitive).
    !> @param out Pointer to store the found header value.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, C_ABSTRACT_HTTP_ERR_INVAL if not
    ! found or on invalid input.
    !
    FUNCTION http_headers_get(headers, key, out_) BIND(C, NAME="http_headers_get") RESULT(cdd_ret_)
      IMPORT :: httpheaders, C_CHAR, C_PTR, C_INT
      IMPLICIT NONE
      TYPE(httpheaders), INTENT(IN) :: headers
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: key
      TYPE(C_PTR), VALUE :: out_
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_headers_get

    !> @brief Removes a header by key.
    !> @param headers The headers container.
    !> @param key The header key to remove (case-insensitive).
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, C_ABSTRACT_HTTP_ERR_INVAL if not
    ! found or on invalid input.
    !
    FUNCTION http_headers_remove(headers, key) BIND(C, NAME="http_headers_remove") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: headers
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: key
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_headers_remove

    !> @brief Initialize a cookie jar.
    !> @param jar The cookie jar to initialize.
    !> @return 0 on success, EINVAL if jar is NULL.
    !
    FUNCTION http_cookie_jar_init(jar) BIND(C, NAME="http_cookie_jar_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: jar
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_cookie_jar_init

    !> @brief Free resources held by a cookie jar.
    !> @param jar The cookie jar to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_cookie_jar_free(jar) BIND(C, NAME="http_cookie_jar_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: jar
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_cookie_jar_free

    !> @brief Add or update a cookie in the jar.
    !> @param jar The cookie jar.
    !> @param name Cookie name.
    !> @param value Cookie value.
    !> @return 0 on success, ENOMEM on failure.
    !
    FUNCTION http_cookie_jar_set(jar, name, value_) BIND(C, NAME="http_cookie_jar_set") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: jar
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: name
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: value_
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_cookie_jar_set

    !> @brief Get a cookie value from the jar.
    !> @param jar The cookie jar.
    !> @param name Cookie name.
    !> @param out Pointer to store the found cookie value string.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, C_ABSTRACT_HTTP_ERR_INVAL if not
    ! found or on invalid input.
    !
    FUNCTION http_cookie_jar_get(jar, name, out_) BIND(C, NAME="http_cookie_jar_get") RESULT(cdd_ret_)
      IMPORT :: httpcookiejar, C_CHAR, C_PTR, C_INT
      IMPLICIT NONE
      TYPE(httpcookiejar), INTENT(IN) :: jar
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: name
      TYPE(C_PTR), VALUE :: out_
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_cookie_jar_get

    !> @brief Parse an RFC 6265 Set-Cookie header string into the cookie jar.
    !
    ! Extracts cookie name, value, Domain, Path, Expires, Max-Age, Secure,
    ! HttpOnly, and SameSite attributes and inserts or updates the matching entry.
    !
    !> @param[in,out] jar Target cookie jar.
    !> @param[in] set_cookie_header Raw Set-Cookie header value.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error enum on failure.
    !
    FUNCTION http_cookie_jar_parse_set_cookie(jar, set_cookie_header) BIND(C,  &
      &NAME="http_cookie_jar_parse_set_cookie") RESULT(cdd_ret_)
      IMPORT :: httpcookiejar, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httpcookiejar), INTENT(INOUT) :: jar
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: set_cookie_header
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_cookie_jar_parse_set_cookie

    !> @brief Build an outgoing RFC 6265 Cookie header value for a request URL.
    !
    ! Filters stored cookies by Domain, Path, Secure scheme, and expiration time.
    ! Sets `*out_cookie_header` to NULL with C_ABSTRACT_HTTP_SUCCESS when no
    ! cookies match.
    !
    !> @param[in] jar Source cookie jar.
    !> @param[in] request_url Target request URL (optional).
    !> @param[out] out_cookie_header Output pointer receiving allocated header
    ! string or NULL.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error enum on failure.
    !
    FUNCTION http_cookie_jar_build_header(jar, request_url, out_cookie_header) BIND(C,  &
      &NAME="http_cookie_jar_build_header") RESULT(cdd_ret_)
      IMPORT :: httpcookiejar, C_CHAR, C_PTR, C_INT
      IMPLICIT NONE
      TYPE(httpcookiejar), INTENT(IN) :: jar
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: request_url
      TYPE(C_PTR), INTENT(OUT) :: out_cookie_header
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_cookie_jar_build_header

    !> @brief Executes the http_config_init operation.
    !> @param config The config parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_config_init(config) BIND(C, NAME="http_config_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_config_init

    !> @brief Executes the http_config_free operation.
    !> @param config The config parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_config_free(config) BIND(C, NAME="http_config_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_config_free

    !> @brief Executes the http_client_init operation.
    !> @param client The client parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_client_init(client) BIND(C, NAME="http_client_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_client_init

    !> @brief Executes the http_client_free operation.
    !> @param client The client parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_client_free(client) BIND(C, NAME="http_client_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_client_free

    !> @brief Executes the http_request_init operation.
    !> @param req The req parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init(req) BIND(C, NAME="http_request_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init

    !> @brief Executes the http_request_free operation.
    !> @param req The req parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_free(req) BIND(C, NAME="http_request_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_free

    !> @brief Initialize a ModalityContext.
    !> @param ctx Context to initialize.
    !> @return 0 on success.
    !
    FUNCTION http_modality_context_init(ctx) BIND(C, NAME="http_modality_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_modality_context_init

    !> @brief Free a ModalityContext.
    !> @param ctx Context to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_modality_context_free(ctx) BIND(C, NAME="http_modality_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_modality_context_free

    !> @brief Initialize a HttpFuture.
    !> @param future Future to initialize.
    !> @return 0 on success.
    !
    FUNCTION http_future_init(future) BIND(C, NAME="http_future_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: future
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_future_init

    !> @brief Free a HttpFuture.
    !> @param future Future to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_future_free(future) BIND(C, NAME="http_future_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: future
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_future_free

    !> @brief Wait for a deferred HttpFuture to complete and return its status enum.
    !> @param[in,out] future Target future handle.
    !> @return Resulting error enum from the completed future, or
    ! C_ABSTRACT_HTTP_ERR_INVAL if unscheduled and unready.
    !
    FUNCTION http_future_await(future) BIND(C, NAME="http_future_await") RESULT(cdd_ret_)
      IMPORT :: httpfuture, C_INT
      IMPLICIT NONE
      TYPE(httpfuture), INTENT(INOUT) :: future
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_future_await

    !> @brief Mark an HttpFuture as cancelled and notify any waiting callers.
    !> @param[in,out] future Target future handle to cancel.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error enum on failure.
    !
    FUNCTION http_future_cancel(future) BIND(C, NAME="http_future_cancel") RESULT(cdd_ret_)
      IMPORT :: httpfuture, C_INT
      IMPLICIT NONE
      TYPE(httpfuture), INTENT(INOUT) :: future
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_future_cancel

    !> @brief Initialize an HttpMultiRequest.
    !> @param multi Multi-request to initialize.
    !> @return 0 on success.
    !
    FUNCTION http_multi_request_init(multi) BIND(C, NAME="http_multi_request_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: multi
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_multi_request_init

    !> @brief Free an HttpMultiRequest.
    !> @param multi Multi-request to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_multi_request_free(multi) BIND(C, NAME="http_multi_request_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: multi
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_multi_request_free

    !> @brief Add a request to a multi-request struct.
    !> @param multi Multi-request struct.
    !> @param req Request to add.
    !> @return 0 on success, ENOMEM on allocation failure.
    !
    FUNCTION http_multi_request_add(multi, req) BIND(C, NAME="http_multi_request_add") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: multi
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_multi_request_add

    !> @brief Executes the http_request_set_auth_bearer operation.
    !> @param req The req parameter.
    !> @param token The token parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_set_auth_bearer(req, token) BIND(C, NAME="http_request_set_auth_bearer") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_set_auth_bearer

    !> @brief Set HTTP Basic Authorization header with a pre-encoded token.
    !
    ! The token must be the base64-encoded "username:password" string.
    !
    !> @param[in,out] req The request to modify.
    !> @param[in] token Base64-encoded "username:password".
    !> @return 0 on success, error code otherwise.
    !
    FUNCTION http_request_set_auth_basic(req, token) BIND(C, NAME="http_request_set_auth_basic") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_set_auth_basic

    !> @brief Set HTTP Basic Authorization header by encoding username and password.
    !
    ! Base64 encodes the "username:password" and applies the Authorization header.
    !
    !> @param[in,out] req The request to modify.
    !> @param[in] username The username.
    !> @param[in] password The password.
    !> @return 0 on success, error code otherwise.
    !
    FUNCTION http_request_set_auth_basic_userpwd(req, username, password) BIND(C,  &
      &NAME="http_request_set_auth_basic_userpwd") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: username
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: password
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_set_auth_basic_userpwd

    !> @brief Construct an OAuth 2.0 Password Grant Request.
    !
    ! Configures the provided HttpRequest object for a POST to the specified token
    ! endpoint with application/x-www-form-urlencoded content.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] token_endpoint_url The URL of the OAuth 2.0 token endpoint.
    !> @param[in] username The resource owner username.
    !> @param[in] password The resource owner password.
    !> @param[in] client_id Optional Client ID.
    !> @param[in] client_secret Optional Client Secret.
    !> @param[in] scope Optional scope.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_password_grant(req, token_endpoint_url, username, password, client_id,  &
      &client_secret, scope) BIND(C, NAME="http_request_init_oauth2_password_grant") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: username
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: password
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_secret
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: scope
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_password_grant

    !> @brief Construct an OAuth 2.0 Refresh Token Grant Request.
    !
    ! Configures the provided HttpRequest object for a POST to the specified token
    ! endpoint with application/x-www-form-urlencoded content.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] token_endpoint_url The URL of the OAuth 2.0 token endpoint.
    !> @param[in] refresh_token The refresh token string.
    !> @param[in] client_id Optional Client ID.
    !> @param[in] client_secret Optional Client Secret.
    !> @param[in] scope Optional scope.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_refresh_token_grant(req, token_endpoint_url, refresh_token, client_id,  &
      &client_secret, scope) BIND(C, NAME="http_request_init_oauth2_refresh_token_grant") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: refresh_token
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_secret
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: scope
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_refresh_token_grant

    !> @brief Construct an OAuth 2.0 Authorization Code Grant Request.
    !
    ! Configures the provided HttpRequest object for a POST to the specified token
    ! endpoint with application/x-www-form-urlencoded content.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] token_endpoint_url The URL of the OAuth 2.0 token endpoint.
    !> @param[in] code The authorization code received from the authorization
    ! server.
    !> @param[in] redirect_uri Optional redirect URI used in the initial
    ! authorization request.
    !> @param[in] client_id Optional Client ID.
    !> @param[in] client_secret Optional Client Secret.
    !> @param[in] code_verifier Optional PKCE code verifier (RFC 7636).
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_authorization_code_grant(req, token_endpoint_url, code, redirect_uri, client_id, &
      &client_secret, code_verifier) BIND(C, NAME="http_request_init_oauth2_authorization_code_grant") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: code
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: redirect_uri
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_secret
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: code_verifier
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_authorization_code_grant

    !> @brief Construct an OAuth 2.0 Client Credentials Grant Request.
    !
    ! Configures the provided HttpRequest object for a POST to the specified token
    ! endpoint with application/x-www-form-urlencoded content.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] token_endpoint_url The URL of the OAuth 2.0 token endpoint.
    !> @param[in] client_id Optional Client ID.
    !> @param[in] client_secret Optional Client Secret.
    !> @param[in] scope Optional scope.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_client_credentials_grant(req, token_endpoint_url, client_id, client_secret,  &
      &scope) BIND(C, NAME="http_request_init_oauth2_client_credentials_grant") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_secret
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: scope
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_client_credentials_grant

    !> @brief Construct an OAuth 2.0 JWT Bearer Grant Request (RFC 7523).
    !
    ! Configures the provided HttpRequest object for a POST to the specified token
    ! endpoint with application/x-www-form-urlencoded content.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] token_endpoint_url The URL of the OAuth 2.0 token endpoint.
    !> @param[in] assertion The JWT string.
    !> @param[in] scope Optional scope.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_jwt_bearer_grant(req, token_endpoint_url, assertion, scope) BIND(C,  &
      &NAME="http_request_init_oauth2_jwt_bearer_grant") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: assertion
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: scope
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_jwt_bearer_grant

    !> @brief Construct an OAuth 2.0 Device Authorization Request (RFC 8628).
    !
    ! Configures the provided HttpRequest object for a POST to the specified device
    ! authorization endpoint.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] device_endpoint_url The URL of the device authorization endpoint.
    !> @param[in] client_id The Client ID.
    !> @param[in] scope Optional scope.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_device_authorization_request(req, device_endpoint_url, client_id, scope) BIND(C, &
      &NAME="http_request_init_oauth2_device_authorization_request") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: device_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: scope
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_device_authorization_request

    !> @brief Construct an OAuth 2.0 Device Access Token Request (RFC 8628).
    !
    ! Configures the provided HttpRequest object for a POST to the token endpoint
    ! to poll for a device code.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] token_endpoint_url The URL of the token endpoint.
    !> @param[in] client_id The Client ID.
    !> @param[in] device_code The device code from the authorization response.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_device_access_token_request(req, token_endpoint_url, client_id, device_code)  &
      &BIND(C, NAME="http_request_init_oauth2_device_access_token_request") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: device_code
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_device_access_token_request

    !> @brief Construct an OAuth 2.0 Token Revocation Request (RFC 7009).
    !
    ! Configures the provided HttpRequest object for a POST to the token revocation
    ! endpoint.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] revocation_endpoint_url The URL of the revocation endpoint.
    !> @param[in] token The token to revoke.
    !> @param[in] token_type_hint Optional token type hint ("access_token" or
    ! "refresh_token").
    !> @param[in] client_id Optional Client ID.
    !> @param[in] client_secret Optional Client Secret.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_token_revocation(req, revocation_endpoint_url, token, token_type_hint,  &
      &client_id, client_secret) BIND(C, NAME="http_request_init_oauth2_token_revocation") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: revocation_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_type_hint
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_secret
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_token_revocation

    !> @brief Construct an OAuth 2.0 Token Introspection Request (RFC 7662).
    !
    ! Configures the provided HttpRequest object for a POST to the token
    ! introspection endpoint.
    !
    !> @param[in,out] req The request object to initialize/configure.
    !> @param[in] introspection_endpoint_url The URL of the introspection endpoint.
    !> @param[in] token The token to introspect.
    !> @param[in] token_type_hint Optional token type hint ("access_token" or
    ! "refresh_token").
    !> @param[in] client_id Optional Client ID.
    !> @param[in] client_secret Optional Client Secret.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_request_init_oauth2_token_introspection(req, introspection_endpoint_url, token, token_type_hint,  &
      &client_id, client_secret) BIND(C, NAME="http_request_init_oauth2_token_introspection") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: introspection_endpoint_url
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: token_type_hint
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_secret
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_init_oauth2_token_introspection

    !> @brief Construct an OAuth 2.0 Authorization Request URL.
    !
    ! Builds the URL to redirect the user's browser to for authorization.
    !
    !> @param[in] auth_endpoint The URL of the authorization endpoint.
    !> @param[in] client_id The Client ID.
    !> @param[in] response_type The response type (e.g., "code", "token").
    !> @param[in] redirect_uri The redirect URI (e.g., "http://127.0.0.1:8080/cb").
    !> @param[in] scope Optional scope.
    !> @param[in] state Optional state string.
    !> @param[in] code_challenge Optional PKCE code challenge (RFC 7636).
    !> @param[in] code_challenge_method Optional PKCE method (e.g., "S256").
    !> @param[out] out_url Pointer to store the newly allocated URL string.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_oauth2_build_authorization_url(auth_endpoint, client_id, response_type, redirect_uri, scope, state,  &
      &code_challenge, code_challenge_method, out_url) BIND(C, NAME="http_oauth2_build_authorization_url") RESULT( &
      &cdd_ret_)
      IMPORT :: C_CHAR, C_PTR, C_INT
      IMPLICIT NONE
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: auth_endpoint
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: client_id
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: response_type
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: redirect_uri
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: scope
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: state
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: code_challenge
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: code_challenge_method
      TYPE(C_PTR), INTENT(OUT) :: out_url
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_oauth2_build_authorization_url

    !> @brief Synchronous localhost server intercept for OAuth 2.0 loopback flow.
    !
    ! Starts a minimal HTTP server on 127.0.0.1 at the specified port, accepts
    ! exactly one incoming connection, parses the HTTP GET request line for
    ! query parameters ('code', 'state', 'error', 'error_description'), and
    ! responds with the provided HTML success/failure message.
    !
    !> @param[in] port Port to bind to (e.g., 8080).
    !> @param[in] html_response Complete HTTP response to send (e.g., "HTTP/1.1 200
    ! OK\\r\\n\\r\\n...").
    !> @param[out] out_code Allocated string containing 'code' (or NULL if not
    ! found).
    !> @param[out] out_state Allocated string containing 'state' (or NULL if not
    ! found).
    !> @param[out] out_error Allocated string containing 'error' (or NULL if not
    ! found).
    !> @param[out] out_error_desc Allocated string containing 'error_description'
    ! (or NULL if not found).
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_oauth2_localhost_intercept(port, html_response, out_code, out_state, out_error, out_error_desc)  &
      &BIND(C, NAME="http_oauth2_localhost_intercept") RESULT(cdd_ret_)
      IMPORT :: C_INT16_T, C_CHAR, C_PTR, C_INT
      IMPLICIT NONE
      INTEGER(C_INT16_T), VALUE :: port
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: html_response
      TYPE(C_PTR), INTENT(OUT) :: out_code
      TYPE(C_PTR), INTENT(OUT) :: out_state
      TYPE(C_PTR), INTENT(OUT) :: out_error
      TYPE(C_PTR), INTENT(OUT) :: out_error_desc
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_oauth2_localhost_intercept

    !> @brief Executes the http_response_init operation.
    !> @param res The res parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_response_init(res) BIND(C, NAME="http_response_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_response_init

    !> @brief Executes the http_response_free operation.
    !> @param res The res parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_response_free(res) BIND(C, NAME="http_response_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_response_free

    !> @brief Saves an HTTP response to a file.
    !> @param res The HTTP response object.
    !> @param path The path to the file.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_response_save_to_file(res, path) BIND(C, NAME="http_response_save_to_file") RESULT(cdd_ret_)
      IMPORT :: httpresponse, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httpresponse), INTENT(IN) :: res
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: path
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_response_save_to_file

    !> @brief Initialize parts container.
    !> @param[out] parts Container to init.
    !> @return 0 on success.
    !
    FUNCTION http_parts_init(parts) BIND(C, NAME="http_parts_init") RESULT(cdd_ret_)
      IMPORT :: httpparts, C_INT
      IMPLICIT NONE
      TYPE(httpparts), INTENT(OUT) :: parts
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_parts_init

    !> @brief Free parts container contents.
    !> @param[in] parts Container to clean.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_parts_free(parts) BIND(C, NAME="http_parts_free") RESULT(cdd_ret_)
      IMPORT :: httpparts, C_INT
      IMPLICIT NONE
      TYPE(httpparts), INTENT(IN) :: parts
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_parts_free

    !> @brief Add a part to the request.
    !
    !> @param[in,out] req The request object.
    !> @param[in] name Field name.
    !> @param[in] filename Optional filename (NULL for text fields).
    !> @param[in] content_type Optional MIME type (NULL autodetects/defaults).
    !> @param[in] data Pointer to payload.
    !> @param[in] data_len Length of payload.
    !> @return 0 on success, ENOMEM on failure.
    !
    FUNCTION http_request_add_part(req, name, filename, content_type, data_, data_len) BIND(C,  &
      &NAME="http_request_add_part") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_PTR, C_SIZE_T, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: name
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: filename
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: content_type
      TYPE(C_PTR), VALUE :: data_
      INTEGER(C_SIZE_T), VALUE :: data_len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_add_part

    !> @brief Add a header to the most recently added multipart part.
    !
    !> @param[in,out] req The request object.
    !> @param[in] key Header name.
    !> @param[in] value Header value.
    !> @return 0 on success, EINVAL if no part exists or inputs are invalid.
    !
    FUNCTION http_request_add_part_header_last(req, key, value_) BIND(C, NAME="http_request_add_part_header_last")  &
      &RESULT(cdd_ret_)
      IMPORT :: httprequest, C_CHAR, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: key
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: value_
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_add_part_header_last

    !> @brief Flatten parts into a single multipart/form-data body buffer.
    !
    ! Generates the boundary, headers, and payload for all parts, concatenates them
    ! into `req->body`, and sets the `Content-Type` header on `req`.
    ! Used by transport layers that lack native multipart support or for
    ! consistency.
    !
    !> @param[in,out] req The request structure.
    !> @return 0 on success, ENOMEM on allocation failure.
    !
    FUNCTION http_request_flatten_parts(req) BIND(C, NAME="http_request_flatten_parts") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(INOUT) :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_request_flatten_parts

    !> @brief Initialize the global wasm environment safely.
    !
    !> @return 0 on success.
    !
    FUNCTION http_wasm_global_init() BIND(C, NAME="http_wasm_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_global_init

    !> @brief Cleanup the global wasm environment.
    !> @return 0 on success.
    !
    FUNCTION http_wasm_global_cleanup() BIND(C, NAME="http_wasm_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_global_cleanup

    !> @brief Create a new Wasm-backed transport context.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return 0 on success, ENOMEM on allocation failure.
    !
    FUNCTION http_wasm_context_init(ctx) BIND(C, NAME="http_wasm_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_context_init

    !> @brief Free the transport context.
    !
    !> @param[in] ctx The context to free. Safe to pass NULL.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_wasm_context_free(ctx) BIND(C, NAME="http_wasm_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_context_free

    !> @brief Apply configuration settings to the context.
    !
    ! Maps `timeout_ms`, `connect_timeout_ms`, `read_timeout_ms`,
    ! `write_timeout_ms`, and `user_agent` onto Emscripten Fetch attributes.
    !
    !> @note Browser Fetch API Limitations: TLS peer/host verification
    ! (`verify_peer`, `verify_host`), protocol negotiation (`tls_version_mask`),
    ! and redirect handling (`follow_redirects`) are governed by the host browser's
    ! CORS and TLS stack and cannot be weakened from sandboxed WebAssembly.
    ! Explicit custom proxies (`proxy_url`), custom CA bundles (`ca_path`,
    ! `ca_buffer`), mTLS client certificates (`client_cert_path`,
    ! `client_key_path`), or SPKI certificate pinning (`cert_pin_sha256`) are not
    ! supported by browser Fetch and return `C_ABSTRACT_HTTP_ERR_NOTSUP`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration structure to apply.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, C_ABSTRACT_HTTP_ERR_NOTSUP if
    ! unsupported proxy/pinning options are requested, or C_ABSTRACT_HTTP_ERR_INVAL
    ! if inputs are invalid.
    !
    FUNCTION http_wasm_config_apply(ctx, config) BIND(C, NAME="http_wasm_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_config_apply

    !> @brief The send implementation for Emscripten fetch.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response
    ! object.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_wasm_send(ctx, req, res) BIND(C, NAME="http_wasm_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_send

    !> @brief Perform multiple HTTP requests concurrently via wasm fetch.
    !
    !> @param[in] ctx The context.
    !> @param[in] loop The event loop context.
    !> @param[in] multi The multi request definition.
    !> @param[out] futures Array of futures to populate.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_wasm_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_wasm_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wasm_send_multi

    !> @brief Initialize the global WinHTTP environment.
    ! Usually a no-op as WinHTTP initializes lazily, but kept for interface
    ! consistency.
    !
    !> @return 0 on success.
    !
    FUNCTION http_winhttp_global_init() BIND(C, NAME="http_winhttp_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_global_init

    !> @brief Cleanup global WinHTTP environment.
    ! No-op for WinHTTP.
    !> @return 0 on success.
    !
    FUNCTION http_winhttp_global_cleanup() BIND(C, NAME="http_winhttp_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_global_cleanup

    !> @brief Create a new WinHTTP-backed transport context.
    ! Opens a WinHTTP Session handle using `WinHttpOpen`.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return 0 on success, ENOMEM on allocation failure, or EIO on WinHTTP
    ! failure.
    !
    FUNCTION http_winhttp_context_init(ctx) BIND(C, NAME="http_winhttp_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_context_init

    !> @brief Free the transport context.
    ! Closes the internal WinHTTP Session handle.
    !
    !> @param[in] ctx The context to free. Satisfies NULL-safety.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_winhttp_context_free(ctx) BIND(C, NAME="http_winhttp_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_context_free

    !> @brief Apply configuration settings to the WinHTTP session.
    !
    ! Updates the session handle with:
    ! - Timeouts (Resolve, Connect, Send, Receive) via `WinHttpSetTimeouts`.
    ! - Proxy settings via `WinHttpSetOption`.
    ! - Caches Security flags (SSL verification) internally to be applied
    !   during `send` (since they apply to Request handles, not Session handles).
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration to apply.
    !> @return 0 on success, EIO if WinHTTP calls fail.
    !
    FUNCTION http_winhttp_config_apply(ctx, config) BIND(C, NAME="http_winhttp_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_config_apply

    !> @brief The send implementation for WinHTTP.
    ! Matches generic `http_send_fn` signature.
    !
    ! Workflow:
    ! 1. URL Cracking (Host/Port/Path separation).
    ! 2. UTF-8 to UTF-16 conversions for all strings.
    ! 3. Handle hierarchy creation: Session -> Connect -> Request.
    ! 4. Application of cached security flags (Ignore Cert Errors).
    ! 5. Header aggregation.
    ! 6. Synchronous transmission.
    ! 7. Response reading and transcoding (UTF-16 -> UTF-8).
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the allocated response object.
    !> @return 0 on success, error code (EIO/EINVAL/ENOMEM) on failure.
    !
    FUNCTION http_winhttp_send(ctx, req, res) BIND(C, NAME="http_winhttp_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_send

    !> @brief Asynchronous multi-send implementation for WinHTTP.
    ! Matches `http_send_multi_fn` signature.
    !
    ! Uses background threads to execute requests and posts completion events
    ! to the `ModalityEventLoop`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop The event loop context.
    !> @param[in] multi The multi request structure.
    !> @param[out] futures Array of futures.
    !> @return 0 on success.
    !
    FUNCTION http_winhttp_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_winhttp_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_winhttp_send_multi

    !> @brief Initialize the global WinInet environment.
    !
    ! WinInet is largely initialized per-call or implicitly by the DLL,
    ! but this hook allows for future global configuration if needed.
    !
    !> @return 0 on success.
    !
    FUNCTION http_wininet_global_init() BIND(C, NAME="http_wininet_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_global_init

    !> @brief Cleanup global WinInet environment.
    ! No-op for WinInet.
    !> @return 0 on success.
    !
    FUNCTION http_wininet_global_cleanup() BIND(C, NAME="http_wininet_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_global_cleanup

    !> @brief Create a new WinInet-backed transport context.
    ! Opens a root HINTERNET handle using `InternetOpen`.
    !
    !> @param[out] ctx Double pointer to receive the allocated context.
    !> @return 0 on success, ENOMEM on allocation failure, or EIO on WinInet
    ! failure.
    !
    FUNCTION http_wininet_context_init(ctx) BIND(C, NAME="http_wininet_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_context_init

    !> @brief Free the transport context.
    ! Closes the root HINTERNET handle.
    !
    !> @param[in] ctx The context to free. Satisfies NULL-safety.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_wininet_context_free(ctx) BIND(C, NAME="http_wininet_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_context_free

    !> @brief Apply configuration settings to the WinInet context.
    !
    ! - Sets timeouts via `InternetSetOption`.
    ! - Caches verification flags to be applied to specific requests.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] config The configuration to apply.
    !> @return 0 on success, EIO if WinInet calls fail.
    !
    FUNCTION http_wininet_config_apply(ctx, config) BIND(C, NAME="http_wininet_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_config_apply

    !> @brief The send implementation for WinInet.
    ! Matches generic `http_send_fn` signature.
    !
    ! Workflow:
    ! 1. Parse URL elements (Host, Path, Scheme).
    ! 2. `InternetConnect` into the target host.
    ! 3. `HttpOpenRequest` for the specific resource.
    ! 4. Apply headers and Send Request.
    ! 5. Read response stream.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the allocated response object.
    !> @return 0 on success, error code (EIO/EINVAL/ENOMEM) on failure.
    !
    FUNCTION http_wininet_send(ctx, req, res) BIND(C, NAME="http_wininet_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_send

    !> @brief Asynchronous multi-request dispatcher for WinInet.
    !
    ! Uses `INTERNET_FLAG_ASYNC` + `InternetSetStatusCallbackW` and posts
    ! completion notifications to `ModalityEventLoop`.
    !
    !> @param[in] ctx The transport context.
    !> @param[in,out] loop Optional event loop to wake on completion.
    !> @param[in] multi Batch of HTTP requests to execute concurrently.
    !> @param[out] futures Array of futures populated with responses/errors.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or error code on failure.
    !
    FUNCTION http_wininet_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_wininet_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_wininet_send_multi

    !> @brief Initialize a WebSocket request.
    !
    !> @param req The HTTP request to initialize.
    !> @param config Optional WebSocket configuration.
    !> @return 0 on success, or a negative error code.
    !
    FUNCTION c_abstract_http_ws_init(req, config) BIND(C, NAME="c_abstract_http_ws_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, c_abstract_http_ws_config, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      TYPE(c_abstract_http_ws_config), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_init

    !> @brief Run a blocking loop to read WebSocket frames for the given connection.
    !> @param client The HTTP client.
    !> @param req The connected request.
    !> @param on_msg The callback to trigger on messages.
    !> @param on_err The callback to trigger on errors.
    !> @param on_close The callback to trigger when the stream closes.
    !> @param user_data Opaque pointer to pass to the callbacks.
    !> @param exit_flag Optional volatile int pointer. Loop exits cleanly when
    ! *exit_flag != 0.
    !> @return 0 on clean exit, negative error code on failure.
    !
    FUNCTION c_abstract_http_ws_sync_read_loop(client, req, on_msg, on_err, on_close, user_data, exit_flag) BIND(C,  &
      &NAME="c_abstract_http_ws_sync_read_loop") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      TYPE(C_PTR), VALUE :: req
      TYPE(C_FUNPTR), VALUE :: on_msg
      TYPE(C_FUNPTR), VALUE :: on_err
      TYPE(C_FUNPTR), VALUE :: on_close
      TYPE(C_PTR), VALUE :: user_data
      TYPE(C_PTR), VALUE :: exit_flag
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_sync_read_loop

    !> @brief Perform an HTTP/1.1 `101 Switching Protocols` WebSocket handshake,
    ! detach persistent ownership of the upgraded non-blocking socket, and
    ! register full-duplex frame read/write handlers on `client->loop`.
    !> @param client The HTTP client with an initialized `ModalityEventLoop`.
    !> @param req The WebSocket request (`ws://` or `http://` URL).
    !> @param on_msg Callback invoked on incoming WebSocket messages/control frames.
    !> @param on_err Callback invoked on framing or socket errors.
    !> @param on_close Callback invoked when the WebSocket connection closes.
    !> @param user_data Opaque pointer passed to callbacks.
    !> @return C_ABSTRACT_HTTP_SUCCESS on upgrade and registration, or error enum.
    !
    FUNCTION c_abstract_http_ws_async_connect(client, req, on_msg, on_err, on_close, user_data) BIND(C,  &
      &NAME="c_abstract_http_ws_async_connect") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      TYPE(C_PTR), VALUE :: req
      TYPE(C_FUNPTR), VALUE :: on_msg
      TYPE(C_FUNPTR), VALUE :: on_err
      TYPE(C_FUNPTR), VALUE :: on_close
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_async_connect

    !> @brief Register a WebSocket connection with the underlying transport's
    ! asynchronous event loop (or thread pool fallback).
    !> @param client The HTTP client.
    !> @param req The connected request.
    !> @param on_msg The callback to trigger on messages.
    !> @param on_err The callback to trigger on errors.
    !> @param on_close The callback to trigger when the stream closes.
    !> @param user_data Opaque pointer to pass to the callbacks.
    !> @return 0 on success, negative error code on failure.
    !
    FUNCTION c_abstract_http_ws_async_register(client, req, on_msg, on_err, on_close, user_data) BIND(C,  &
      &NAME="c_abstract_http_ws_async_register") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: client
      TYPE(C_PTR), VALUE :: req
      TYPE(C_FUNPTR), VALUE :: on_msg
      TYPE(C_FUNPTR), VALUE :: on_err
      TYPE(C_FUNPTR), VALUE :: on_close
      TYPE(C_PTR), VALUE :: user_data
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_async_register

    !> @brief Queue a WebSocket frame to be sent asynchronously or from a thread
    ! pool.
    !
    !> @param req The HTTP request (acting as the connection handle).
    !> @param opcode The WebSocket opcode to send.
    !> @param payload The payload data to send.
    !> @param len The length of the payload data.
    !> @return 0 on success, or a negative error code.
    !
    FUNCTION c_abstract_http_ws_send_async(req, opcode, payload, len) BIND(C, NAME="c_abstract_http_ws_send_async")  &
      &RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT, C_CHAR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT), VALUE :: opcode
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: payload
      INTEGER(C_SIZE_T), VALUE :: len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_send_async

    !> @brief Send a WebSocket frame.
    !
    !> @param req The HTTP request (acting as the connection handle).
    !> @param opcode The WebSocket opcode to send.
    !> @param payload The payload data to send.
    !> @param len The length of the payload data.
    !> @return 0 on success, or a negative error code.
    !
    FUNCTION c_abstract_http_ws_send(req, opcode, payload, len) BIND(C, NAME="c_abstract_http_ws_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT, C_CHAR, C_SIZE_T
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT), VALUE :: opcode
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: payload
      INTEGER(C_SIZE_T), VALUE :: len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_send

    !> @brief Close a WebSocket connection.
    !
    !> @param req The HTTP request (acting as the connection handle).
    !> @param status_code The closure status code (e.g., 1000 for normal closure).
    !> @return 0 on success, or a negative error code.
    !
    FUNCTION c_abstract_http_ws_close(req, status_code) BIND(C, NAME="c_abstract_http_ws_close") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT32_T), VALUE :: status_code
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_close

    !> @brief Register or clear custom TLS hooks on a WebSocket request for `wss://`
    ! connections.
    !
    ! When @p hooks is NULL, `wss://` connections fall back to the library's
    ! default TLS provider (`c_abstract_http_get_default_tls_hooks`).
    !
    !> @param req The HTTP request acting as the WebSocket handle.
    !> @param hooks Optional TLS hook table to copy, or NULL to clear custom hooks.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or C_ABSTRACT_HTTP_ERR_INVAL /
    ! C_ABSTRACT_HTTP_ERR_NOMEM on failure.
    !
    FUNCTION c_abstract_http_ws_set_tls_hooks(req, hooks) BIND(C, NAME="c_abstract_http_ws_set_tls_hooks") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, httprawtlshooks, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      TYPE(httprawtlshooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_set_tls_hooks

    !> @brief Free WebSocket stream resources attached to an HTTP request.
    !
    !> @param req The HTTP request containing WebSocket context to free.
    !> @return C_ABSTRACT_HTTP_SUCCESS on success, or an error code.
    !
    FUNCTION c_abstract_http_ws_free(req) BIND(C, NAME="c_abstract_http_ws_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION c_abstract_http_ws_free

    !> @brief Initialize the global xquic API state.
    ! Reference-counted; safe to call multiple times.
    !> @return 0 on success, ENOMEM or native error code on failure.
    !
    FUNCTION http_xquic_global_init() BIND(C, NAME="http_xquic_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_global_init

    !> @brief Clean up the global xquic API state.
    ! Automatically cleans up when the reference count drops to 0.
    !> @return 0 on success.
    !
    FUNCTION http_xquic_global_cleanup() BIND(C, NAME="http_xquic_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_global_cleanup

    !> @brief Initialize a new xquic transport context.
    !
    !> @param[out] ctx Double pointer to receive the newly allocated context.
    !> @return 0 on success, ENOMEM on failure.
    !
    FUNCTION http_xquic_context_init(ctx) BIND(C, NAME="http_xquic_context_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_context_init

    !> @brief Free a xquic transport context and its associated resources.
    !
    !> @param[in] ctx The context to free.
    !> @return 0 on success, or an error code.
    !
    FUNCTION http_xquic_context_free(ctx) BIND(C, NAME="http_xquic_context_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_context_free

    !> @brief Apply configuration settings to a xquic transport context.
    !
    ! Maps abstract settings to xquic engine settings.
    !
    !> @param[in,out] ctx The target context.
    !> @param[in] config The configuration settings.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_xquic_config_apply(ctx, config) BIND(C, NAME="http_xquic_config_apply") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpconfig, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httpconfig), INTENT(IN) :: config
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_config_apply

    !> @brief Synchronous single request dispatcher for xquic.
    ! Matches http_send_fn signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] req The request to send.
    !> @param[out] res Double pointer to receive the newly allocated response.
    !> @return 0 on success, or a mapped error code on failure.
    !
    FUNCTION http_xquic_send(ctx, req, res) BIND(C, NAME="http_xquic_send") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httprequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_send

    !> @brief Asynchronous multi-send implementation for xquic.
    ! Matches http_send_multi_fn signature.
    !
    !> @param[in] ctx The transport context.
    !> @param[in] loop The event loop context.
    !> @param[in] multi The multi request structure.
    !> @param[out] futures Array of futures.
    !> @return 0 on success.
    !
    FUNCTION http_xquic_send_multi(ctx, loop, multi, futures) BIND(C, NAME="http_xquic_send_multi") RESULT(cdd_ret_)
      IMPORT :: C_PTR, httpmultirequest, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: ctx
      TYPE(C_PTR), VALUE :: loop
      TYPE(httpmultirequest), INTENT(IN) :: multi
      TYPE(C_PTR), INTENT(OUT) :: futures
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION http_xquic_send_multi

    !> @brief Write a debug log message.
    !> @param[in] fmt Formatting string.
    !> @param[in] ... Variadic arguments.
    !
    ! Skipped variadic C function (not ISO_C_BINDING compatible): c_abstract_http_log_debug
    !> @brief Register external process hooks.
    !> @param[in] hooks The hooks structure (copied internally).
    !> @return 0 on success.
    !
    FUNCTION abstract_http_process_set_hooks(hooks) BIND(C, NAME="abstract_http_process_set_hooks") RESULT(cdd_ret_)
      IMPORT :: abstracthttpprocesshooks, C_INT
      IMPLICIT NONE
      TYPE(abstracthttpprocesshooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_process_set_hooks

    !> @brief Initialize an IPC pipe pair.
    !> @param[out] ipc_pipe The pipe structure to populate.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION abstract_http_ipc_pipe_init(ipc_pipe) BIND(C, NAME="abstract_http_ipc_pipe_init") RESULT(cdd_ret_)
      IMPORT :: abstracthttpipcpipe, C_INT
      IMPLICIT NONE
      TYPE(abstracthttpipcpipe), INTENT(OUT) :: ipc_pipe
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_pipe_init

    !> @brief Close an IPC pipe.
    !> @param[in] ipc_pipe The pipe to close.
    !> @return 0 on success, or an error code.
    !
    FUNCTION abstract_http_ipc_pipe_free(ipc_pipe) BIND(C, NAME="abstract_http_ipc_pipe_free") RESULT(cdd_ret_)
      IMPORT :: abstracthttpipcpipe, C_INT
      IMPLICIT NONE
      TYPE(abstracthttpipcpipe), INTENT(IN) :: ipc_pipe
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_pipe_free

    !> @brief Spawn a child process.
    ! The child process will execute the current executable but with a specific
    ! internal argument flag that routes it to the worker main.
    !
    !> @param[out] proc Pointer to receive the allocated process handle.
    !> @param[out] parent_to_child The pipe for parent -> child communication.
    !> @param[out] child_to_parent The pipe for child -> parent communication.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION abstract_http_process_spawn(proc, parent_to_child, child_to_parent) BIND(C,  &
      &NAME="abstract_http_process_spawn") RESULT(cdd_ret_)
      IMPORT :: C_PTR, abstracthttpipcpipe, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: proc
      TYPE(abstracthttpipcpipe), INTENT(OUT) :: parent_to_child
      TYPE(abstracthttpipcpipe), INTENT(OUT) :: child_to_parent
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_process_spawn

    !> @brief Wait for a process to exit and free its handle.
    !> @param[in] proc The process handle.
    !> @param[out] exit_code Pointer to store the exit code (optional).
    !> @return 0 on success.
    !
    FUNCTION abstract_http_process_wait_and_free(proc, exit_code) BIND(C, NAME="abstract_http_process_wait_and_free")  &
      &RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT32_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: proc
      INTEGER(C_INT32_T), INTENT(OUT) :: exit_code
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_process_wait_and_free

    !> @brief Serialize an HttpRequest into a buffer.
    !> @param[in] req The request.
    !> @param[out] out_buf Pointer to receive the allocated buffer.
    !> @param[out] out_len Length of the buffer.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_ipc_serialize_request(req, out_buf, out_len) BIND(C,  &
      &NAME="abstract_http_ipc_serialize_request") RESULT(cdd_ret_)
      IMPORT :: httprequest, C_PTR, C_SIZE_T, C_INT
      IMPLICIT NONE
      TYPE(httprequest), INTENT(IN) :: req
      TYPE(C_PTR), INTENT(OUT) :: out_buf
      INTEGER(C_SIZE_T), INTENT(OUT) :: out_len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_serialize_request

    !> @brief Deserialize a buffer into an HttpRequest.
    !> @param[in] buf The buffer.
    !> @param[in] len Length of the buffer.
    !> @param[out] req The request to populate.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_ipc_deserialize_request(buf, len, req) BIND(C,  &
      &NAME="abstract_http_ipc_deserialize_request") RESULT(cdd_ret_)
      IMPORT :: C_CHAR, C_SIZE_T, httprequest, C_INT
      IMPLICIT NONE
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: buf
      INTEGER(C_SIZE_T), VALUE :: len
      TYPE(httprequest), INTENT(OUT) :: req
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_deserialize_request

    !> @brief Serialize an HttpResponse into a buffer.
    !> @param[in] res The response.
    !> @param[out] out_buf Pointer to receive the allocated buffer.
    !> @param[out] out_len Length of the buffer.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_ipc_serialize_response(res, out_buf, out_len) BIND(C,  &
      &NAME="abstract_http_ipc_serialize_response") RESULT(cdd_ret_)
      IMPORT :: httpresponse, C_PTR, C_SIZE_T, C_INT
      IMPLICIT NONE
      TYPE(httpresponse), INTENT(IN) :: res
      TYPE(C_PTR), INTENT(OUT) :: out_buf
      INTEGER(C_SIZE_T), INTENT(OUT) :: out_len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_serialize_response

    !> @brief Deserialize a buffer into an HttpResponse.
    !> @param[in] buf The buffer.
    !> @param[in] len Length of the buffer.
    !> @param[out] res The response to populate.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_ipc_deserialize_response(buf, len, res) BIND(C,  &
      &NAME="abstract_http_ipc_deserialize_response") RESULT(cdd_ret_)
      IMPORT :: C_CHAR, C_SIZE_T, httpresponse, C_INT
      IMPLICIT NONE
      CHARACTER(KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: buf
      INTEGER(C_SIZE_T), VALUE :: len
      TYPE(httpresponse), INTENT(OUT) :: res
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_deserialize_response

    !> @brief Write data to an IPC pipe endpoint securely.
    !> @param handle The handle parameter.
    !> @param data The data parameter.
    !> @param len The len parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION abstract_http_ipc_write(handle, data_, len) BIND(C, NAME="abstract_http_ipc_write") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: handle
      TYPE(C_PTR), VALUE :: data_
      INTEGER(C_SIZE_T), VALUE :: len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_write

    !> @brief Read data from an IPC pipe endpoint securely.
    !> @param handle The handle parameter.
    !> @param data The data parameter.
    !> @param len The len parameter.
    !> @return 0 on success, or an error code.
    !
    FUNCTION abstract_http_ipc_read(handle, data_, len) BIND(C, NAME="abstract_http_ipc_read") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: handle
      TYPE(C_PTR), VALUE :: data_
      INTEGER(C_SIZE_T), VALUE :: len
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_ipc_read

    !> @brief Initialize a mutex.
    !> @param[out] mutex Pointer to receive the allocated mutex.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION abstract_http_mutex_init(mutex) BIND(C, NAME="abstract_http_mutex_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: mutex
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_mutex_init

    !> @brief Lock a mutex.
    !> @param[in] mutex The mutex.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_mutex_lock(mutex) BIND(C, NAME="abstract_http_mutex_lock") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: mutex
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_mutex_lock

    !> @brief Unlock a mutex.
    !> @param[in] mutex The mutex.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_mutex_unlock(mutex) BIND(C, NAME="abstract_http_mutex_unlock") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: mutex
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_mutex_unlock

    !> @brief Free a mutex wrapper.
    !> @param mutex The mutex.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_mutex_free(mutex) BIND(C, NAME="abstract_http_mutex_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: mutex
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_mutex_free

    !> @brief Initialize a condition variable.
    !> @param[out] cond Pointer to receive the allocated condition variable.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_cond_init(cond) BIND(C, NAME="abstract_http_cond_init") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: cond
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_cond_init

    !> @brief Wait on a condition variable.
    !> @param[in] cond The condition variable.
    !> @param[in] mutex The associated mutex.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_cond_wait(cond, mutex) BIND(C, NAME="abstract_http_cond_wait") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: cond
      TYPE(C_PTR), VALUE :: mutex
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_cond_wait

    !> @brief Signal a condition variable (wake one).
    !> @param[in] cond The condition variable.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_cond_signal(cond) BIND(C, NAME="abstract_http_cond_signal") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: cond
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_cond_signal

    !> @brief Broadcast a condition variable (wake all).
    !> @param[in] cond The condition variable.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_cond_broadcast(cond) BIND(C, NAME="abstract_http_cond_broadcast") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: cond
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_cond_broadcast

    !> @brief Free a condition variable.
    !> @param cond The condition variable.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_cond_free(cond) BIND(C, NAME="abstract_http_cond_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: cond
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_cond_free

    !> @brief Initialize a thread pool.
    !> @param[out] pool Pointer to receive the allocated thread pool.
    !> @param[in] num_threads Number of worker threads to spawn.
    !> @return 0 on success, ENOMEM or other error on failure.
    !
    FUNCTION abstract_http_thread_pool_init(pool, num_threads) BIND(C, NAME="abstract_http_thread_pool_init") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, C_SIZE_T, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: pool
      INTEGER(C_SIZE_T), VALUE :: num_threads
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_thread_pool_init

    !> @brief Initialize a thread pool that delegates tasks to an external worker
    ! pool.
    !> @param[out] pool Pointer to receive the allocated thread pool proxy.
    !> @param[in] hooks External thread pool hooks.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION abstract_http_thread_pool_init_external(pool, hooks) BIND(C,  &
      &NAME="abstract_http_thread_pool_init_external") RESULT(cdd_ret_)
      IMPORT :: C_PTR, abstracthttpthreadpoolhooks, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), INTENT(OUT) :: pool
      TYPE(abstracthttpthreadpoolhooks), INTENT(IN) :: hooks
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_thread_pool_init_external

    !> @brief Push a task to the thread pool queue.
    !> @param[in] pool The thread pool.
    !> @param[in] cb The function to execute.
    !> @param[in] arg The argument to pass to the function.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION abstract_http_thread_pool_push(pool, cb, arg) BIND(C, NAME="abstract_http_thread_pool_push") RESULT( &
      &cdd_ret_)
      IMPORT :: C_PTR, C_FUNPTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: pool
      TYPE(C_FUNPTR), VALUE :: cb
      TYPE(C_PTR), VALUE :: arg
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_thread_pool_push

    !> @brief Destroy the thread pool, waiting for tasks to complete.
    !> @param[in] pool The thread pool.
    !> @return 0 on success.
    !
    FUNCTION abstract_http_thread_pool_free(pool) BIND(C, NAME="abstract_http_thread_pool_free") RESULT(cdd_ret_)
      IMPORT :: C_PTR, C_INT
      IMPLICIT NONE
      TYPE(C_PTR), VALUE :: pool
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION abstract_http_thread_pool_free

    !> @brief Initialize the global transport layer.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION transport_global_init() BIND(C, NAME="transport_global_init") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION transport_global_init

    !> @brief Clean up the global transport layer.
    !> @return 0 on success.
    !
    FUNCTION transport_global_cleanup() BIND(C, NAME="transport_global_cleanup") RESULT(cdd_ret_)
      IMPORT :: C_INT
      IMPLICIT NONE
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION transport_global_cleanup

    !> @brief Initialize a new HTTP client transport.
    !> @param[in,out] client The HTTP client to initialize.
    !> @return 0 on success, error code on failure.
    !
    FUNCTION transport_factory_init_client(client) BIND(C, NAME="transport_factory_init_client") RESULT(cdd_ret_)
      IMPORT :: httpclient, C_INT
      IMPLICIT NONE
      TYPE(httpclient), INTENT(INOUT) :: client
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION transport_factory_init_client

    !> @brief Clean up an HTTP client transport.
    !> @param[in] client The HTTP client to clean up.
    !> @return 0 on success.
    !
    FUNCTION transport_factory_cleanup_client(client) BIND(C, NAME="transport_factory_cleanup_client") RESULT(cdd_ret_)
      IMPORT :: httpclient, C_INT
      IMPLICIT NONE
      TYPE(httpclient), INTENT(IN) :: client
      INTEGER(C_INT) :: cdd_ret_
    END FUNCTION transport_factory_cleanup_client

  END INTERFACE

CONTAINS

  !> @brief Fortran CHARACTER convenience wrapper for abstract_http_actor_spawn.
  !> @param[in] bus Argument 1.
  !> @param[in] name Argument 2.
  !> @param[in] handler Argument 3.
  !> @param[in] state Argument 4.
  !> @param[out] actor Argument 5.
  !> @return Wrapped C call result.
  FUNCTION abstract_http_actor_spawn_str(bus, name, handler, state, actor) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: bus
    CHARACTER(LEN=*), INTENT(IN) :: name
    TYPE(C_FUNPTR), VALUE :: handler
    TYPE(C_PTR), VALUE :: state
    TYPE(C_PTR), INTENT(OUT) :: actor
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = abstract_http_actor_spawn(bus, TRIM(name) // C_NULL_CHAR, handler, state, actor)
  END FUNCTION abstract_http_actor_spawn_str
  !> @brief Fortran CHARACTER convenience wrapper for c_abstract_http_strdup.
  !> @param[in] s Argument 1.
  !> @param[out] out_s Argument 2.
  !> @return Wrapped C call result.
  FUNCTION c_abstract_http_strdup_2_str(s, out_s) RESULT(cdd_ret_)
    CHARACTER(LEN=*), INTENT(IN) :: s
    TYPE(C_PTR), INTENT(OUT) :: out_s
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = c_abstract_http_strdup_2(TRIM(s) // C_NULL_CHAR, out_s)
  END FUNCTION c_abstract_http_strdup_2_str
  !> @brief Fortran CHARACTER convenience wrapper for http_headers_add.
  !> @param[in] headers Argument 1.
  !> @param[in] key Argument 2.
  !> @param[in] value_ Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_headers_add_str(headers, key, value_) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: headers
    CHARACTER(LEN=*), INTENT(IN) :: key
    CHARACTER(LEN=*), INTENT(IN) :: value_
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_headers_add(headers, TRIM(key) // C_NULL_CHAR, TRIM(value_) // C_NULL_CHAR)
  END FUNCTION http_headers_add_str
  !> @brief Fortran CHARACTER convenience wrapper for http_headers_get.
  !> @param[in] headers Argument 1.
  !> @param[in] key Argument 2.
  !> @param[in] out_ Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_headers_get_str(headers, key, out_) RESULT(cdd_ret_)
    TYPE(httpheaders), INTENT(IN) :: headers
    CHARACTER(LEN=*), INTENT(IN) :: key
    TYPE(C_PTR), VALUE :: out_
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_headers_get(headers, TRIM(key) // C_NULL_CHAR, out_)
  END FUNCTION http_headers_get_str
  !> @brief Fortran CHARACTER convenience wrapper for http_headers_remove.
  !> @param[in] headers Argument 1.
  !> @param[in] key Argument 2.
  !> @return Wrapped C call result.
  FUNCTION http_headers_remove_str(headers, key) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: headers
    CHARACTER(LEN=*), INTENT(IN) :: key
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_headers_remove(headers, TRIM(key) // C_NULL_CHAR)
  END FUNCTION http_headers_remove_str
  !> @brief Fortran CHARACTER convenience wrapper for http_cookie_jar_set.
  !> @param[in] jar Argument 1.
  !> @param[in] name Argument 2.
  !> @param[in] value_ Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_cookie_jar_set_str(jar, name, value_) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: jar
    CHARACTER(LEN=*), INTENT(IN) :: name
    CHARACTER(LEN=*), INTENT(IN) :: value_
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_cookie_jar_set(jar, TRIM(name) // C_NULL_CHAR, TRIM(value_) // C_NULL_CHAR)
  END FUNCTION http_cookie_jar_set_str
  !> @brief Fortran CHARACTER convenience wrapper for http_cookie_jar_get.
  !> @param[in] jar Argument 1.
  !> @param[in] name Argument 2.
  !> @param[in] out_ Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_cookie_jar_get_str(jar, name, out_) RESULT(cdd_ret_)
    TYPE(httpcookiejar), INTENT(IN) :: jar
    CHARACTER(LEN=*), INTENT(IN) :: name
    TYPE(C_PTR), VALUE :: out_
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_cookie_jar_get(jar, TRIM(name) // C_NULL_CHAR, out_)
  END FUNCTION http_cookie_jar_get_str
  !> @brief Fortran CHARACTER convenience wrapper for http_cookie_jar_parse_set_cookie.
  !> @param[inout] jar Argument 1.
  !> @param[in] set_cookie_header Argument 2.
  !> @return Wrapped C call result.
  FUNCTION http_cookie_jar_parse_set_cookie_str(jar, set_cookie_header) RESULT(cdd_ret_)
    TYPE(httpcookiejar), INTENT(INOUT) :: jar
    CHARACTER(LEN=*), INTENT(IN) :: set_cookie_header
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_cookie_jar_parse_set_cookie(jar, TRIM(set_cookie_header) // C_NULL_CHAR)
  END FUNCTION http_cookie_jar_parse_set_cookie_str
  !> @brief Fortran CHARACTER convenience wrapper for http_cookie_jar_build_header.
  !> @param[in] jar Argument 1.
  !> @param[in] request_url Argument 2.
  !> @param[out] out_cookie_header Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_cookie_jar_build_header_str(jar, request_url, out_cookie_header) RESULT(cdd_ret_)
    TYPE(httpcookiejar), INTENT(IN) :: jar
    CHARACTER(LEN=*), INTENT(IN) :: request_url
    TYPE(C_PTR), INTENT(OUT) :: out_cookie_header
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_cookie_jar_build_header(jar, TRIM(request_url) // C_NULL_CHAR, out_cookie_header)
  END FUNCTION http_cookie_jar_build_header_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_set_auth_bearer.
  !> @param[in] req Argument 1.
  !> @param[in] token Argument 2.
  !> @return Wrapped C call result.
  FUNCTION http_request_set_auth_bearer_str(req, token) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: req
    CHARACTER(LEN=*), INTENT(IN) :: token
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_set_auth_bearer(req, TRIM(token) // C_NULL_CHAR)
  END FUNCTION http_request_set_auth_bearer_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_set_auth_basic.
  !> @param[inout] req Argument 1.
  !> @param[in] token Argument 2.
  !> @return Wrapped C call result.
  FUNCTION http_request_set_auth_basic_str(req, token) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_set_auth_basic(req, TRIM(token) // C_NULL_CHAR)
  END FUNCTION http_request_set_auth_basic_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_set_auth_basic_userpwd.
  !> @param[inout] req Argument 1.
  !> @param[in] username Argument 2.
  !> @param[in] password Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_request_set_auth_basic_userpwd_str(req, username, password) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: username
    CHARACTER(LEN=*), INTENT(IN) :: password
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_set_auth_basic_userpwd(req, TRIM(username) // C_NULL_CHAR, TRIM(password) // C_NULL_CHAR)
  END FUNCTION http_request_set_auth_basic_userpwd_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_password_grant.
  !> @param[inout] req Argument 1.
  !> @param[in] token_endpoint_url Argument 2.
  !> @param[in] username Argument 3.
  !> @param[in] password Argument 4.
  !> @param[in] client_id Argument 5.
  !> @param[in] client_secret Argument 6.
  !> @param[in] scope Argument 7.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_password_grant_str(req, token_endpoint_url, username, password, client_id,  &
      &client_secret, scope) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: username
    CHARACTER(LEN=*), INTENT(IN) :: password
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: client_secret
    CHARACTER(LEN=*), INTENT(IN) :: scope
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_password_grant(req, TRIM(token_endpoint_url) // C_NULL_CHAR, TRIM(username)  &
      &// C_NULL_CHAR, TRIM(password) // C_NULL_CHAR, TRIM(client_id) // C_NULL_CHAR, TRIM(client_secret) //  &
      &C_NULL_CHAR, TRIM(scope) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_password_grant_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_refresh_token_grant.
  !> @param[inout] req Argument 1.
  !> @param[in] token_endpoint_url Argument 2.
  !> @param[in] refresh_token Argument 3.
  !> @param[in] client_id Argument 4.
  !> @param[in] client_secret Argument 5.
  !> @param[in] scope Argument 6.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_refresh_token_grant_str(req, token_endpoint_url, refresh_token, client_id,  &
      &client_secret, scope) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: refresh_token
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: client_secret
    CHARACTER(LEN=*), INTENT(IN) :: scope
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_refresh_token_grant(req, TRIM(token_endpoint_url) // C_NULL_CHAR, TRIM( &
      &refresh_token) // C_NULL_CHAR, TRIM(client_id) // C_NULL_CHAR, TRIM(client_secret) // C_NULL_CHAR, TRIM(scope)  &
      &// C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_refresh_token_grant_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_authorization_code_grant.
  !> @param[inout] req Argument 1.
  !> @param[in] token_endpoint_url Argument 2.
  !> @param[in] code Argument 3.
  !> @param[in] redirect_uri Argument 4.
  !> @param[in] client_id Argument 5.
  !> @param[in] client_secret Argument 6.
  !> @param[in] code_verifier Argument 7.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_authorization_code_grant_str(req, token_endpoint_url, code, redirect_uri,  &
      &client_id, client_secret, code_verifier) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: code
    CHARACTER(LEN=*), INTENT(IN) :: redirect_uri
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: client_secret
    CHARACTER(LEN=*), INTENT(IN) :: code_verifier
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_authorization_code_grant(req, TRIM(token_endpoint_url) // C_NULL_CHAR, TRIM( &
      &code) // C_NULL_CHAR, TRIM(redirect_uri) // C_NULL_CHAR, TRIM(client_id) // C_NULL_CHAR, TRIM(client_secret)  &
      &// C_NULL_CHAR, TRIM(code_verifier) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_authorization_code_grant_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_client_credentials_grant.
  !> @param[inout] req Argument 1.
  !> @param[in] token_endpoint_url Argument 2.
  !> @param[in] client_id Argument 3.
  !> @param[in] client_secret Argument 4.
  !> @param[in] scope Argument 5.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_client_credentials_grant_str(req, token_endpoint_url, client_id, client_secret,  &
      &scope) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: client_secret
    CHARACTER(LEN=*), INTENT(IN) :: scope
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_client_credentials_grant(req, TRIM(token_endpoint_url) // C_NULL_CHAR, TRIM( &
      &client_id) // C_NULL_CHAR, TRIM(client_secret) // C_NULL_CHAR, TRIM(scope) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_client_credentials_grant_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_jwt_bearer_grant.
  !> @param[inout] req Argument 1.
  !> @param[in] token_endpoint_url Argument 2.
  !> @param[in] assertion Argument 3.
  !> @param[in] scope Argument 4.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_jwt_bearer_grant_str(req, token_endpoint_url, assertion, scope) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: assertion
    CHARACTER(LEN=*), INTENT(IN) :: scope
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_jwt_bearer_grant(req, TRIM(token_endpoint_url) // C_NULL_CHAR, TRIM( &
      &assertion) // C_NULL_CHAR, TRIM(scope) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_jwt_bearer_grant_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_device_authorization_request.
  !> @param[inout] req Argument 1.
  !> @param[in] device_endpoint_url Argument 2.
  !> @param[in] client_id Argument 3.
  !> @param[in] scope Argument 4.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_device_authorization_request_str(req, device_endpoint_url, client_id, scope)  &
      &RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: device_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: scope
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_device_authorization_request(req, TRIM(device_endpoint_url) // C_NULL_CHAR,  &
      &TRIM(client_id) // C_NULL_CHAR, TRIM(scope) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_device_authorization_request_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_device_access_token_request.
  !> @param[inout] req Argument 1.
  !> @param[in] token_endpoint_url Argument 2.
  !> @param[in] client_id Argument 3.
  !> @param[in] device_code Argument 4.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_device_access_token_request_str(req, token_endpoint_url, client_id, device_code)  &
      &RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: token_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: device_code
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_device_access_token_request(req, TRIM(token_endpoint_url) // C_NULL_CHAR,  &
      &TRIM(client_id) // C_NULL_CHAR, TRIM(device_code) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_device_access_token_request_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_token_revocation.
  !> @param[inout] req Argument 1.
  !> @param[in] revocation_endpoint_url Argument 2.
  !> @param[in] token Argument 3.
  !> @param[in] token_type_hint Argument 4.
  !> @param[in] client_id Argument 5.
  !> @param[in] client_secret Argument 6.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_token_revocation_str(req, revocation_endpoint_url, token, token_type_hint,  &
      &client_id, client_secret) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: revocation_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: token
    CHARACTER(LEN=*), INTENT(IN) :: token_type_hint
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: client_secret
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_token_revocation(req, TRIM(revocation_endpoint_url) // C_NULL_CHAR, TRIM( &
      &token) // C_NULL_CHAR, TRIM(token_type_hint) // C_NULL_CHAR, TRIM(client_id) // C_NULL_CHAR, TRIM( &
      &client_secret) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_token_revocation_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_init_oauth2_token_introspection.
  !> @param[inout] req Argument 1.
  !> @param[in] introspection_endpoint_url Argument 2.
  !> @param[in] token Argument 3.
  !> @param[in] token_type_hint Argument 4.
  !> @param[in] client_id Argument 5.
  !> @param[in] client_secret Argument 6.
  !> @return Wrapped C call result.
  FUNCTION http_request_init_oauth2_token_introspection_str(req, introspection_endpoint_url, token, token_type_hint,  &
      &client_id, client_secret) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: introspection_endpoint_url
    CHARACTER(LEN=*), INTENT(IN) :: token
    CHARACTER(LEN=*), INTENT(IN) :: token_type_hint
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: client_secret
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_init_oauth2_token_introspection(req, TRIM(introspection_endpoint_url) // C_NULL_CHAR,  &
      &TRIM(token) // C_NULL_CHAR, TRIM(token_type_hint) // C_NULL_CHAR, TRIM(client_id) // C_NULL_CHAR, TRIM( &
      &client_secret) // C_NULL_CHAR)
  END FUNCTION http_request_init_oauth2_token_introspection_str
  !> @brief Fortran CHARACTER convenience wrapper for http_oauth2_build_authorization_url.
  !> @param[in] auth_endpoint Argument 1.
  !> @param[in] client_id Argument 2.
  !> @param[in] response_type Argument 3.
  !> @param[in] redirect_uri Argument 4.
  !> @param[in] scope Argument 5.
  !> @param[in] state Argument 6.
  !> @param[in] code_challenge Argument 7.
  !> @param[in] code_challenge_method Argument 8.
  !> @param[out] out_url Argument 9.
  !> @return Wrapped C call result.
  FUNCTION http_oauth2_build_authorization_url_str(auth_endpoint, client_id, response_type, redirect_uri, scope,  &
      &state, code_challenge, code_challenge_method, out_url) RESULT(cdd_ret_)
    CHARACTER(LEN=*), INTENT(IN) :: auth_endpoint
    CHARACTER(LEN=*), INTENT(IN) :: client_id
    CHARACTER(LEN=*), INTENT(IN) :: response_type
    CHARACTER(LEN=*), INTENT(IN) :: redirect_uri
    CHARACTER(LEN=*), INTENT(IN) :: scope
    CHARACTER(LEN=*), INTENT(IN) :: state
    CHARACTER(LEN=*), INTENT(IN) :: code_challenge
    CHARACTER(LEN=*), INTENT(IN) :: code_challenge_method
    TYPE(C_PTR), INTENT(OUT) :: out_url
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_oauth2_build_authorization_url(TRIM(auth_endpoint) // C_NULL_CHAR, TRIM(client_id) // C_NULL_CHAR, &
      &TRIM(response_type) // C_NULL_CHAR, TRIM(redirect_uri) // C_NULL_CHAR, TRIM(scope) // C_NULL_CHAR, TRIM(state)  &
      &// C_NULL_CHAR, TRIM(code_challenge) // C_NULL_CHAR, TRIM(code_challenge_method) // C_NULL_CHAR, out_url)
  END FUNCTION http_oauth2_build_authorization_url_str
  !> @brief Fortran CHARACTER convenience wrapper for http_oauth2_localhost_intercept.
  !> @param[in] port Argument 1.
  !> @param[in] html_response Argument 2.
  !> @param[out] out_code Argument 3.
  !> @param[out] out_state Argument 4.
  !> @param[out] out_error Argument 5.
  !> @param[out] out_error_desc Argument 6.
  !> @return Wrapped C call result.
  FUNCTION http_oauth2_localhost_intercept_str(port, html_response, out_code, out_state, out_error, out_error_desc)  &
      &RESULT(cdd_ret_)
    INTEGER(C_INT16_T), VALUE :: port
    CHARACTER(LEN=*), INTENT(IN) :: html_response
    TYPE(C_PTR), INTENT(OUT) :: out_code
    TYPE(C_PTR), INTENT(OUT) :: out_state
    TYPE(C_PTR), INTENT(OUT) :: out_error
    TYPE(C_PTR), INTENT(OUT) :: out_error_desc
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_oauth2_localhost_intercept(port, TRIM(html_response) // C_NULL_CHAR, out_code, out_state,  &
      &out_error, out_error_desc)
  END FUNCTION http_oauth2_localhost_intercept_str
  !> @brief Fortran CHARACTER convenience wrapper for http_response_save_to_file.
  !> @param[in] res Argument 1.
  !> @param[in] path Argument 2.
  !> @return Wrapped C call result.
  FUNCTION http_response_save_to_file_str(res, path) RESULT(cdd_ret_)
    TYPE(httpresponse), INTENT(IN) :: res
    CHARACTER(LEN=*), INTENT(IN) :: path
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_response_save_to_file(res, TRIM(path) // C_NULL_CHAR)
  END FUNCTION http_response_save_to_file_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_add_part.
  !> @param[inout] req Argument 1.
  !> @param[in] name Argument 2.
  !> @param[in] filename Argument 3.
  !> @param[in] content_type Argument 4.
  !> @param[in] data_ Argument 5.
  !> @param[in] data_len Argument 6.
  !> @return Wrapped C call result.
  FUNCTION http_request_add_part_str(req, name, filename, content_type, data_, data_len) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: name
    CHARACTER(LEN=*), INTENT(IN) :: filename
    CHARACTER(LEN=*), INTENT(IN) :: content_type
    TYPE(C_PTR), VALUE :: data_
    INTEGER(C_SIZE_T), VALUE :: data_len
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_add_part(req, TRIM(name) // C_NULL_CHAR, TRIM(filename) // C_NULL_CHAR, TRIM( &
      &content_type) // C_NULL_CHAR, data_, data_len)
  END FUNCTION http_request_add_part_str
  !> @brief Fortran CHARACTER convenience wrapper for http_request_add_part_header_last.
  !> @param[inout] req Argument 1.
  !> @param[in] key Argument 2.
  !> @param[in] value_ Argument 3.
  !> @return Wrapped C call result.
  FUNCTION http_request_add_part_header_last_str(req, key, value_) RESULT(cdd_ret_)
    TYPE(httprequest), INTENT(INOUT) :: req
    CHARACTER(LEN=*), INTENT(IN) :: key
    CHARACTER(LEN=*), INTENT(IN) :: value_
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = http_request_add_part_header_last(req, TRIM(key) // C_NULL_CHAR, TRIM(value_) // C_NULL_CHAR)
  END FUNCTION http_request_add_part_header_last_str
  !> @brief Fortran CHARACTER convenience wrapper for c_abstract_http_ws_send_async.
  !> @param[in] req Argument 1.
  !> @param[in] opcode Argument 2.
  !> @param[in] payload Argument 3.
  !> @param[in] len Argument 4.
  !> @return Wrapped C call result.
  FUNCTION c_abstract_http_ws_send_async_str(req, opcode, payload, len) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: req
    INTEGER(C_INT), VALUE :: opcode
    CHARACTER(LEN=*), INTENT(IN) :: payload
    INTEGER(C_SIZE_T), VALUE :: len
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = c_abstract_http_ws_send_async(req, opcode, TRIM(payload) // C_NULL_CHAR, len)
  END FUNCTION c_abstract_http_ws_send_async_str
  !> @brief Fortran CHARACTER convenience wrapper for c_abstract_http_ws_send.
  !> @param[in] req Argument 1.
  !> @param[in] opcode Argument 2.
  !> @param[in] payload Argument 3.
  !> @param[in] len Argument 4.
  !> @return Wrapped C call result.
  FUNCTION c_abstract_http_ws_send_str(req, opcode, payload, len) RESULT(cdd_ret_)
    TYPE(C_PTR), VALUE :: req
    INTEGER(C_INT), VALUE :: opcode
    CHARACTER(LEN=*), INTENT(IN) :: payload
    INTEGER(C_SIZE_T), VALUE :: len
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = c_abstract_http_ws_send(req, opcode, TRIM(payload) // C_NULL_CHAR, len)
  END FUNCTION c_abstract_http_ws_send_str
  !> @brief Fortran CHARACTER convenience wrapper for abstract_http_ipc_deserialize_request.
  !> @param[in] buf Argument 1.
  !> @param[in] len Argument 2.
  !> @param[out] req Argument 3.
  !> @return Wrapped C call result.
  FUNCTION abstract_http_ipc_deserialize_request_str(buf, len, req) RESULT(cdd_ret_)
    CHARACTER(LEN=*), INTENT(IN) :: buf
    INTEGER(C_SIZE_T), VALUE :: len
    TYPE(httprequest), INTENT(OUT) :: req
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = abstract_http_ipc_deserialize_request(TRIM(buf) // C_NULL_CHAR, len, req)
  END FUNCTION abstract_http_ipc_deserialize_request_str
  !> @brief Fortran CHARACTER convenience wrapper for abstract_http_ipc_deserialize_response.
  !> @param[in] buf Argument 1.
  !> @param[in] len Argument 2.
  !> @param[out] res Argument 3.
  !> @return Wrapped C call result.
  FUNCTION abstract_http_ipc_deserialize_response_str(buf, len, res) RESULT(cdd_ret_)
    CHARACTER(LEN=*), INTENT(IN) :: buf
    INTEGER(C_SIZE_T), VALUE :: len
    TYPE(httpresponse), INTENT(OUT) :: res
    INTEGER(C_INT) :: cdd_ret_
    cdd_ret_ = abstract_http_ipc_deserialize_response(TRIM(buf) // C_NULL_CHAR, len, res)
  END FUNCTION abstract_http_ipc_deserialize_response_str
END MODULE c_abstract_http_bind_mod
