C> @file fah_f77_strict.f
C> @brief Pure ANSI X3.9-1978 include-free FORTRAN 77 convenience HTTP
C> subprograms (strictly zero INCLUDE extensions, <= 6-char uppercase
C> identifiers, CHARACTER*(*), PARAMETER, and columns 1-72).
C
C> @brief Execute a self-contained HTTP GET in pure ANSI X3.9-1978 F77.
C> @param[in] ICLI Client handle.
C> @param[in] URL Destination URL string.
C> @param[out] OUTBDY Destination CHARACTER*(*) response buffer.
C> @param[out] ISTS HTTP status code.
C> @param[out] NLEN Actual response byte length.
C> @param[out] IERR Unified error code (0 on success).
      SUBROUTINE FHSGET(ICLI, URL, OUTBDY, ISTS, NLEN, IERR)
      INTEGER ICLI, ISTS, NLEN, IERR
      CHARACTER*(*) URL, OUTBDY
      INTEGER IHMGET
      PARAMETER (IHMGET=0)
      INTEGER IHRQCR, IHSEND, IHRSIN, IHRSFR, IHRQFR
      INTEGER IREQ, IRES, IDM, IDUM
      ISTS = 0
      NLEN = 0
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      CALL FH7REQ(IREQ, 1, IHMGET, URL, ' ', ' ', ' ', IERR)
      IF (IERR .EQ. 0) THEN
        IERR = IHSEND(ICLI, IREQ, IRES)
        IF (IERR .EQ. 0) THEN
          IERR = IHRSIN(IRES, ISTS, NLEN)
          IF (IERR .EQ. 0) THEN
            CALL FH7RES(IRES, 1, 0, OUTBDY, OUTBDY, NLEN, IDUM, IERR)
          ENDIF
          IDM = IHRSFR(IRES)
        ENDIF
      ENDIF
      IDM = IHRQFR(IREQ)
      RETURN
      END
C
C> @brief Execute a self-contained HTTP POST in pure ANSI X3.9-1978 F77.
C> @param[in] ICLI Client handle.
C> @param[in] URL Destination URL string.
C> @param[in] CTYPE Content-Type header value.
C> @param[in] REQBDY Request payload string.
C> @param[out] OUTBDY Destination CHARACTER*(*) response buffer.
C> @param[out] ISTS HTTP status code.
C> @param[out] NLEN Actual response byte length.
C> @param[out] IERR Unified error code (0 on success).
      SUBROUTINE FHSPST(ICLI, URL, CTYPE, REQBDY, OUTBDY, ISTS, NLEN,
     1 IERR)
      INTEGER ICLI, ISTS, NLEN, IERR
      CHARACTER*(*) URL, CTYPE, REQBDY, OUTBDY
      INTEGER IHMPST
      PARAMETER (IHMPST=1)
      INTEGER IHRQCR, IHSEND, IHRSIN, IHRSFR, IHRQFR
      INTEGER IREQ, IRES, IDM, IDUM
      ISTS = 0
      NLEN = 0
      IERR = IHRQCR(IREQ)
      IF (IERR .NE. 0) RETURN
      CALL FH7REQ(IREQ, 1, IHMPST, URL, ' ', ' ', ' ', IERR)
      IF (IERR .EQ. 0) THEN
        CALL FH7REQ(IREQ, 2, 0, 'Content-Type', CTYPE, ' ', ' ', IERR)
      ENDIF
      IF (IERR .EQ. 0) THEN
        CALL FH7REQ(IREQ, 3, 0, REQBDY, ' ', ' ', ' ', IERR)
      ENDIF
      IF (IERR .EQ. 0) THEN
        IERR = IHSEND(ICLI, IREQ, IRES)
        IF (IERR .EQ. 0) THEN
          IERR = IHRSIN(IRES, ISTS, NLEN)
          IF (IERR .EQ. 0) THEN
            CALL FH7RES(IRES, 1, 0, OUTBDY, OUTBDY, NLEN, IDUM, IERR)
          ENDIF
          IDM = IHRSFR(IRES)
        ENDIF
      ENDIF
      IDM = IHRQFR(IREQ)
      RETURN
      END
