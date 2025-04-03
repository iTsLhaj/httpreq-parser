#ifndef IHTTPREQUEST_H
#define IHTTPREQUEST_H

#include <iostream>
#include <map>

#include "HTTPRequestMethod.h"



class IHTTPRequest {

protected:
    IHTTPRequest();
    IHTTPRequest(IHTTPRequest const&);
    IHTTPRequest& operator=(IHTTPRequest const&);

public:
    typedef std::map<std::string, std::string> HTTPHeaderMap;

    virtual ~IHTTPRequest();
    virtual HTTPRequestMethod const& getMethod() const = 0;
    virtual std::string const& getPath() const = 0;
    virtual std::string const& getQuery() const = 0;
    virtual HTTPHeaderMap const& getHeaders() const = 0;
    virtual std::string const& getBody() const = 0;
    virtual void setMethod(HTTPRequestMethod const& m) = 0;
    virtual void setPath(std::string const& path) = 0;
    virtual void setQuery(std::string const& quer) = 0;
    virtual void setHeaders(HTTPHeaderMap const& headers) = 0;
    virtual void setHeader(std::string const& k, std::string const& v) = 0;
    virtual void setBody(std::string const& body) = 0;

};



#endif
