#include "HTTPRequestParser.h"



HTTPRequestParser::HTTPRequestParser(std::string const& raw_data) {
    this->_state = p_REQUEST_LINE;
    this->feed(raw_data);
}

HTTPRequestParser::HTTPRequestParser(HTTPRequestParser const &src) {
    *this = src;
}

HTTPRequestParser::~HTTPRequestParser() {
}

HTTPRequestParser& HTTPRequestParser::operator=(HTTPRequestParser const& src) {
    if (this != &src) {
        this->_request = src._request;
        this->_state = src._state;
    }
    return *this;
}

bool HTTPRequestParser::verify_method_string(std::string const& method) {
    std::string supported_methods[3] = {
        "GET",
        "POST",
        "DELETE"
    };
    std::size_t error_counter = 0;

    for (std::size_t i = 0; i < 3; ++i) {
        if (method.compare(supported_methods[i])) {
            error_counter++;
        }
    }

    return error_counter == 3;
}

/**
 * @note what about making things a bit fast, don't rely on chatGPT but unlock you're potential !!!
 * @note we can do this trust me comrade 💪 .
 */
void HTTPRequestParser::feed(std::string const& raw_data) {

    std::string method_str;
    std::string key_storage;
    std::string value_storage;
    std::stringstream s;
    std::size_t chunk_length = 0;

    for (std::size_t i = 0; i < raw_data.size(); ++i) {
        switch (this->_state) {
            case p_REQUEST_LINE: {
                std::size_t space_pos = raw_data.find_first_of(SP);
                method_str = raw_data.substr(i, space_pos);
                if (HTTPRequestParser::verify_method_string(method_str)) {
                    throw std::runtime_error("HTTPRequestParser: Invalid HTTP method");
                }
                this->_state = p_REQUEST_LINE_METHOD;
                i = space_pos - 2;
                break;
            }
            case p_REQUEST_LINE_METHOD: {
                this->_request.setMethod(HTTPRequest::getMethod(method_str));
                this->_state = p_REQUEST_LINE_METHOD_SP;
                break;
            }
            case p_REQUEST_LINE_METHOD_SP: {
                if (raw_data[i] != SP) {
                    throw std::runtime_error("HTTPRequestParser: Invalid HTTP RequestLine ` ` is expected");
                }
                this->_state = p_REQUEST_LINE_URI;
                break;
            }
            case p_REQUEST_LINE_URI: {
                if (raw_data[i++] != '/') {
                    throw std::runtime_error("HTTPRequestParser: Invalid HTTP RequestLine `/` is expected");
                }
                std::string full_uri = raw_data.substr(
                    i, raw_data.find_first_of(SP, i) - i);
                this->_request.setPath(full_uri);
                if (full_uri.find('?') == std::string::npos) {
                    this->_state = p_REQUEST_LINE_URI_SP;
                } else {
                    this->_state = p_REQUEST_LINE_URI_QUERY;
                }
                i += full_uri.size() - 1;
                break;
            }
            case p_REQUEST_LINE_URI_QUERY: {
                int j = -1;
                int c = 0;
                std::string full_uri = this->_request.getPath();
                while (++j < full_uri.size()) {
                    if (full_uri[j] == '?') {
                        c++;
                    }
                }
                if (c != 1) {
                    throw std::runtime_error("HTTPRequestParser: Invalid Query `?` is duplicated");
                }
                std::string path = full_uri.substr(0, full_uri.find('?'));
                this->_request.setPath(path);
                this->_request.setQuery(
                    full_uri.substr(path.size() + 1,
                        full_uri.size() - path.size()));
                i--;
                this->_state = p_REQUEST_LINE_URI_SP;
                break;
            }
            case p_REQUEST_LINE_URI_SP: {
                if (raw_data[i] != SP) {
                    throw std::runtime_error("HTTPRequestParser: Malformed RequestLine ` ` expected");
                }
                this->_state = p_REQUEST_LINE_VER_H;
                break;
            }
            case p_REQUEST_LINE_VER_H: {
                if (raw_data[i] != 'H') {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HT;
                break;
            }
            case p_REQUEST_LINE_VER_HT: {
                if (raw_data[i] != 'T') {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HTT;
                break;
            }
            case p_REQUEST_LINE_VER_HTT: {
                if (raw_data[i] != 'T') {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HTTP;
                break;
            }
            case p_REQUEST_LINE_VER_HTTP: {
                if (raw_data[i] != 'P') {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HTTP_SLASH;
                break;
            }
            case p_REQUEST_LINE_VER_HTTP_SLASH: {
                if (raw_data[i] != '/') {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HTTP_MAJOR;
                break;
            }
            case p_REQUEST_LINE_VER_HTTP_MAJOR: {
                if (!std::isdigit(raw_data[i])) {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HTTP_DOT;
                break;
            }
            case p_REQUEST_LINE_VER_HTTP_DOT: {
                if (raw_data[i] != '.') {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_VER_HTTP_MINOR;
                break;
            }
            case p_REQUEST_LINE_VER_HTTP_MINOR: {
                if (!std::isdigit(raw_data[i])) {
                    throw std::runtime_error("HTTPRequestParser: Invalid Request Line HTTP Version.");
                }
                this->_state = p_REQUEST_LINE_CR;
                break;
            }
            case p_REQUEST_LINE_CR: {
                if (raw_data[i] != CR) {
                    throw std::runtime_error("HTTPRequestParser: Malormed Request Line `\\r` expected");
                }
                this->_state = p_REQUEST_LINE_LF;
                break;
            }
            case p_REQUEST_LINE_LF: {
                if (raw_data[i] != LF) {
                    throw std::runtime_error("HTTPRequestParser: Malormed Request Line `\\n` expected");
                }
                this->_state = p_HEADERS_FIELD_START;
                break;
            }
            case p_HEADERS_FIELD_START: {
                /// @note "A client MUST send a Host header field in all HTTP/1.1 request messages." — RFC 7230, Section 5.4
                if (!key_storage.empty() && !value_storage.empty()) {
                    this->_request.setHeader(key_storage, value_storage);
                    key_storage.clear();
                    value_storage.clear();
                }
                if (raw_data[i] == CR) {
                    i--;
                    this->_state = p_HEADERS_FIELD_CR;
                    break;
                }
                this->_state = p_HEADERS_FIELD_NAME;
            }
            case p_HEADERS_FIELD_NAME: {
                if (raw_data[i] == ':') {
                    i++;
                    this->_state = p_HEADERS_FIELD_VALUE;
                    break;
                }
                key_storage.push_back(raw_data[i]);
                break;
            }
            case p_HEADERS_FIELD_VALUE: {
                if (raw_data[i] == CR) {
                    i--;
                    this->_state = p_HEADERS_FIELD_CR;
                    break;
                }
                value_storage.push_back(raw_data[i]);
                break;
            }
            case p_HEADERS_FIELD_CR: {
                if (raw_data[i] != CR) {
                    throw std::runtime_error("HTTPRequestParser: Malformed Header Field End `\\r` expected");
                }
                this->_state = p_HEADERS_FIELD_LF;
                break;
            }
            case p_HEADERS_FIELD_LF: {
                if (raw_data[i] != LF) {
                    throw std::runtime_error("HTTPRequestParser: Malformed Header Field End `\\n` expected");
                }
                this->_state = p_ISOLATION_CR;
                break;
            }
            case p_ISOLATION_CR: {
                if (raw_data[i] != CR) {
                    i--;
                    this->_state = p_HEADERS_FIELD_START;
                } else {
                    this->_state = p_ISOLATION_LF;
                }
                break;
            }
            case p_ISOLATION_LF: {
                if (raw_data[i] != LF) {
                    throw std::runtime_error("HTTPRequestParser: things have to be isolated for a while, it has to be that way it\'s for you\'re own sake. just leave !!! go to the woods go to the mountains, unite with the nature, escape this rotten capitalist society, embrace isolation. so as you can see a `\\n` is expected !");
                }
                if (!this->_request.getHeader("transfer-encoding").empty()) {
                    if (this->_request.getHeader("transfer-encoding") == "chunked") {
                        this->_state = p_READ_CHUNKED_LENGHT;
                        break;
                    }
                }
                if (!this->_request.getHeader("content-length").empty()) {
                    this->_body_length = std::atoi(
                        this->_request.getHeader("content-length").c_str());
                    this->_state = p_READ_BODY;
                    break;
                }
                return;
            }
            case p_READ_BODY: {
                this->_request.setBody(raw_data.substr(i, this->_body_length));
                return;
            }
            case p_READ_CHUNKED_LENGHT: {
                if (!std::isxdigit(raw_data[i])) {
                    throw std::runtime_error("HTTPRequestParser: chunked body lenght malformed !");
                }
                s.str("");
                s.clear();
                s << raw_data[i];
                s >> std::hex >> chunk_length;
                if (chunk_length == 0) {
                    this->_state = p_CHUNK_LENGHT_CR;
                } else {
                    this->_state = p_CHUNKED_LENGHT;
                }
                break;
            }
            case p_CHUNKED_LENGHT: {
                if (!std::isxdigit(raw_data[i])) {
                    if (raw_data[i] == CR) {
                        this->_state = p_CHUNK_LENGHT_LF;
                        break;
                    }
                    this->_state = p_CHUNKED_IGN;
                    break;
                }
                int tmp = 0;
                s.str("");
                s.clear();
                s << raw_data[i];
                s >> std::hex >> tmp;
                chunk_length *= 16;
                chunk_length += tmp;
                break;
            }
            case p_CHUNK_LENGHT_CR: {
                if (raw_data[i] == CR) {
                    this->_state = p_CHUNK_LENGHT_LF;
                } else {
                    throw std::runtime_error("HTTPRequestParser: malformed http body !");
                }
                break;
            }
            case p_CHUNK_LENGHT_LF: {
                if (raw_data[i] == LF) {
                    if (chunk_length == 0) {
                        this->_state = p_CHUNKED_CR;
                    } else {
                        this->_state = p_READ_CHUNKED_DATA;
                    }
                } else {
                    throw std::runtime_error("HTTPRequestParser: malformed http body `\\n` is expected !");
                }
                break;
            }
            case p_CHUNKED_IGN: {
                if (raw_data[i] == CR) {
                    this->_state = p_CHUNK_LENGHT_LF;
                }
                break;
            }
            case p_READ_CHUNKED_DATA: {
                this->_chunked_body.push_back(raw_data[i]);
                chunk_length--;
                if (chunk_length == 0) {
                    this->_state = p_CHUNKED_DATA_CR;
                }
                break;
            }
            case p_CHUNKED_DATA_CR: {
                if (raw_data[i] == CR) {
                    this->_state = p_CHUNKED_DATA_LF;
                } else {
                    throw std::runtime_error("HTTPRequestParser: malformed chunked body `\\r` is expected !");
                }
                break;
            }
            case p_CHUNKED_DATA_LF: {
                if (raw_data[i] == LF) {
                    this->_state = p_READ_CHUNKED_LENGHT;
                } else {
                    throw std::runtime_error("HTTPRequestParser: malformed chunked body `\\n` is expected !");
                }
                break;
            }
            case p_CHUNKED_CR: {
                if (raw_data[i] != CR) {
                    throw std::runtime_error("HTTPRequestParser: malformed terminating chunked body `\\r` is expected !");
                }
                this->_state = p_CHUNKED_LF;
                break;
            }
            case p_CHUNKED_LF: {
                if (raw_data[i] != LF) {
                    throw std::runtime_error("HTTPRequestParser: malformed terminating chunked body `\\n` is expected !");
                }
                this->_request.setBody(this->_chunked_body);
                return;
            }
        }
    }

}

std::string HTTPRequestParser::decode(std::string const &encoded) {
    std::string decoded;
    decoded.reserve(encoded.size());
    for (std::size_t i = 0; i < encoded.size(); i++) {
        if (encoded[i] == '%' && i + 2 < encoded.size()) {
            char hex[3];
            hex[0] = encoded[i + 1];
            hex[1] = encoded[i + 2];
            hex[2] = '\0';
            int ch = std::strtol(hex, NULL, 16);
            decoded.push_back(static_cast<char>(ch));
            i += 2;
        } else if (encoded[i] == '+') {
            decoded.push_back(' ');
        } else {
            decoded.push_back(encoded[i]);
        }
    }
    return decoded;
}


HTTPRequest const& HTTPRequestParser::getRequest() const {
    return this->_request;
}
