C> @file fah_f77.f
C> @brief Full FORTRAN 77 (ANSI X3.9-1978 + MIL-STD-1753 INCLUDE) SDK
C> exposing CHARACTER*(*), OAuth2, Multipart, Cookies, WebSockets, SSE,
C> Event Loop, Thread Pool, Mutex/Cond, TLS, Coroutines, Actors, IPC,
C> and CMP Integration with <= 6-char identifiers in columns 1-72.
C
C> @brief Initialize global transport layer (FORTRAN 77).
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7INI(IERR)
      INCLUDE 'fah77.inc'
      INTEGER IERR, IHGINI
      IERR = IHGINI(0)
      RETURN
      END
C
C> @brief Clean up global transport layer (FORTRAN 77).
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7CLN(IERR)
      INCLUDE 'fah77.inc'
      INTEGER IERR, IHGCLN
      IERR = IHGCLN(0)
      RETURN
      END
C
C> @brief Create an HttpConfig handle (FORTRAN 77).
C> @param[out] ICFG Receives positive config handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7CCR(ICFG, IERR)
      INTEGER ICFG, IERR, IHCFCR
      IERR = IHCFCR(ICFG)
      RETURN
      END
C
C> @brief Destroy an HttpConfig handle (FORTRAN 77).
C> @param[in] ICFG Config handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7CFR(ICFG, IERR)
      INTEGER ICFG, IERR, IHCFFR
      IERR = IHCFFR(ICFG)
      RETURN
      END
C
C> @brief Create an HttpClient handle (FORTRAN 77).
C> @param[in] ICFG Optional config handle (0 for defaults).
C> @param[in] IRAW Non-zero for raw sockets, 0 for OS default.
C> @param[out] ICLI Receives positive client handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7CLC(ICFG, IRAW, ICLI, IERR)
      INTEGER ICFG, IRAW, ICLI, IERR, IHCLCR
      IERR = IHCLCR(ICFG, IRAW, ICLI)
      RETURN
      END
C
C> @brief Destroy an HttpClient handle (FORTRAN 77).
C> @param[in] ICLI Client handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7CLF(ICLI, IERR)
      INTEGER ICLI, IERR, IHCLFR
      IERR = IHCLFR(ICLI)
      RETURN
      END
C
C> @brief Create an HttpRequest handle (FORTRAN 77).
C> @param[out] IREQ Receives positive request handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RCR(IREQ, IERR)
      INTEGER IREQ, IERR, IHRQCR
      IERR = IHRQCR(IREQ)
      RETURN
      END
C
C> @brief Destroy an HttpRequest handle (FORTRAN 77).
C> @param[in] IREQ Request handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RFR(IREQ, IERR)
      INTEGER IREQ, IERR, IHRQFR
      IERR = IHRQFR(IREQ)
      RETURN
      END
C
C> @brief Configure method and CHARACTER*(*) URL on an HttpRequest.
C> @param[in] IREQ Request handle.
C> @param[in] IMETH HTTP verb (IHMGET..IHMCON).
C> @param[in] URL Destination URL string.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RTG(IREQ, IMETH, URL, IERR)
      INTEGER IREQ, IMETH, IERR
      CHARACTER*(*) URL
      CALL FH7REQ(IREQ, 1, IMETH, URL, ' ', ' ', ' ', IERR)
      RETURN
      END
C
C> @brief Add a CHARACTER*(*) header pair onto an HttpRequest.
C> @param[in] IREQ Request handle.
C> @param[in] HKEY Header name.
C> @param[in] HVAL Header value.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RHD(IREQ, HKEY, HVAL, IERR)
      INTEGER IREQ, IERR
      CHARACTER*(*) HKEY, HVAL
      CALL FH7REQ(IREQ, 2, 0, HKEY, HVAL, ' ', ' ', IERR)
      RETURN
      END
C
C> @brief Set a binary-safe explicit-length payload on an HttpRequest.
C> @param[in] IREQ Request handle.
C> @param[in] BODY Raw character/byte buffer.
C> @param[in] NBYTES Exact byte length to send.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RBB(IREQ, BODY, NBYTES, IERR)
      INTEGER IREQ, NBYTES, IERR
      CHARACTER*(*) BODY
      CALL FH7REQ(IREQ, 4, NBYTES, BODY, ' ', ' ', ' ', IERR)
      RETURN
      END
