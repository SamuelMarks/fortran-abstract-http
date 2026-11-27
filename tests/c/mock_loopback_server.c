/**
 * @file mock_loopback_server.c
 * @brief Strict ISO C89 + POSIX loopback HTTP/1.1, OAuth2, RFC 6455 WebSocket,
 * and W3C SSE test server implementation.
 */

/* clang-format off */
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "fah_f77_strings.h"
#include "fah_hollerith.h"
#include "mock_loopback_server.h"
/* clang-format on */

struct MockServerSlot {
  int active;
  int listen_fd;
  int port;
  volatile int stop_flag;
  int flaky_hits;
  pthread_t thread;
};

static struct MockServerSlot g_servers[8];

/* --- Minimal RFC 3174 SHA-1 + Base64 for RFC 6455 Sec-WebSocket-Accept --- */

static unsigned int rol32(unsigned int v, unsigned int bits) {
  return ((v << bits) | (v >> (32u - bits))) & 0xFFFFFFFFu;
}

static void sha1_compute(const unsigned char *msg, size_t len,
                         unsigned char digest[20]) {
  unsigned int h0 = 0x67452301u;
  unsigned int h1 = 0xEFCDAB89u;
  unsigned int h2 = 0x98BADCFEu;
  unsigned int h3 = 0x10325476u;
  unsigned int h4 = 0xC3D2E1F0u;
  unsigned char block[128];
  size_t padded_len;
  size_t offset;
  size_t bit_len = len * 8u;

  memset(block, 0, sizeof(block));
  memcpy(block, msg, len);
  block[len] = 0x80u;
  padded_len = ((len + 8u) < 64u) ? 64u : 128u;
  block[padded_len - 4u] = (unsigned char)((bit_len >> 24u) & 0xFFu);
  block[padded_len - 3u] = (unsigned char)((bit_len >> 16u) & 0xFFu);
  block[padded_len - 2u] = (unsigned char)((bit_len >> 8u) & 0xFFu);
  block[padded_len - 1u] = (unsigned char)(bit_len & 0xFFu);

  for (offset = 0u; offset < padded_len; offset += 64u) {
    unsigned int w[80];
    unsigned int a = h0, b = h1, c = h2, d = h3, e = h4;
    int i;
    for (i = 0; i < 16; i++) {
      size_t p = offset + (size_t)(i * 4);
      w[i] = ((unsigned int)block[p] << 24u) |
             ((unsigned int)block[p + 1u] << 16u) |
             ((unsigned int)block[p + 2u] << 8u) |
             ((unsigned int)block[p + 3u]);
    }
    for (i = 16; i < 80; i++) {
      w[i] = rol32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1u);
    }
    for (i = 0; i < 80; i++) {
      unsigned int f, k, temp;
      if (i < 20) {
        f = (b & c) | ((~b) & d);
        k = 0x5A827999u;
      } else if (i < 40) {
        f = b ^ c ^ d;
        k = 0x6ED9EBA1u;
      } else if (i < 60) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8F1BBCDCu;
      } else {
        f = b ^ c ^ d;
        k = 0xCA62C1D6u;
      }
      temp = (rol32(a, 5u) + f + e + k + w[i]) & 0xFFFFFFFFu;
      e = d;
      d = c;
      c = rol32(b, 30u);
      b = a;
      a = temp;
    }
    h0 = (h0 + a) & 0xFFFFFFFFu;
    h1 = (h1 + b) & 0xFFFFFFFFu;
    h2 = (h2 + c) & 0xFFFFFFFFu;
    h3 = (h3 + d) & 0xFFFFFFFFu;
    h4 = (h4 + e) & 0xFFFFFFFFu;
  }
  {
    unsigned int hs[5];
    int j;
    hs[0] = h0;
    hs[1] = h1;
    hs[2] = h2;
    hs[3] = h3;
    hs[4] = h4;
    for (j = 0; j < 5; j++) {
      digest[j * 4] = (unsigned char)((hs[j] >> 24u) & 0xFFu);
      digest[j * 4 + 1] = (unsigned char)((hs[j] >> 16u) & 0xFFu);
      digest[j * 4 + 2] = (unsigned char)((hs[j] >> 8u) & 0xFFu);
      digest[j * 4 + 3] = (unsigned char)(hs[j] & 0xFFu);
    }
  }
}

