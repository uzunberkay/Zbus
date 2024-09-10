cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(tr)



target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/coap)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/workqueue)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/tim)
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/adc_t)

target_sources(app PRIVATE src/coap/coap_t.c)
target_sources(app PRIVATE src/workqueue/wq.c)
target_sources(app PRIVATE src/tim/tim_t.c)
target_sources(app PRIVATE src/adc_t/adc_t.c)
