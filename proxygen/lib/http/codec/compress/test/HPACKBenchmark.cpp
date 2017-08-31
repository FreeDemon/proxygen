/*
 *  Copyright (c) 2017-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#include <proxygen/lib/http/codec/compress/test/TestUtil.h>
#include <folly/Benchmark.h>
#include <folly/Range.h>

#include <algorithm>

using namespace std;
using namespace folly;
using namespace proxygen;
using proxygen::HPACKHeader;

void encodeDecode(vector<HPACKHeader>& headers, HPACKEncoder& encoder,
                  HPACKDecoder& decoder) {
  unique_ptr<IOBuf> encoded = encoder.encode(headers);
  auto decodedHeaders = decoder.decode(encoded.get());
  CHECK(!decoder.hasError());
}


vector<HPACKHeader> getHeaders() {
  vector<HPACKHeader> headers;
  headers.emplace_back(":authority", "www.facebook.com");
  headers.emplace_back(":method", "GET");
  headers.emplace_back(":path", "/graphql");
  headers.emplace_back(":scheme", "https");
  headers.emplace_back(
    "user-agent",
    "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 "
    "(KHTML, like Gecko) Chrome/60.0.3100.0 Safari/537.36");
  headers.emplace_back("accept-encoding", "gzip, deflate, br");
  headers.emplace_back("accept-language", "en-US,en;q=0.8");
  headers.emplace_back(
    "accept",
    "text/html,application/xhtml+xml,application/xml;q=0.9,image/"
    "webp,image/apng,*/*;q=0.8");
  return headers;
}

namespace {
static vector<HPACKHeader> headers = getHeaders();
}

void encodeDecodeBench(int reencodes, int iters) {
  for (int i = 0; i < iters; i++) {
    HPACKEncoder encoder(true);
    HPACKDecoder decoder;
    encodeDecode(headers, encoder, decoder);
    for (int j = 0; j < reencodes; j++) {
      encodeDecode(headers, encoder, decoder);
    }
  }
}


BENCHMARK(EncodeDecode, iters) {
  encodeDecodeBench(0, iters);
}

BENCHMARK(EncodeDecode1, iters) {
  encodeDecodeBench(1, iters);
}

BENCHMARK(EncodeDecode2, iters) {
  encodeDecodeBench(2, iters);
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  folly::runBenchmarks();
  return 0;
}
