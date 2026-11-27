C> @file fah_f57.f
C> @brief IBM 704 FORTRAN (I) (1957) HTTP/WS/SSE external FUNCTION
C> library using implicit I-N integer typing, 6-char uppercase names,
C> DIMENSION statements, and 3-way arithmetic IF control flow.
C
C> @brief Initialize global HTTP transport layer (FORTRAN I).
C> @param[in] IDUM Dummy integer argument.
C> @return 0 on success, negative/errno code on failure.
      FUNCTION IH1INI(IDUM)
      IH1INI = IHGINI(IDUM)
      RETURN
      END
C
C> @brief Clean up global HTTP transport layer (FORTRAN I).
C> @param[in] IDUM Dummy integer argument.
C> @return 0 on success, negative/errno code on failure.
      FUNCTION IH1CLN(IDUM)
      IH1CLN = IHGCLN(IDUM)
      RETURN
      END
C
C> @brief Allocate an HTTP client handle (FORTRAN I).
C> @param[in] IRAW Non-zero for raw-socket HTTP/1.1, 0 for OS default.
C> @param[out] ICLI Receives positive 31-bit integer client handle.
C> @return 0 on success, negative error code on failure.
      FUNCTION IH1CLI(IRAW, ICLI)
      IH1CLI = IHCLCR(0, IRAW, ICLI)
      RETURN
      END
C
C> @brief Destroy an HTTP client handle (FORTRAN I).
C> @param[in] ICLI Active client handle.
C> @return 0 on success, negative error code on failure.
      FUNCTION IH1FIN(ICLI)
      IH1FIN = IHCLFR(ICLI)
      RETURN
      END
C
C> @brief Execute an HTTP GET using unpacked ASCII integer codes.
C> @param[in] ICLI Active client handle.
C> @param[in] IURL Array of unpacked ASCII integer character codes.
C> @param[in] NURL Character length of IURL.
C> @param[out] IRES Receives positive integer response handle.
C> @param[out] ISTS Receives HTTP status code (e.g. 200).
C> @param[out] NBDY Receives response body byte length.
C> @return 0 on success, unified error code on failure.
      FUNCTION IH1GET(ICLI, IURL, NURL, IRES, ISTS, NBDY)
      DIMENSION IURL(1)
      IRES = 0
      ISTS = 0
      NBDY = 0
      IRC = IHRQCR(IREQ)
      IF (IRC) 90, 10, 90
   10 IRC = IHRQTG(IREQ, 0, 0, IURL, NURL)
      IF (IRC) 80, 20, 80
   20 IRC = IHSEND(ICLI, IREQ, IRES)
      IF (IRC) 80, 30, 80
   30 IRC = IHRSIN(IRES, ISTS, NBDY)
      IDM = IHRQFR(IREQ)
      IH1GET = IDM + IRC
      RETURN
   80 IDM = IHRQFR(IREQ)
   90 IH1GET = IRC
      RETURN
      END
C
C> @brief Execute an HTTP POST with unpacked ASCII URL and body arrays.
C> @param[in] ICLI Active client handle.
C> @param[in] IURL Unpacked ASCII integer URL array.
C> @param[in] NURL Length of IURL.
C> @param[in] IBDY Unpacked byte integer request body array.
C> @param[in] NBLN Length of IBDY.
C> @param[out] IRES Receives response handle.
C> @param[out] ISTS Receives HTTP status code.
C> @param[out] NBDY Receives response body byte length.
C> @return 0 on success, unified error code on failure.
      FUNCTION IH1PST(ICLI, IURL, NURL, IBDY, NBLN, IRES, ISTS, NBDY)
      DIMENSION IURL(1), IBDY(1)
      IRES = 0
      ISTS = 0
      NBDY = 0
      IRC = IHRQCR(IREQ)
      IF (IRC) 90, 10, 90
   10 IRC = IHRQTG(IREQ, 1, 0, IURL, NURL)
      IF (IRC) 80, 20, 80
   20 IRC = IHRQBD(IREQ, 0, IBDY, NBLN)
      IF (IRC) 80, 30, 80
   30 IRC = IHSEND(ICLI, IREQ, IRES)
      IF (IRC) 80, 40, 80
   40 IRC = IHRSIN(IRES, ISTS, NBDY)
      IDM = IHRQFR(IREQ)
      IH1PST = IDM + IRC
      RETURN
   80 IDM = IHRQFR(IREQ)
   90 IH1PST = IRC
      RETURN
      END
C
C> @brief Copy response body into unpacked INTEGER array and free IRES.
C> @param[in] IRES Response handle.
C> @param[out] IBUF Destination unpacked integer array.
C> @param[in] ICAP Element capacity of IBUF.
C> @param[out] NOUT Receives actual response byte count.
C> @return 0 on success, unified error code on failure.
      FUNCTION IH1BDY(IRES, IBUF, ICAP, NOUT)
      DIMENSION IBUF(1)
      IRC = IHRSBD(IRES, 0, IBUF, ICAP, NOUT)
      IDM = IHRSFR(IRES)
      IF (IRC) 10, 20, 10
   10 IH1BDY = IRC
      RETURN
   20 IH1BDY = IDM
      RETURN
      END