static void base64_20(const unsigned char in[20], char out[32]) {
  static const char tbl[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int i = 0;
  int o = 0;
  while (i < 18) {
    unsigned int v = ((unsigned int)in[i] << 16u) |
                     ((unsigned int)in[i + 1] << 8u) |
                     ((unsigned int)in[i + 2]);
    out[o++] = tbl[(v >> 18u) & 0x3Fu];
    out[o++] = tbl[(v >> 12u) & 0x3Fu];
    out[o++] = tbl[(v >> 6u) & 0x3Fu];
    out[o++] = tbl[v & 0x3Fu];
    i += 3;
  }
  {
    unsigned int v =
        ((unsigned int)in[18] << 16u) | ((unsigned int)in[19] << 8u);
    out[o++] = tbl[(v >> 18u) & 0x3Fu];
    out[o++] = tbl[(v >> 12u) & 0x3Fu];
    out[o++] = tbl[(v >> 6u) & 0x3Fu];
    out[o++] = '=';
  }
  out[o] = '\0';
}

static void compute_ws_accept(const char *client_key, char out_accept[32]) {
  char concat[128];
  unsigned char sha[20];
  sprintf(concat, "%.60s258EAFA5-E914-47DA-95CA-C5AB0DC85B11", client_key);
  sha1_compute((const unsigned char *)concat, strlen(concat), sha);
  base64_20(sha, out_accept);
}

static int send_all(int fd, const void *buf, size_t len) {
  const char *p = (const char *)buf;
  size_t sent = 0u;
  while (sent < len) {
    ssize_t n = send(fd, p + sent, len - sent, 0);
    if (n <= 0) {
      return -1;
    }
    sent += (size_t)n;
  }
  return 0;
}

static int ci_starts_with(const char *line, const char *prefix) {
  while (*prefix != '\0') {
    if (tolower((unsigned char)*line) != tolower((unsigned char)*prefix)) {
      return 0;
    }
    line++;
    prefix++;
  }
  return 1;
}

static void extract_hdr(const char *hdr_block, const char *name_colon,
                        char *out_val, size_t cap) {
  const char *p = hdr_block;
  out_val[0] = '\0';
  while (*p != '\0') {
    const char *eol = strstr(p, "\r\n");
    size_t l = (eol != NULL) ? (size_t)(eol - p) : strlen(p);
    if (ci_starts_with(p, name_colon)) {
      const char *v = p + strlen(name_colon);
      size_t vl;
      while (*v == ' ' || *v == '\t') {
        v++;
      }
      vl = (eol != NULL && eol >= v) ? (size_t)(eol - v) : strlen(v);
      if (vl >= cap) {
        vl = cap - 1u;
      }
      memcpy(out_val, v, vl);
      out_val[vl] = '\0';
      return;
    }
    if (eol == NULL) {
      break;
    }
    p = p + l + 2u;
  }
}

static void handle_client_fd(struct MockServerSlot *srv, int cfd) {
  char req_buf[16384];
  size_t used = 0u;
  char *hdr_end = NULL;
  char method[16];
  char path[256];
  char cl_str[32];
  char te_str[64];
  char ws_key[96];
  char auth_h[256];
  char cust_h[256];
  char cook_h[256];
  char ctype_h[256];
  char body_buf[8192];
  size_t body_len = 0u;

  while (used + 1u < sizeof(req_buf)) {
    ssize_t r = recv(cfd, req_buf + used, sizeof(req_buf) - 1u - used, 0);
    if (r <= 0) {
      return;
    }
    used += (size_t)r;
    req_buf[used] = '\0';
    hdr_end = strstr(req_buf, "\r\n\r\n");
    if (hdr_end != NULL) {
      break;
    }
  }
  if (hdr_end == NULL) {
    return;
  }

  method[0] = '\0';
  path[0] = '\0';
  (void)sscanf(req_buf, "%15s %255s", method, path);
  *hdr_end = '\0';

  extract_hdr(req_buf, "Content-Length:", cl_str, sizeof(cl_str));
  extract_hdr(req_buf, "Transfer-Encoding:", te_str, sizeof(te_str));
  extract_hdr(req_buf, "Sec-WebSocket-Key:", ws_key, sizeof(ws_key));
  extract_hdr(req_buf, "Authorization:", auth_h, sizeof(auth_h));
  extract_hdr(req_buf, "X-Custom:", cust_h, sizeof(cust_h));
  extract_hdr(req_buf, "Cookie:", cook_h, sizeof(cook_h));
  extract_hdr(req_buf, "Content-Type:", ctype_h, sizeof(ctype_h));

  {
    char *after_hdr = hdr_end + 4;
    size_t have = used - (size_t)(after_hdr - req_buf);
    if (strstr(te_str, "chunked") != NULL) {
      /* Read until 0\r\n\r\n */
      while (strstr(after_hdr, "0\r\n\r\n") == NULL &&
             used + 1u < sizeof(req_buf)) {
        ssize_t r = recv(cfd, req_buf + used, sizeof(req_buf) - 1u - used, 0);
        if (r <= 0) {
          break;
        }
        used += (size_t)r;
        req_buf[used] = '\0';
      }
      /* Decode chunked stream */
      {
        const char *cp = after_hdr;
        while (*cp != '\0') {
          unsigned long csz = strtoul(cp, NULL, 16);
          const char *crlf = strstr(cp, "\r\n");
          if (crlf == NULL || csz == 0ul) {
            break;
          }
          cp = crlf + 2;
          if (body_len + csz < sizeof(body_buf)) {
            memcpy(body_buf + body_len, cp, (size_t)csz);
            body_len += (size_t)csz;
          }
          cp += csz + 2u;
        }
      }
    } else if (cl_str[0] != '\0') {
      size_t want = (size_t)strtoul(cl_str, NULL, 10);
      if (want > sizeof(body_buf) - 1u) {
        want = sizeof(body_buf) - 1u;
      }
      if (have > 0u) {
        size_t n0 = (have < want) ? have : want;
        memcpy(body_buf, after_hdr, n0);
        body_len = n0;
      }
      while (body_len < want) {
        ssize_t r = recv(cfd, body_buf + body_len, want - body_len, 0);
        if (r <= 0) {
          break;
        }
        body_len += (size_t)r;
      }
    }
    body_buf[body_len] = '\0';
  }

  /* Route dispatch */
  if (strncmp(path, "/ws_bad", 7) == 0) {
    const char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n"
                       "Connection: close\r\n\r\n";
    (void)send_all(cfd, resp, strlen(resp));
    return;
  }

  if (strncmp(path, "/ws", 3) == 0 && ws_key[0] != '\0') {
    char accept_key[32];
    char hs[256];
    unsigned char welcome_frame[12];
    unsigned char close_frame[4];
    compute_ws_accept(ws_key, accept_key);
    sprintf(hs,
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n\r\n",
            accept_key);
    (void)send_all(cfd, hs, strlen(hs));

    /* Text frame (FIN=1, opcode=1): "ws-welcome" (10 bytes) */
    welcome_frame[0] = 0x81u;
    welcome_frame[1] = 10u;
    memcpy(welcome_frame + 2, "ws-welcome", 10u);
    (void)send_all(cfd, welcome_frame, 12u);

    if (strstr(path, "echo=1") != NULL) {
      fd_set rfds;
      struct timeval tv;
      FD_ZERO(&rfds);
      FD_SET(cfd, &rfds);
      tv.tv_sec = 0;
      tv.tv_usec = 200000;
      if (select(cfd + 1, &rfds, NULL, NULL, &tv) > 0) {
        unsigned char in_f[256];
        ssize_t rn = recv(cfd, (char *)in_f, sizeof(in_f), 0);
        if (rn >= 6 && (in_f[0] & 0x0Fu) != 0x08u) {
          size_t plen = (size_t)(in_f[1] & 0x7Fu);
          int masked = (in_f[1] & 0x80u) != 0u;
          size_t hlen = masked ? 6u : 2u;
          if ((size_t)rn >= hlen + plen && plen <= 120u) {
            unsigned char out_f[128];
            size_t k;
            out_f[0] = (unsigned char)(0x80u | (in_f[0] & 0x0Fu));
            out_f[1] = (unsigned char)plen;
            for (k = 0u; k < plen; k++) {
              out_f[2u + k] =
                  masked ? (unsigned char)(in_f[6u + k] ^ in_f[2u + (k % 4u)])
                         : in_f[2u + k];
            }
            (void)send_all(cfd, out_f, 2u + plen);
          }
        }
      }
    }

    /* Clean Close frame (FIN=1, opcode=8, status 1000 = 0x03E8) */
    close_frame[0] = 0x88u;
    close_frame[1] = 2u;
    close_frame[2] = 0x03u;
    close_frame[3] = 0xE8u;
    (void)send_all(cfd, close_frame, 4u);
    return;
  }

  if (strncmp(path, "/sse", 4) == 0) {
    const char *sse_resp = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/event-stream\r\n"
                           "Cache-Control: no-cache\r\n"
                           "Connection: close\r\n\r\n"
                           "id: e1\n"
                           "event: tick\n"
                           "data: alpha\n\n"
                           "id: e2\n"
                           "data: omega\n\n";
    (void)send_all(cfd, sse_resp, strlen(sse_resp));
    return;
  }

  if (strcmp(path, "/empty") == 0) {
    const char *r = "HTTP/1.1 204 No Content\r\nContent-Length: 0\r\n"
                    "Connection: close\r\n\r\n";
    (void)send_all(cfd, r, strlen(r));
    return;
  }

  if (strcmp(path, "/chunked") == 0) {
    const char *r = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\n"
                    "Connection: close\r\n\r\n"
                    "5\r\nchunk\r\n6\r\nstream\r\n0\r\n\r\n";
    (void)send_all(cfd, r, strlen(r));
    return;
  }

  if (strcmp(path, "/redirect") == 0) {
    char r[256];
    sprintf(r,
            "HTTP/1.1 302 Found\r\n"
            "Location: http://127.0.0.1:%d/hello\r\n"
            "Content-Length: 0\r\n"
            "Connection: close\r\n\r\n",
            srv->port);
    (void)send_all(cfd, r, strlen(r));
    return;
  }

  if (strcmp(path, "/flaky") == 0) {
    srv->flaky_hits++;
    if (srv->flaky_hits == 1) {
      const char *r = "HTTP/1.1 503 Service Unavailable\r\n"
                      "Content-Length: 4\r\nConnection: close\r\n\r\nbusy";
      (void)send_all(cfd, r, strlen(r));
    } else {
      const char *r = "HTTP/1.1 200 OK\r\nContent-Length: 9\r\n"
                      "Connection: close\r\n\r\nRecovered";
      (void)send_all(cfd, r, strlen(r));
    }
    return;
  }

  if (strncmp(path, "/oauth/", 7) == 0) {
    const char *json = "{\"access_token\":\"tok_xyz\",\"token_type\":"
                       "\"Bearer\",\"active\":true}";
    char hdr[256];
    sprintf(hdr,
            "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n"
            "Content-Length: %lu\r\nConnection: close\r\n\r\n",
            (unsigned long)strlen(json));
    (void)send_all(cfd, hdr, strlen(hdr));
    (void)send_all(cfd, json, strlen(json));
    return;
  }

  if (strncmp(path, "/echo", 5) == 0) {
    char hdr[2048];
    sprintf(hdr,
            "HTTP/1.1 200 OK\r\n"
            "X-Echo-Method: %s\r\n"
            "X-Echo-Auth: %s\r\n"
            "X-Echo-Custom: %s\r\n"
            "X-Echo-Cookie: %s\r\n"
            "X-Echo-Type: %s\r\n"
            "Content-Length: %lu\r\n"
            "Connection: close\r\n\r\n",
            method, auth_h, cust_h, cook_h, ctype_h, (unsigned long)body_len);
    (void)send_all(cfd, hdr, strlen(hdr));
    if (body_len > 0u) {
      (void)send_all(cfd, body_buf, body_len);
    }
    return;
  }

  /* Default (/hello or any path): 200 OK "Hello FORTRAN" */
  {
    const char *payload = "Hello FORTRAN";
    const char *hdr = "HTTP/1.1 200 OK\r\n"
                      "X-Fah-Server: fortran-http\r\n"
                      "Set-Cookie: sid=abc123; Path=/\r\n"
                      "Content-Length: 13\r\n"
                      "Connection: close\r\n\r\n";
    (void)send_all(cfd, hdr, strlen(hdr));
    (void)send_all(cfd, payload, 13u);
  }
}

static void *server_loop(void *arg) {
  struct MockServerSlot *srv = (struct MockServerSlot *)arg;
  while (!srv->stop_flag) {
    fd_set rfds;
    struct timeval tv;
    int ready;
    FD_ZERO(&rfds);
    FD_SET(srv->listen_fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 50000;
    ready = select(srv->listen_fd + 1, &rfds, NULL, NULL, &tv);
    if (ready > 0 && !srv->stop_flag) {
      struct sockaddr_in cli_addr;
      socklen_t clen = (socklen_t)sizeof(cli_addr);
      int cfd = accept(srv->listen_fd, (struct sockaddr *)&cli_addr, &clen);
      if (cfd >= 0) {
        handle_client_fd(srv, cfd);
        (void)close(cfd);
      }
    }
  }
  return NULL;
}

int fah_mock_server_start(int *out_port) {
  int slot = -1;
  int i;
  int fd;
  int opt = 1;
  struct sockaddr_in addr;
  socklen_t alen = (socklen_t)sizeof(addr);

  if (out_port == NULL) {
    return -1;
  }
  for (i = 0; i < 8; i++) {
    if (!g_servers[i].active) {
      slot = i;
      break;
    }
  }
  if (slot < 0) {
    return -2;
  }

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    return -3;
  }
  (void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t)sizeof(opt));

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;

  if (bind(fd, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)) != 0 ||
      listen(fd, 32) != 0 ||
      getsockname(fd, (struct sockaddr *)&addr, &alen) != 0) {
    (void)close(fd);
    return -4;
  }

  g_servers[slot].active = 1;
  g_servers[slot].listen_fd = fd;
  g_servers[slot].port = (int)ntohs(addr.sin_port);
  g_servers[slot].stop_flag = 0;
  g_servers[slot].flaky_hits = 0;

  if (pthread_create(&g_servers[slot].thread, NULL, server_loop,
                     &g_servers[slot]) != 0) {
    g_servers[slot].active = 0;
    (void)close(fd);
    return -5;
  }

  *out_port = g_servers[slot].port;
  return 0;
}

