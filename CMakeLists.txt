cmake_minimum_required(VERSION 3.30)
project(http_request_parser)

set(CMAKE_CXX_STANDARD 98)

add_executable(http_request_parser
        IHTTPRequest.cpp
        IHTTPRequest.h
        HTTPRequestMethod.h
        HTTPRequest.cpp
        HTTPRequest.h
        HTTPRequestParser.cpp
        HTTPRequestParser.h
        main.cpp)
