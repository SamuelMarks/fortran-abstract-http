C> @file fah77.inc
C> @brief Unified 6-character FORTRAN 77 PARAMETER constants for
C> fortran-abstract-http (strictly columns 1-72).
C
C     --- Unified Error Codes (fah_error_code_t) ---
C> @brief Operation succeeded (FAH_OK)
      INTEGER IHEOK
C> @brief Invalid argument (FAH_ERR_INVAL)
      INTEGER IHEINV
C> @brief Out of memory (FAH_ERR_NOMEM)
      INTEGER IHENOM
C> @brief Network or I/O error (FAH_ERR_IO)
      INTEGER IHEIO
C> @brief Operation not supported (FAH_ERR_NOTSUP)
      INTEGER IHENSU
C> @brief Operation timed out (FAH_ERR_TIMEOUT)
      INTEGER IHETMO
C> @brief System call failed (FAH_ERR_SYSCALL)
      INTEGER IHESYS
C> @brief WebSocket framing violation (FAH_ERR_WS_FRAMING)
      INTEGER IHEWSF
C> @brief WebSocket handshake failed (FAH_ERR_WS_HANDSHAKE)
      INTEGER IHEWSH
C> @brief SSE stream parse error (FAH_ERR_SSE_PARSE)
      INTEGER IHESSP
C> @brief SSE buffer OOM (FAH_ERR_SSE_OOM)
      INTEGER IHESSO
C> @brief WebSocket closed (FAH_ERR_WS_CLOSED)
      INTEGER IHEWSC
C> @brief Message size exceeded (FAH_ERR_MSGSIZE)
      INTEGER IHEMSG
C> @brief Null handle passed (FAH_ERR_NULL_HANDLE)
      INTEGER IHENUL
C> @brief Handle type tag mismatch (FAH_ERR_BAD_HANDLE_TAG)
      INTEGER IHETAG
C> @brief Stale or freed handle (FAH_ERR_STALE_HANDLE)
      INTEGER IHESTL
C> @brief Handle table full (FAH_ERR_HANDLE_TABLE_FULL)
      INTEGER IHEFUL
C> @brief Output buffer truncated (FAH_ERR_BUF_TOO_SMALL)
      INTEGER IHEBUF
C> @brief Invalid slice/length (FAH_ERR_INVALID_SLICE)
      INTEGER IHESLC
C> @brief Fortran callback returned error (FAH_ERR_CALLBACK_FAULT)
      INTEGER IHECBF
C> @brief Invalid CFI descriptor (FAH_ERR_CFI_DESCRIPTOR)
      INTEGER IHECFI
C> @brief Invalid Hollerith byte code (FAH_ERR_HOLLERITH_DECODE)
      INTEGER IHEHOL
      PARAMETER (IHEOK=0, IHEINV=-1, IHENOM=-2, IHEIO=-3)
      PARAMETER (IHENSU=-4, IHETMO=-5, IHESYS=-6)
      PARAMETER (IHEWSF=-1001, IHEWSH=-1002, IHESSP=-1003)
      PARAMETER (IHESSO=-1004, IHEWSC=-1005, IHEMSG=90)
      PARAMETER (IHENUL=-2001, IHETAG=-2002, IHESTL=-2003)
      PARAMETER (IHEFUL=-2004, IHEBUF=-2005, IHESLC=-2006)
      PARAMETER (IHECBF=-2007, IHECFI=-2008, IHEHOL=-2009)
C
C     --- HTTP Methods (enum HttpMethod) ---
C> @brief HTTP GET verb
      INTEGER IHMGET
C> @brief HTTP POST verb
      INTEGER IHMPST
C> @brief HTTP PUT verb
      INTEGER IHMPUT
C> @brief HTTP DELETE verb
      INTEGER IHMDEL
C> @brief HTTP PATCH verb
      INTEGER IHMPCH
C> @brief HTTP HEAD verb
      INTEGER IHMPHD
C> @brief HTTP OPTIONS verb
      INTEGER IHMOPT
C> @brief HTTP TRACE verb
      INTEGER IHMTRC
C> @brief HTTP QUERY verb
      INTEGER IHMQRY
C> @brief HTTP CONNECT verb
      INTEGER IHMCON
      PARAMETER (IHMGET=0, IHMPST=1, IHMPUT=2, IHMDEL=3, IHMPCH=4)
      PARAMETER (IHMPHD=5, IHMOPT=6, IHMTRC=7, IHMQRY=8, IHMCON=9)
C
C     --- Retry Policies (enum HttpRetryPolicy) ---
C> @brief No retries
      INTEGER IHRNON
C> @brief Linear retry delay
      INTEGER IHRLIN
C> @brief Exponential backoff retry
      INTEGER IHREXP
      PARAMETER (IHRNON=0, IHRLIN=1, IHREXP=2)
C
C     --- Execution Modalities (enum ExecutionModality) ---
C> @brief Synchronous blocking modality
      INTEGER IHOSYN
C> @brief Asynchronous event-loop modality
      INTEGER IHOASY
C> @brief Thread-pool modality
      INTEGER IHOTPL
C> @brief Multi-process IPC modality
      INTEGER IHOMPR
C> @brief Cooperative coroutine modality
      INTEGER IHOCOR
C> @brief Actor message-bus modality
      INTEGER IHOACT
      PARAMETER (IHOSYN=0, IHOASY=1, IHOTPL=2)
      PARAMETER (IHOMPR=3, IHOCOR=4, IHOACT=5)
C
C     --- HTTP & TLS Version Masks ---
C> @brief Default HTTP version mask
      INTEGER IHVDEF
C> @brief HTTP/1.0 bitmask
      INTEGER IHV10
C> @brief HTTP/1.1 bitmask
      INTEGER IHV11
C> @brief HTTP/2 bitmask
      INTEGER IHV20
C> @brief HTTP/3 bitmask
      INTEGER IHV30
C> @brief Any HTTP version bitmask
      INTEGER IHVANY
      PARAMETER (IHVDEF=0, IHV10=1, IHV11=2, IHV20=4, IHV30=8)
      PARAMETER (IHVANY=15)
C
C     --- WebSocket Opcodes (enum c_abstract_http_ws_opcode) ---
C> @brief WebSocket continuation frame
      INTEGER IHWCNT
C> @brief WebSocket UTF-8 text frame
      INTEGER IHWTXT
C> @brief WebSocket binary frame
      INTEGER IHWBIN
C> @brief WebSocket close frame
      INTEGER IHWCLS
C> @brief WebSocket ping frame
      INTEGER IHWPNG
C> @brief WebSocket pong frame
      INTEGER IHWPON
      PARAMETER (IHWCNT=0, IHWTXT=1, IHWBIN=2)
      PARAMETER (IHWCLS=8, IHWPNG=9, IHWPON=10)
