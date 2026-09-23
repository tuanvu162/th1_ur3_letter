#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/msg/pose.hpp>
#include <iostream>
#include <thread>

class TestPoseNode : public rclcpp::Node {
public:
  TestPoseNode(const rclcpp::NodeOptions & options)
  : Node("test_pose_node", options) {}

  void run() {
    RCLCPP_INFO(this->get_logger(), "Đang kết nối MoveGroupInterface...");
    auto move_group = moveit::planning_interface::MoveGroupInterface(shared_from_this(), "ur_manipulator");
    
    // Giảm tốc độ xuống 10% cho an toàn
    move_group.setMaxVelocityScalingFactor(0.1);
    move_group.setMaxAccelerationScalingFactor(0.1);

    while (rclcpp::ok()) {
      double x, y, z;
      std::cout << "\n============================================\n";
      std::cout << "Nhập tọa độ X Y Z (cách nhau bởi khoảng trắng): ";
      
      if (!(std::cin >> x >> y >> z)) {
        break; // Thoát nếu nhập sai định dạng hoặc bấm Ctrl+C
      }

      geometry_msgs::msg::Pose target_pose;
      target_pose.position.x = x;
      target_pose.position.y = y;
      target_pose.position.z = z;
      
      // Giữ nguyên góc cắm mỏ hàn vuông góc xuống đất như bài Lab
      target_pose.orientation.w = 0.0;
      target_pose.orientation.x = 0.0;
      target_pose.orientation.y = 1.0;
      target_pose.orientation.z = 0.0;

      move_group.setPoseTarget(target_pose);
      moveit::planning_interface::MoveGroupInterface::Plan plan;

      RCLCPP_INFO(this->get_logger(), "Đang tính toán quỹ đạo tới [%.3f, %.3f, %.3f]...", x, y, z);
      
      if (move_group.plan(plan) == moveit::core::MoveItErrorCode::SUCCESS) {
        RCLCPP_INFO(this->get_logger(), "Đường đi hợp lệ! Đang di chuyển...");
        move_group.execute(plan);
      } else {
        RCLCPP_ERROR(this->get_logger(), "LỖI: Điểm này nằm ngoài tầm với hoặc gây va chạm (kẹt sàn)!");
      }
    }
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  node_options.automatically_declare_parameters_from_overrides(true);
  auto node = std::make_shared<TestPoseNode>(node_options);

  std::thread run_thread([node]() {
    rclcpp::sleep_for(std::chrono::seconds(2));
    node->run();
  });

  rclcpp::spin(node);
  run_thread.join();
  rclcpp::shutdown();
  return 0;
}