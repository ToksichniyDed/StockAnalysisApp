add_library(NetworkTools)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS beast)

target_link_libraries(NetworkTools PRIVATE Boost::headers OpenSSL::SSL OpenSSL::Crypto Logger)

target_sources(NetworkTools PUBLIC FILE_SET CXX_MODULES FILES
        IHttpClient.ixx
        BoostBeastHttpClient.ixx
)