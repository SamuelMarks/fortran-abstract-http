!> @file c_abstract_http_bind_mod_test.f90
!> @brief Auto-generated Fortran ISO_C_BINDING C_SIZEOF ABI tests for c_abstract_http_bind_mod.
PROGRAM c_abstract_http_bind_mod_test_f90
  USE, INTRINSIC :: ISO_C_BINDING
  USE c_abstract_http_bind_mod
  IMPLICIT NONE
  TYPE(abstracthttpactor) :: v_1
  TYPE(abstracthttpmessage) :: v_2
  TYPE(abstracthttpmessagebus) :: v_5
  TYPE(abstracthttpactorhooks) :: v_7
  TYPE(abstracthttptlskey) :: v_16
  TYPE(cmpappconfig) :: v_21
  TYPE(cmpprogressbinding) :: v_22
  TYPE(httpcookie) :: v_27
  TYPE(httpcookiejar) :: v_28
  TYPE(httpconfig) :: v_29
  TYPE(httpheader) :: v_33
  TYPE(httpheaders) :: v_34
  TYPE(httppart) :: v_35
  TYPE(httpparts) :: v_36
  TYPE(httprequest) :: v_40
  TYPE(abstracthttpcoroutine) :: v_42
  TYPE(abstracthttpcoroutinehooks) :: v_44
  TYPE(modalityeventloop) :: v_53
  TYPE(httploophooks) :: v_56
  TYPE(httptransportcontext) :: v_75
  TYPE(httpresponse) :: v_80
  TYPE(httpmultirequest) :: v_82
  TYPE(httpfuture) :: v_83
  TYPE(httprawtlshooks) :: v_148
  TYPE(c_abstract_http_sse_event) :: v_158
  TYPE(c_abstract_http_sse_config) :: v_159
  TYPE(abstracthttpthreadpool) :: v_166
  TYPE(httpclient) :: v_167
  TYPE(modalitycontext) :: v_171
  TYPE(c_abstract_http_ws_event) :: v_265
  TYPE(c_abstract_http_ws_config) :: v_266
  TYPE(abstracthttpipcpipe) :: v_296
  TYPE(abstracthttpprocess) :: v_297
  TYPE(abstracthttpprocesshooks) :: v_298
  TYPE(abstracthttpthreadpoolhooks) :: v_311
  TYPE(abstracthttpmutex) :: v_312
  TYPE(abstracthttpcond) :: v_313
  IF (SIZE(TRANSFER(v_1, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_2, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_5, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_7, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_16, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_21, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_22, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_27, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_28, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_29, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_33, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_34, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_35, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_36, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_40, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_42, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_44, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_53, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_56, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_75, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_80, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_82, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_83, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_148, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_158, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_159, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_166, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_167, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_171, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_265, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_266, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_296, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_297, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_298, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_311, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_312, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
  IF (SIZE(TRANSFER(v_313, (/ 0_C_INT8_T /)), KIND=C_SIZE_T) <= 0_C_SIZE_T) STOP 1
END PROGRAM c_abstract_http_bind_mod_test_f90
