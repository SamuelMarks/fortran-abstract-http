C> @file fah_f62.f
C> @brief IBM 7090/System 360 FORTRAN IV (1962) HTTP, WebSocket, and
C> SSE layer featuring explicit INTEGER/LOGICAL declarations, logical
C> IF, BLOCK DATA, Hollerith DATA, and EXTERNAL callbacks.
C
C> @brief Default FORTRAN IV configuration COMMON block initializer.
      BLOCK DATA FAH4BD
      INTEGER IDEFTM, IDEF62
      LOGICAL LDEFVP, LDEFRD
      COMMON /FAH4CM/ IDEFTM, IDEF62, LDEFVP, LDEFRD
      DATA IDEFTM /5000/, IDEF62 /1962/
      DATA LDEFVP /.FALSE./, LDEFRD /.TRUE./
      END
C
C> @brief Create and populate an HttpConfig using FORTRAN IV LOGICALs.
C> @param[in] ITMO Timeout in ms (0 uses BLOCK DATA default IDEFTM).
C> @param[in] LVERIF .TRUE. to verify TLS peer, .FALSE. otherwise.
C> @param[in] LREDIR .TRUE. to follow 3xx redirects.
C> @param[in] IRETRY Max retry count.
C> @param[out] ICFG Receives positive config handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH4CFG(ITMO, LVERIF, LREDIR, IRETRY, ICFG, IERR)
      INTEGER ITMO, IRETRY, ICFG, IERR
      LOGICAL LVERIF, LREDIR
      INTEGER IDEFTM, IDEF62
      LOGICAL LDEFVP, LDEFRD
      COMMON /FAH4CM/ IDEFTM, IDEF62, LDEFVP, LDEFRD
      INTEGER IHCFCR, IHCFST, IHCFFR
      INTEGER IEFFTM, IVP, IRD, IDM
      ICFG = 0
      IEFFTM = ITMO
      IF (IEFFTM .LE. 0) IEFFTM = IDEFTM
      IVP = 0
      IF (LVERIF) IVP = 1
      IRD = 0
      IF (LREDIR) IRD = 1
      IERR = IHCFCR(ICFG)
      IF (IERR .NE. 0) RETURN
      IERR = IHCFST(ICFG, IEFFTM, IVP, IRD, IRETRY)
      IF (IERR .EQ. 0) RETURN
      IDM = IHCFFR(ICFG)
      ICFG = 0
      RETURN
      END
C
C> @brief Execute a streaming HTTP GET with EXTERNAL callbacks (F-IV).
C> @param[in] ICLI Client handle.
C> @param[in] IHURL Packed 4-char Hollerith URL word array.
C> @param[in] NURL Exact URL character count.
C> @param[in] ONCHUK EXTERNAL INTEGER FUNCTION(IUID, BUF, NLEN).
C> @param[in] ICHUID User context integer for ONCHUK.
C> @param[in] ONPROG EXTERNAL INTEGER FUNCTION(ICUR, ITOT, IUID).
C> @param[in] IPGUID User context integer for ONPROG.
C> @param[out] ISTS HTTP status code.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH4STR(ICLI, IHURL, NURL, ONCHUK, ICHUID, ONPROG,
     1 IPGUID, ISTS, IERR)
      INTEGER ICLI, IHURL(1), NURL, ICHUID, IPGUID, ISTS, IERR
      EXTERNAL ONCHUK, ONPROG
      INTEGER IHRQCR, IHRQTG, IHSEND, IHRSIN, IHRSFR, IHRQFR
      INTEGER IREQ, IRES, ILEN, IDM
      ISTS = 0
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      IERR = IHRQTG(IREQ, 0, 1, IHURL, NURL)
      IF (IERR .EQ. 0) CALL FHC4ST(IREQ, ONCHUK, ICHUID, ONPROG,
     1 IPGUID, IERR)
      IF (IERR .EQ. 0) IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR .NE. 0) GOTO 20
      IERR = IHRSIN(IRES, ISTS, ILEN)
      IDM = IHRSFR(IRES)
   20 IDM = IHRQFR(IREQ)
      RETURN
      END
C
C> @brief Run a synchronous WebSocket session with EXTERNAL callbacks.
C> @param[in] ICLI Client handle.
C> @param[in] IHURL Packed 4-char Hollerith ws:// URL word array.
C> @param[in] NURL Exact URL character count.
C> @param[in] ONMSG EXTERNAL INTEGER FUNCTION(IOP, BUF, NLEN, IFIN, ID).
C> @param[in] ONERR EXTERNAL INTEGER FUNCTION(IERR, ID).
C> @param[in] ONCLS EXTERNAL INTEGER FUNCTION(ISTS, ID).
C> @param[in] IUID Integer callback context.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH4WSS(ICLI, IHURL, NURL, ONMSG, ONERR, ONCLS, IUID,
     1 IERR)
      INTEGER ICLI, IHURL(1), NURL, IUID, IERR
      EXTERNAL ONMSG, ONERR, ONCLS
      INTEGER IHRQCR, IHRQTG, IHWSOP, IHRQFR
      INTEGER IREQ, IDM
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      IERR = IHRQTG(IREQ, 0, 1, IHURL, NURL)
      IF (IERR .EQ. 0) IERR = IHWSOP(ICLI, IREQ, 0, 1, 1, IHURL, 0, 0)
      IF (IERR .EQ. 0) CALL FHC4WS(ICLI, IREQ, ONMSG, ONERR, ONCLS,
     1 IUID, IERR)
      IDM = IHWSOP(ICLI, IREQ, 4, 0, 1, IHURL, 0, 0)
      IDM = IHRQFR(IREQ)
      RETURN
      END
C
C> @brief Run a synchronous SSE stream loop with EXTERNAL callbacks.
C> @param[in] ICLI Client handle.
C> @param[in] IHURL Packed 4-char Hollerith URL word array.
C> @param[in] NURL Exact URL character count.
C> @param[in] ONEVT EXTERNAL INTEGER FUNCTION for SSE events.
C> @param[in] ONERR EXTERNAL INTEGER FUNCTION(IERR, ID).
C> @param[in] ONCLS EXTERNAL INTEGER FUNCTION(ID).
C> @param[in] IUID Integer callback context.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH4SSE(ICLI, IHURL, NURL, ONEVT, ONERR, ONCLS, IUID,
     1 IERR)
      INTEGER ICLI, IHURL(1), NURL, IUID, IERR
      EXTERNAL ONEVT, ONERR, ONCLS
      INTEGER IHRQCR, IHRQTG, IHSSEO, IHRQFR
      INTEGER IREQ, IDM
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      IERR = IHRQTG(IREQ, 0, 1, IHURL, NURL)
      IF (IERR .EQ. 0) IERR = IHSSEO(ICLI, IREQ, 0, 100, 0)
      IF (IERR .EQ. 0) CALL FHC4SS(ICLI, IREQ, ONEVT, ONERR, ONCLS,
     1 IUID, IERR)
      IDM = IHSSEO(ICLI, IREQ, 2, 0, 0)
      IDM = IHRQFR(IREQ)
      RETURN
      END
