C> @file fah_f58.f
C> @brief IBM 704/709 FORTRAN II (1958) procedural SUBROUTINE, CALL,
C> RETURN, and COMMON /FAH2CM/ HTTP SDK (strictly <= 6-char names,
C> implicit I-N typing, columns 1-72).
C
C> @brief Initialize transport and reset COMMON /FAH2CM/ counters.
C> @param[out] IERR Unified error code (0 on success).
      SUBROUTINE FH2INI(IERR)
      COMMON /FAH2CM/ LASTER, LASTST, NREQS
      LASTST = 0
      NREQS = 0
      IERR = IHGINI(0)
      LASTER = IERR
      RETURN
      END
C
C> @brief Clean up global transport and update COMMON /FAH2CM/.
C> @param[out] IERR Unified error code (0 on success).
      SUBROUTINE FH2CLN(IERR)
      COMMON /FAH2CM/ LASTER, LASTST, NREQS
      IERR = IHGCLN(0)
      LASTER = IERR
      RETURN
      END
C
C> @brief Create an HTTP client handle in FORTRAN II.
C> @param[in] IRAW Non-zero for raw sockets, 0 for OS default.
C> @param[out] ICLI Receives positive client handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH2CLI(IRAW, ICLI, IERR)
      COMMON /FAH2CM/ LASTER, LASTST, NREQS
      IERR = IHCLCR(0, IRAW, ICLI)
      LASTER = IERR
      RETURN
      END
C
C> @brief Destroy an HTTP client handle in FORTRAN II.
C> @param[in] ICLI Client handle.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH2FIN(ICLI, IERR)
      COMMON /FAH2CM/ LASTER, LASTST, NREQS
      IERR = IHCLFR(ICLI)
      LASTER = IERR
      RETURN
      END
C
C> @brief Execute an HTTP request with custom header and optional body.
C> @param[in] ICLI Client handle.
C> @param[in] IMETH HTTP verb (0=GET, 1=POST, 2=PUT, 3=DELETE).
C> @param[in] IURL Unpacked integer URL array.
C> @param[in] NURL URL length.
C> @param[in] IKEY Unpacked integer header key array.
C> @param[in] NKEY Header key length (0 to skip).
C> @param[in] IVAL Unpacked integer header value array.
C> @param[in] NVAL Header value length.
C> @param[in] IBDY Unpacked integer request body array.
C> @param[in] NBDY Request body length (0 for empty).
C> @param[out] IOUT Unpacked integer response body buffer.
C> @param[in] ICAP Capacity of IOUT.
C> @param[out] ISTS HTTP status code.
C> @param[out] NOUT Actual response body length.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH2REQ(ICLI, IMETH, IURL, NURL, IKEY, NKEY, IVAL,
     1 NVAL, IBDY, NBDY, IOUT, ICAP, ISTS, NOUT, IERR)
      COMMON /FAH2CM/ LASTER, LASTST, NREQS
      DIMENSION IURL(1), IKEY(1), IVAL(1), IBDY(1), IOUT(1)
      ISTS = 0
      NOUT = 0
      IERR = IHRQCR(IREQ)
      IF (IERR) 90, 10, 90
   10 IERR = IHRQTG(IREQ, IMETH, 0, IURL, NURL)
      IF (IERR) 80, 20, 80
   20 IF (NKEY) 40, 40, 30
   30 IERR = IHRQHD(IREQ, 0, IKEY, NKEY, IVAL, NVAL)
      IF (IERR) 80, 40, 80
   40 IF (NBDY) 60, 60, 50
   50 IERR = IHRQBD(IREQ, 0, IBDY, NBDY)
      IF (IERR) 80, 60, 80
   60 IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR) 80, 70, 80
   70 IERR = IHRSIN(IRES, ISTS, ILEN)
      IF (IERR) 75, 72, 75
   72 IERR = IHRSBD(IRES, 0, IOUT, ICAP, NOUT)
      LASTST = ISTS
      NREQS = NREQS + 1
   75 IDM = IHRSFR(IRES)
   80 IDM = IHRQFR(IREQ)
   90 LASTER = IERR
      RETURN
      END
