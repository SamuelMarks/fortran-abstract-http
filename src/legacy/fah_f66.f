C> @file fah_f66.f
C> @brief Strict ANSI X3.9-1966 (FORTRAN 66) HTTP, Streaming Upload,
C> Header Inspection, and Error Classification Layer.
C> Uses zero CHARACTER types, zero INCLUDE directives, zero THEN/ENDIF
C> blocks, <= 6-char uppercase names, and zero-trip-guarded DO loops.
C
C> @brief Initialize global HTTP transport (ANSI FORTRAN 66).
C> @param[out] IERR Unified error code (0 on success).
      SUBROUTINE FH6INI(IERR)
      INTEGER IERR, IHGINI
      IERR = IHGINI(0)
      RETURN
      END
C
C> @brief Clean up global HTTP transport (ANSI FORTRAN 66).
C> @param[out] IERR Unified error code (0 on success).
      SUBROUTINE FH6CLN(IERR)
      INTEGER IERR, IHGCLN
      IERR = IHGCLN(0)
      RETURN
      END
C
C> @brief Create an HTTP client handle (ANSI FORTRAN 66).
C> @param[in] ICFG Optional config handle (0 for defaults).
C> @param[in] IRAW Non-zero for raw sockets, 0 for OS default.
C> @param[out] ICLI Receives positive client handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH6CLI(ICFG, IRAW, ICLI, IERR)
      INTEGER ICFG, IRAW, ICLI, IERR, IHCLCR
      IERR = IHCLCR(ICFG, IRAW, ICLI)
      RETURN
      END
C
C> @brief Destroy an HTTP client handle (ANSI FORTRAN 66).
C> @param[in] ICLI Client handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH6FIN(ICLI, IERR)
      INTEGER ICLI, IERR, IHCLFR
      IERR = IHCLFR(ICLI)
      RETURN
      END
C
C> @brief Execute a Hollerith HTTP request and read header + body.
C> @param[in] ICLI Client handle.
C> @param[in] IMETH HTTP verb (0=GET, 1=POST, etc.).
C> @param[in] IHURL 4-char Hollerith URL word array.
C> @param[in] NURL Exact URL character count.
C> @param[in] IHKY 4-char Hollerith request/response header key.
C> @param[in] NKY Header key character count (0 to skip).
C> @param[in] IHVL 4-char Hollerith request header value.
C> @param[in] NVL Request header value character count.
C> @param[out] IHRV Unpacked INTEGER buffer for response header value.
C> @param[in] NRCAP Capacity of IHRV.
C> @param[out] NRLN Response header value length (0 if absent).
C> @param[out] IBOUT Unpacked INTEGER buffer for response body.
C> @param[in] IBCAP Capacity of IBOUT.
C> @param[out] NBLN Response body byte length.
C> @param[out] ISTS HTTP status code.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH6REQ(ICLI, IMETH, IHURL, NURL, IHKY, NKY, IHVL, NVL,
     1 IHRV, NRCAP, NRLN, IBOUT, IBCAP, NBLN, ISTS, IERR)
      INTEGER ICLI, IMETH, IHURL(1), NURL, IHKY(1), NKY, IHVL(1), NVL
      INTEGER IHRV(1), NRCAP, NRLN, IBOUT(1), IBCAP, NBLN, ISTS, IERR
      INTEGER IHRQCR, IHRQTG, IHRQHD, IHSEND, IHRSIN, IHRSBD, IHRSHD
      INTEGER IHRSFR, IHRQFR
      INTEGER IREQ, IRES, IDM, IHDRC
      ISTS = 0
      NRLN = 0
      NBLN = 0
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      IERR = IHRQTG(IREQ, IMETH, 1, IHURL, NURL)
      IF (IERR .NE. 0) GOTO 80
      IF (NKY .LE. 0) GOTO 20
      IERR = IHRQHD(IREQ, 1, IHKY, NKY, IHVL, NVL)
      IF (IERR .NE. 0) GOTO 80
   20 IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR .NE. 0) GOTO 80
      IERR = IHRSIN(IRES, ISTS, NBLN)
      IF (IERR .NE. 0) GOTO 70
      IF (NKY .LE. 0) GOTO 30
      IHDRC = IHRSHD(IRES, 1, IHKY, NKY, IHRV, NRCAP, NRLN)
      IF (IHDRC .NE. 0) NRLN = 0
   30 IERR = IHRSBD(IRES, 0, IBOUT, IBCAP, NBLN)
   70 IDM = IHRSFR(IRES)
   80 IDM = IHRQFR(IREQ)
      RETURN
      END