int fah_mock_server_stop(int port) {
  int i;
  for (i = 0; i < 8; i++) {
    if (g_servers[i].active && g_servers[i].port == port) {
      g_servers[i].stop_flag = 1;
      (void)pthread_join(g_servers[i].thread, NULL);
      (void)close(g_servers[i].listen_fd);
      g_servers[i].active = 0;
      g_servers[i].port = 0;
      return 0;
    }
  }
  return 0;
}

static void *oauth_redirect_worker(void *arg) {
  int port = *(int *)arg;
  int attempt;
  struct timespec ts;
  free(arg);

  ts.tv_sec = 0;
  ts.tv_nsec = 15000000L; /* 15 ms */
  for (attempt = 0; attempt < 60; attempt++) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in dst;
    if (fd >= 0) {
      memset(&dst, 0, sizeof(dst));
      dst.sin_family = AF_INET;
      dst.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
      dst.sin_port = htons((unsigned short)port);
      if (connect(fd, (struct sockaddr *)&dst, (socklen_t)sizeof(dst)) == 0) {
        const char *req = "GET /cb?code=auth77&state=st99 HTTP/1.1\r\n"
                          "Host: 127.0.0.1\r\nConnection: close\r\n\r\n";
        char sink[256];
        (void)send_all(fd, req, strlen(req));
        (void)recv(fd, sink, sizeof(sink), 0);
        (void)close(fd);
        return NULL;
      }
      (void)close(fd);
    }
    (void)nanosleep(&ts, NULL);
  }
  return NULL;
}

