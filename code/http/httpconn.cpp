#include "httpconn.h"
namespace MicroWS {
const char *HttpConn::srcDir;
std::atomic<int> HttpConn::userCount;
bool HttpConn::isET;

HttpConn::HttpConn() {
  fd_ = 1;
  addr_ = {0};
  isClose_ = true;
}

HttpConn::~HttpConn() { Close(); }
} // namespace MicroWS