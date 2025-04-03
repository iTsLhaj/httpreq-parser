#ifndef HTTPREQUESTPARSER_H
#define HTTPREQUESTPARSER_H

#include "HTTPRequest.h"
#include <sstream>

#define SP  0x0020
#define CR  0x000D
#define LF  0x000A



class HTTPRequestParser {

private:
    enum State {
        p_REQUEST_LINE,
        p_REQUEST_LINE_METHOD,
        p_REQUEST_LINE_METHOD_SP,
        p_REQUEST_LINE_URI,
        p_REQUEST_LINE_URI_QUERY,
        p_REQUEST_LINE_URI_SP,
        p_REQUEST_LINE_VER_H,
        p_REQUEST_LINE_VER_HT,
        p_REQUEST_LINE_VER_HTT,
        p_REQUEST_LINE_VER_HTTP,
        p_REQUEST_LINE_VER_HTTP_SLASH,
        p_REQUEST_LINE_VER_HTTP_MAJOR,
        p_REQUEST_LINE_VER_HTTP_DOT,
        p_REQUEST_LINE_VER_HTTP_MINOR,
        p_REQUEST_LINE_CR,
        p_REQUEST_LINE_LF,
        p_HEADERS_FIELD_START,
        p_HEADERS_FIELD_NAME,
        p_HEADERS_FIELD_VALUE,
        p_HEADERS_FIELD_CR,
        p_HEADERS_FIELD_LF,
        p_ISOLATION_CR,
        p_ISOLATION_LF,
        p_READ_BODY,
        p_READ_CHUNKED_LENGHT,
        p_CHUNKED_LENGHT,
        p_CHUNK_LENGHT_CR,
        p_CHUNK_LENGHT_LF,
        p_CHUNKED_IGN,
        p_CHUNKED_CR,
        p_CHUNKED_LF,
        p_READ_CHUNKED_DATA,
        p_CHUNKED_DATA_CR,
        p_CHUNKED_DATA_LF
    };

    std::size_t _body_length;
    HTTPRequest _request;
    State _state;

    std::string _chunked_body;

public:
    HTTPRequestParser(std::string const& raw_data);
    HTTPRequestParser(HTTPRequestParser const& src);
    ~HTTPRequestParser();

    HTTPRequestParser& operator=(HTTPRequestParser const& src);

    HTTPRequest const& getRequest() const;

private:
    void feed(std::string const& raw_data);

    static std::string decode(std::string const& encoded);
    static bool verify_method_string(std::string const& method);

};



#endif
