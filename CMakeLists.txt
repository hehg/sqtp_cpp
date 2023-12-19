cmake_minimum_required(VERSION 3.2)
project(sqtp)

option(build_test        "build  test code"    on)
option(build_app         "build  app code"     off)

add_subdirectory(libsqtp)
add_subdirectory(libsqbz)
add_subdirectory(liboffer)
add_subdirectory(libframe)

#app
if(build_app)
    add_subdirectory(app_trade_service)
    #add_subdirectory(app_client_ui)

    if(WIN32)
    #add_subdirectory(app_client_ui)
    endif(WIN32)
endif()



#test
if(build_test)
    add_subdirectory(libsqbz/test)
    add_subdirectory(libsqtp/test)
    add_subdirectory(libsqtp/example)
endif()

