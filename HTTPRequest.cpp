#include "HTTPRequest.h"



HTTPRequest::HTTPRequest()
: _method(m_None),
  _headers(),
  _body() {
}

HTTPRequest::HTTPRequest(HTTPRequest const &src) {
  *this = src;
}

HTTPRequest::~HTTPRequest() {
}

HTTPRequest& HTTPRequest::operator=(HTTPRequest const &rhs) {
  if (this != &rhs) {
    this->_method = rhs._method;
    this->_headers = rhs._headers;
    this->_body = rhs._body;
  }
  return *this;
}


HTTPRequestMethod const& HTTPRequest::getMethod() const {
  return _method;
}

std::string const& HTTPRequest::getPath() const {
  return _path;
}

std::string const& HTTPRequest::getQuery() const {
  return _query;
}

IHTTPRequest::HTTPHeaderMap const& HTTPRequest::getHeaders() const {
  return _headers;
}

std::string const& HTTPRequest::getHeader(std::string const& k) {
  static const std::string empty;
  IHTTPRequest::HTTPHeaderMap::const_iterator const it = _headers.find(k);
  return (it != _headers.end()) ? it->second : empty;
}

std::string const& HTTPRequest::getBody() const {
  return _body;
}

void HTTPRequest::setMethod(HTTPRequestMethod const& m) {
  this->_method = m;
}

void HTTPRequest::setPath(std::string const& p) {
  /// @todo perform a url-decode before setting the path attrib !
  _path = p;
}

void HTTPRequest::setQuery(std::string const& q) {
  /// @todo perform a url-decode before setting the query attrib !
  _query = q;
}

void HTTPRequest::setHeaders(HTTPHeaderMap const &headers) {
  this->_headers = headers;
}

void HTTPRequest::setHeader(std::string const &k, std::string const &v) {
  std::string key = k;
  std::string value = v;
  std::size_t start = v.find_first_not_of(" \t");
  std::size_t end = v.find_last_not_of(" \t");

  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  if (start == std::string::npos) {
    value.clear();
  } else {
    value = value.substr(start, end - start + 1);
  }
  this->_headers[key] = value;
}

void HTTPRequest::setBody(std::string const &body) {
  /// @todo perform a url-decode before setting the body !
  this->_body = body;
}

HTTPRequestMethod HTTPRequest::getMethod(std::string const &method) {
  if (method == "GET")  return m_GET;
  else if (method == "POST") return m_POST;
  else if (method == "DELETE") return m_DELETE;
  else return InvalidMethod;
}

std::string HTTPRequest::getMethod(HTTPRequestMethod const& method) {
  switch (method) {
    case m_GET:          return "GET";
    case m_POST:         return "POST";
    case m_DELETE:       return "DELETE";
    case InvalidMethod:  return "InvalidMethod";
    case m_None:         return "None";
  }
  return "";
}

std::ostream& operator<<(std::ostream& os, HTTPRequest const& req) {
  IHTTPRequest::HTTPHeaderMap::const_iterator it = req.getHeaders().begin();

  os << "Method : " << req.getMethod() << " [" << HTTPRequest::getMethod(req.getMethod()) << "]" << std::endl;
  os << "Headers: " << std::endl;
  for (; it != req.getHeaders().end(); ++it) {
    os  << "\t - ["
        << it->first
        << "]: ["
        << it->second
        << "]" << std::endl;
  }
  return os;
}
