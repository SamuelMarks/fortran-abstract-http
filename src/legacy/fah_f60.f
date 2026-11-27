C> @file fah_f60.f
C> @brief Portable IBM 704/709 FORTRAN III (1958-1961) HTTP, Boolean
C> bitmask, 4-char Hollerith word, and pull-polling streaming layer.
C
C> @brief Compose a Boolean HTTP/TLS version bitmask (FORTRAN III).
C> @param[in] IV10 Non-zero to include HTTP/1.0 / TLS 1.0 (bit 0).
C> @param[in] IV11 Non-zero to include HTTP/1.1 / TLS 1.1 (bit 1).
C> @param[in] IV20 Non-zero to include HTTP/2 / TLS 1.2 (bit 2).
C> @param[in] IV30 Non-zero to include HTTP/3 / TLS 1.3 (bit 3).
C> @param[out] IMASK Composed integer bitmask (0..15).
C> @param[out] IERR Always 0 (FAH_OK) or -1 if negative flag passed.
      SUBROUTINE FH3BMS(IV10, IV11, IV20, IV30, IMASK, IERR)
      IMASK = 0
      IF (IV10) 90, 10, 5
    5 IMASK = IMASK + 1
   10 IF (IV11) 90, 20, 15
   15 IMASK = IMASK + 2
   20 IF (IV20) 90, 30, 25
   25 IMASK = IMASK + 4
   30 IF (IV30) 90, 40, 35
   35 IMASK = IMASK + 8
   40 IERR = 0
      RETURN
   90 IERR = -1
      RETURN
      END
C
C> @brief Execute an HTTP request using packed 4-char Hollerith words.
C> @param[in] ICLI Client handle.
C> @param[in] IMETH HTTP verb (0=GET, 1=POST, etc.).
C> @param[in] IHURL Packed 4-char Hollerith URL word array.
C> @param[in] NURL Exact character count of URL.
C> @param[out] IHOUT Packed 4-char Hollerith response word buffer.
C> @param[in] NWCAP Capacity of IHOUT in 4-byte INTEGER words.
C> @param[out] ISTS HTTP status code.
C> @param[out] NOUT Actual response byte count.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH3HOL(ICLI, IMETH, IHURL, NURL, IHOUT, NWCAP, ISTS,
     1 NOUT, IERR)
      DIMENSION IHURL(1), IHOUT(1)
      ISTS = 0
      NOUT = 0
      IERR = IHRQCR(IREQ)
      IF (IERR) 90, 10, 90
   10 IERR = IHRQTG(IREQ, IMETH, 1, IHURL, NURL)
      IF (IERR) 80, 20, 80
   20 IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR) 80, 30, 80
   30 IERR = IHRSIN(IRES, ISTS, ILEN)
      IF (IERR) 70, 40, 70
   40 IERR = IHRSBD(IRES, 1, IHOUT, NWCAP, NOUT)
   70 IDM = IHRSFR(IRES)
   80 IDM = IHRQFR(IREQ)
   90 RETURN
      END
C
C> @brief Stream HTTP response chunks via FahPollQueue (FORTRAN III).
C> @param[in] ICLI Client handle.
C> @param[in] IHURL Packed 4-char Hollerith URL word array.
C> @param[in] NURL Exact character count of URL.
C> @param[out] IBUF Unpacked integer buffer receiving last chunk.
C> @param[in] ICAP Element capacity of IBUF.
C> @param[out] ISTS HTTP status code.
C> @param[out] NCHKS Number of chunks drained from poll queue.
C> @param[out] NTOT Total byte count drained across all chunks.
C> @param[out] IERR Unified error code.
      SUBROUTINE FH3POL(ICLI, IHURL, NURL, IBUF, ICAP, ISTS, NCHKS,
     1 NTOT, IERR)
      DIMENSION IHURL(1), IBUF(1)
      ISTS = 0
      NCHKS = 0
      NTOT = 0
      IERR = IHPQCR(IQH)
      IF (IERR) 95, 10, 95
   10 IERR = IHRQCR(IREQ)
      IF (IERR) 90, 20, 90
   20 IERR = IHRQTG(IREQ, 0, 1, IHURL, NURL)
      IF (IERR) 80, 30, 80
   30 IERR = IHRQCB(IREQ, IQH)
      IF (IERR) 80, 40, 80
   40 IERR = IHSEND(ICLI, IREQ, IRES)
      IF (IERR) 80, 50, 80
   50 IERR = IHRSIN(IRES, ISTS, ILEN)
      IDM = IHRSFR(IRES)
      IF (IERR) 80, 60, 80
   60 IERR = IHPQPP(IQH, IKIND, ICODE, IBUF, ICAP, NCHUNK)
      IF (IERR) 80, 65, 80
   65 IF (IKIND) 80, 80, 70
   70 NCHKS = NCHKS + 1
      NTOT = NTOT + NCHUNK
      GOTO 60
   80 IDM = IHRQFR(IREQ)
   90 IDM = IHPQFR(IQH)
   95 RETURN
      END
