cmake_minimum_required(VERSION 3.8)
project(lab01_ur3_letter)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(shape_msgs REQUIRED)
find_package(moveit_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)

# Đảm bảo biên dịch lại letter_v_node
add_executable(letter_v_node src/letter_v_node.cpp)
ament_target_dependencies(letter_v_node
  rclcpp
  moveit_ros_planning_interface
  geometry_msgs
  shape_msgs
  moveit_msgs
)

install(TARGETS
  letter_v_node
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY launch rviz DESTINATION share/${PROJECT_NAME})
ament_package()