C
C> @brief Add an RFC 7578 multipart part onto an HttpRequest.
C> @param[in] IREQ Request handle.
C> @param[in] PNAME Part field name.
C> @param[in] FNAME Optional filename (' ' for none).
C> @param[in] CTYPE Optional MIME type (' ' for default).
C> @param[in] PDATA Part payload buffer.
C> @param[in] NBYTES Exact payload byte length.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RPT(IREQ, PNAME, FNAME, CTYPE, PDATA, NBYTES, IERR)
      INTEGER IREQ, NBYTES, IERR
      CHARACTER*(*) PNAME, FNAME, CTYPE, PDATA
      CALL FH7REQ(IREQ, 5, NBYTES, PNAME, FNAME, CTYPE, PDATA, IERR)
      RETURN
      END
C
C> @brief Execute an HttpRequest synchronously and read status + len.
C> @param[in] ICLI Client handle.
C> @param[in] IREQ Request handle.
C> @param[out] IRES Receives positive response handle.
C> @param[out] ISTS Receives HTTP status code.
C> @param[out] NLEN Receives response body byte length.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7SND(ICLI, IREQ, IRES, ISTS, NLEN, IERR)
      INTEGER ICLI, IREQ, IRES, ISTS, NLEN, IERR
      INTEGER IHSEND, IHRSIN
      ISTS = 0
      NLEN = 0
      IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR .EQ. 0) THEN
        IERR = IHRSIN(IRES, ISTS, NLEN)
      ENDIF
      RETURN
      END
C
C> @brief Copy response body into a CHARACTER*(*) buffer (space-padded).
C> @param[in] IRES Response handle.
C> @param[out] BUF Destination CHARACTER*(*) buffer.
C> @param[out] NLEN Full untruncated body byte length.
C> @param[out] IERR Unified error code (IHEBUF if truncated).
      SUBROUTINE FH7RSB(IRES, BUF, NLEN, IERR)
      INTEGER IRES, NLEN, IERR, IDUM
      CHARACTER*(*) BUF
      CALL FH7RES(IRES, 1, 0, BUF, BUF, NLEN, IDUM, IERR)
      RETURN
      END
C
C> @brief Retrieve a response header value by key into CHARACTER*(*).
C> @param[in] IRES Response handle.
C> @param[in] HKEY Case-insensitive header name.
C> @param[out] HVAL Destination CHARACTER*(*) buffer.
C> @param[out] NLEN Full untruncated header value length.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RSH(IRES, HKEY, HVAL, NLEN, IERR)
      INTEGER IRES, NLEN, IERR, IDUM
      CHARACTER*(*) HKEY, HVAL
      CHARACTER*128 KTMP
      KTMP = HKEY
      CALL FH7RES(IRES, 2, 0, KTMP, HVAL, NLEN, IDUM, IERR)
      RETURN
      END
C
C> @brief Save response body directly to disk path and/or destroy IRES.
C> @param[in] IRES Response handle.
C> @param[in] FPATH Destination file path (' ' to skip saving).
C> @param[in] LFREE Non-zero to destroy IRES after optional save.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH7RSF(IRES, FPATH, LFREE, IERR)
      INTEGER IRES, LFREE, IERR, IHRSFR, N1, N2
      CHARACTER*(*) FPATH
      CHARACTER*512 PTMP
      IERR = 0
      IF (FPATH .NE. ' ') THEN
        PTMP = FPATH
        CALL FH7RES(IRES, 5, 0, PTMP, PTMP, N1, N2, IERR)
      ENDIF
      IF (LFREE .NE. 0) THEN
        N1 = IHRSFR(IRES)
        IF (IERR .EQ. 0) IERR = N1
      ENDIF
      RETURN
      END
C
C> @brief Format unified error code into CHARACTER*(*) diagnostic text.
C> @param[in] ICODE Error code.
C> @param[in] OPNAME Originating subroutine or operation name.
C> @param[out] OUTMSG Space-padded formatted diagnostic buffer.
C> @param[out] ICAT Semantic error category (0..8).
C> @param[out] LRETRY .TRUE. if retryable, .FALSE. otherwise.
C> @param[out] NLEN Untruncated formatted length.
C> @param[out] IERR Formatting status (0 or IHEBUF).
      SUBROUTINE FH7FMT(ICODE, OPNAME, OUTMSG, ICAT, LRETRY, NLEN, IERR)
      INTEGER ICODE, ICAT, NLEN, IERR, IRET
      LOGICAL LRETRY
      CHARACTER*(*) OPNAME, OUTMSG
      CALL FH7ERR(ICODE, OPNAME, OUTMSG, ICAT, IRET, NLEN, IERR)
      LRETRY = (IRET .NE. 0)
      RETURN
      END