int fah_mock_spawn_oauth_redirect_client(int *out_port) {
  int sfd;
  struct sockaddr_in addr;
  socklen_t alen = (socklen_t)sizeof(addr);
  int *heap_p;
  pthread_t th;

  if (out_port == NULL) {
    return -1;
  }
  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd < 0) {
    return -2;
  }
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  if (bind(sfd, (struct sockaddr *)&addr, (socklen_t)sizeof(addr)) != 0 ||
      getsockname(sfd, (struct sockaddr *)&addr, &alen) != 0) {
    (void)close(sfd);
    return -3;
  }
  *out_port = (int)ntohs(addr.sin_port);
  (void)close(sfd);

  heap_p = (int *)malloc(sizeof(int));
  if (heap_p == NULL) {
    return -4;
  }
  *heap_p = *out_port;
  if (pthread_create(&th, NULL, oauth_redirect_worker, heap_p) != 0) {
    free(heap_p);
    return -5;
  }
  (void)pthread_detach(th);
  return 0;
}

/* --- Fortran-callable shims --- */

int FAH_FC_GLOBAL(ihmkst, IHMKST)(int *out_port) {
  return fah_mock_server_start(out_port);
}

int FAH_FC_GLOBAL(ihmksp, IHMKSP)(const int *port) {
  return (port != NULL) ? fah_mock_server_stop(*port) : -1;
}

