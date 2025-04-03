#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "IHTTPRequest.h"



class HTTPRequest : public IHTTPRequest {

private:
    HTTPRequestMethod _method;
    std::string _path;
    std::string _query;
    HTTPHeaderMap _headers;
    std::string _body;

public:
    HTTPRequest();
    HTTPRequest(HTTPRequest const& src);
    ~HTTPRequest();

    HTTPRequest& operator=(HTTPRequest const& rhs);

    HTTPRequestMethod const& getMethod() const;
    std::string const& getPath() const;
    std::string const& getQuery() const;
    HTTPHeaderMap const& getHeaders() const;
    std::string const& getHeader(std::string const& k);
    std::string const& getBody() const;
    void setMethod(HTTPRequestMethod const& m);
    void setPath(std::string const& p);
    void setQuery(std::string const& q);
    void setHeaders(HTTPHeaderMap const& headers);
    void setHeader(std::string const& k, std::string const& v);
    void setBody(std::string const& body);

    static HTTPRequestMethod getMethod(std::string const& method);
    static std::string getMethod(HTTPRequestMethod const& method);

};

std::ostream& operator<<(std::ostream& os, HTTPRequest const& req);



#endif
