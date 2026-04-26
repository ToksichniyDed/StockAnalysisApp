set(NETWORK_TOOLS_HEADERS
        IHttpClient.h
        UrlParser.h
        HttpClientFactory.h

        BoostBeast/BoostBeastHttpClient.h
)

set(NETWORK_TOOLS_SOURCES
        HttpClientFactory.cpp

        BoostBeast/BoostBeastHttpClient.cpp
        BoostBeast/BoostBeastHttpClientRegistration.h
)

add_library(NetworkTools
        ${NETWORK_TOOLS_HEADERS}
        ${NETWORK_TOOLS_SOURCES}
)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)

#target_sources(NetworkTools PUBLIC FILE_SET CXX_MODULES FILES
#        IHttpClient.ixx
#        BoostBeastHttpClient/BoostBeastHttpClient.ixx
#        UrlParser.ixx
#        HttpClientFactory.ixx
#)

target_link_libraries(NetworkTools PUBLIC Boost::headers OpenSSL::SSL OpenSSL::Crypto Logger)

target_include_directories(NetworkTools
        PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)