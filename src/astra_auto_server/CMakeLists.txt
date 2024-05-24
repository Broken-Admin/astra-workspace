cmake_minimum_required(VERSION 3.8)
project(astra_auto_server)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(astra_auto_interfaces REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

add_executable(navigate_rover_server_node src/auto_server.cpp)
ament_target_dependencies(navigate_rover_server_node
  "rclcpp" 
  "rclcpp_action" 
  "astra_auto_interfaces"
)

install(TARGETS
  navigate_rover_server_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
