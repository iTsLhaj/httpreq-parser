#include "HTTPRequestParser.h"



int main() {

    std::string const raw_http_request =    "POST /login.php HTTP/1.1\r\n" \
                                            "Host: localhost\r\n" \
                                            "User-Agent: meowsurf/1.0\r\n" \
                                            "Accept: text/html\r\n" \
                                            "Content-Length: 43\r\n" \
                                            "Connection: close\r\n\r\n" \
                                            "{\"username\": \"lhaj\", \"password\": \"meow23$\"}\r\n";
    HTTPRequestParser parser(raw_http_request);

    std::cout << parser.getRequest() << std::endl;
    std::cout << std::endl << parser.getRequest().getBody() << std::endl;

}
