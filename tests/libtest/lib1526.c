/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Vijay Panghal, <vpanghal@maginatics.com>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * SPDX-License-Identifier: curl
 *
 ***************************************************************************/

/*
 * This unit test PUT http data over proxy. Proxy header will be different
 * from server http header
 */

#include "first.h"

#include "memdebug.h"

static const char t1526_testdata[] = "Hello Cloud!\n";

static size_t t1526_read_cb(char *ptr, size_t size, size_t nmemb, void *stream)
{
  size_t  amount = nmemb * size; /* Total bytes curl wants */
  if(amount < strlen(t1526_testdata)) {
    return strlen(t1526_testdata);
  }
  (void)stream;
  memcpy(ptr, t1526_testdata, strlen(t1526_testdata));
  return strlen(t1526_testdata);
}

static CURLcode test_lib1526(const char *URL)
{
  CURL *curl = NULL;
  CURLcode res = CURLE_FAILED_INIT;
  /* http and proxy header list */
  struct curl_slist *hhl = NULL, *phl = NULL, *tmp = NULL;

  if(curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
    curl_mfprintf(stderr, "curl_global_init() failed\n");
    return TEST_ERR_MAJOR_BAD;
  }

  curl = curl_easy_init();
  if(!curl) {
    curl_mfprintf(stderr, "curl_easy_init() failed\n");
    curl_global_cleanup();
    return TEST_ERR_MAJOR_BAD;
  }

  hhl = curl_slist_append(hhl, "User-Agent: Http Agent");
  phl = curl_slist_append(phl, "User-Agent: Proxy Agent");
  if(!hhl || !phl) {
    goto test_cleanup;
  }
  tmp = curl_slist_append(phl, "Expect:");
  if(!tmp) {
    goto test_cleanup;
  }
  phl = tmp;

  test_setopt(curl, CURLOPT_URL, URL);
  test_setopt(curl, CURLOPT_PROXY, libtest_arg2);
  test_setopt(curl, CURLOPT_HTTPHEADER, hhl);
  test_setopt(curl, CURLOPT_PROXYHEADER, phl);
  test_setopt(curl, CURLOPT_HEADEROPT, CURLHEADER_SEPARATE);
  test_setopt(curl, CURLOPT_POST, 0L);
  test_setopt(curl, CURLOPT_UPLOAD, 1L);
  test_setopt(curl, CURLOPT_VERBOSE, 1L);
  test_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
  test_setopt(curl, CURLOPT_HEADER, 1L);
  test_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
  test_setopt(curl, CURLOPT_READFUNCTION, t1526_read_cb);
  test_setopt(curl, CURLOPT_HTTPPROXYTUNNEL, 1L);
  test_setopt(curl, CURLOPT_INFILESIZE, (long)strlen(t1526_testdata));

  res = curl_easy_perform(curl);

test_cleanup:

  curl_easy_cleanup(curl);

  curl_slist_free_all(hhl);

  curl_slist_free_all(phl);

  curl_global_cleanup();

  return res;
}
