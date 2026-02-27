add_library(NetworkTools)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED)

target_sources(NetworkTools PUBLIC FILE_SET CXX_MODULES FILES
        IHttpClient.ixx
        BoostBeastHttpClient/BoostBeastHttpClient.ixx
        UrlParser.ixx
        HttpClientFactory.ixx
)

target_sources(NetworkTools PRIVATE
        BoostBeastHttpClient/BoostBeastHttpClientRegistration.cpp
)

target_link_libraries(NetworkTools PUBLIC Boost::headers OpenSSL::SSL OpenSSL::Crypto Logger)

target_include_directories(NetworkTools
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)