C
C> @brief Execute a streaming chunked upload via EXTERNAL RDCHUK (F66).
C> @param[in] ICLI Client handle.
C> @param[in] IHURL 4-char Hollerith URL word array.
C> @param[in] NURL Exact URL character count.
C> @param[in] RDCHUK EXTERNAL INTEGER FUNCTION(IUID, BUF, ICAP, NOUT).
C> @param[in] IUID Integer callback user context.
C> @param[in] IEXPLN Expected total upload byte length.
C> @param[out] ISTS HTTP status code.
C> @param[out] IBOUT Unpacked INTEGER response body buffer.
C> @param[in] IBCAP Capacity of IBOUT.
C> @param[out] NBLN Response body byte length.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH6UPL(ICLI, IHURL, NURL, RDCHUK, IUID, IEXPLN, ISTS,
     1 IBOUT, IBCAP, NBLN, IERR)
      INTEGER ICLI, IHURL(1), NURL, IUID, IEXPLN, ISTS
      INTEGER IBOUT(1), IBCAP, NBLN, IERR
      EXTERNAL RDCHUK
      INTEGER IHRQCR, IHRQTG, IHSEND, IHRSIN, IHRSBD, IHRSFR, IHRQFR
      INTEGER IREQ, IRES, IDM
      ISTS = 0
      NBLN = 0
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      IERR = IHRQTG(IREQ, 1, 1, IHURL, NURL)
      IF (IERR .NE. 0) GOTO 80
      CALL FHC4UP(IREQ, RDCHUK, IUID, IEXPLN, IERR)
      IF (IERR .NE. 0) GOTO 80
      IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR .NE. 0) GOTO 80
      IERR = IHRSIN(IRES, ISTS, NBLN)
      IF (IERR .NE. 0) GOTO 70
      IERR = IHRSBD(IRES, 0, IBOUT, IBCAP, NBLN)
   70 IDM = IHRSFR(IRES)
   80 IDM = IHRQFR(IREQ)
      RETURN
      END
C
C> @brief Inspect unified error code category, retryability, & message.
C> @param[in] ICODE Error code to classify.
C> @param[out] ICAT Receives semantic category (0..8).
C> @param[out] LRETRY .TRUE. if transient/retryable, .FALSE. otherwise.
C> @param[out] IMSG Unpacked ASCII integer array receiving diagnostic.
C> @param[in] ICAP Element capacity of IMSG.
C> @param[out] NLEN Diagnostic byte length.
C> @param[out] IERR Formatting status (0 or -2005 on truncation).
      SUBROUTINE FH6ERR(ICODE, ICAT, LRETRY, IMSG, ICAP, NLEN, IERR)
      INTEGER ICODE, ICAT, IMSG(1), ICAP, NLEN, IERR
      LOGICAL LRETRY
      INTEGER IHERRC, IRET, I
      LRETRY = .FALSE.
      IF (ICAP .LE. 0) GOTO 20
      DO 10 I = 1, ICAP
        IMSG(I) = 32
   10 CONTINUE
   20 IERR = IHERRC(ICODE, ICAT, IRET, IMSG, ICAP, NLEN)
      IF (IRET .NE. 0) LRETRY = .TRUE.
      RETURN
      END