int FAH_FC_GLOBAL(ihmkul, IHMKUL)(const int *port, const int *route_id,
                                  const int *mode, int *out_arr,
                                  int *out_nchars) {
  char url[128];
  const char *sfx = "/hello";
  const char *scheme = "http";
  size_t ulen;

  if (port == NULL || route_id == NULL || mode == NULL || out_arr == NULL ||
      out_nchars == NULL) {
    return -1;
  }
  switch (*route_id) {
  case 0:
    sfx = "/hello";
    break;
  case 1:
    sfx = "/echo";
    break;
  case 2:
    sfx = "/chunked";
    break;
  case 3:
    scheme = "ws";
    sfx = "/ws";
    break;
  case 4:
    sfx = "/sse";
    break;
  case 5:
    sfx = "/empty";
    break;
  case 6:
    sfx = "/redirect";
    break;
  case 7:
    sfx = "/flaky";
    break;
  case 8:
    sfx = "/oauth/token";
    break;
  default:
    sfx = "/hello";
    break;
  }
  sprintf(url, "%s://127.0.0.1:%d%s", scheme, *port, sfx);
  ulen = strlen(url);
  if (*mode == 0) {
    return (int)fah_cbytes_to_icodes(url, ulen, out_arr, 128, out_nchars);
  }
  return (int)fah_cbytes_to_hollerith4(url, ulen, out_arr, 32, out_nchars);
}

void FAH_FC_GLOBAL(fhmks7, FHMKS7)(const int *port, const char *path,
                                   char *out_url, int *out_len,
                                   fah_charlen_t plen, fah_charlen_t ulen) {
  char *cpath = NULL;
  char url[256];
  size_t actual = 0u;
  FAH_IGNORE_RC(fah_fstr_to_cstr(path, (size_t)plen, 1, &cpath));
  if (cpath != NULL && strncmp(cpath, "ws:", 3) == 0) {
    sprintf(url, "ws://127.0.0.1:%d%s", *port, cpath + 3);
  } else {
    sprintf(url, "http://127.0.0.1:%d%s", *port,
            (cpath != NULL) ? cpath : "/hello");
  }
  free(cpath);
  FAH_IGNORE_RC(
      fah_cbytes_to_fstr(url, strlen(url), out_url, (size_t)ulen, &actual, 1));
  *out_len = (int)actual;
}

void FAH_FC_GLOBAL(fhmkoa, FHMKOA)(int *out_port, int *out_err) {
  *out_err = fah_mock_spawn_oauth_redirect_client(out_port);
}
