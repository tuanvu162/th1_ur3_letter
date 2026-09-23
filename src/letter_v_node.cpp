#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <geometry_msgs/msg/pose.hpp>
#include <shape_msgs/msg/solid_primitive.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <vector>
#include <thread>
#include <chrono>

class LetterVWriterNode : public rclcpp::Node {
public:
  LetterVWriterNode(const rclcpp::NodeOptions & options)
  : Node("letter_v_writer", options) {
    // Khởi tạo ngòi bút ảo (Publisher vẽ nét trên RViz)
    marker_pub_ = this->create_publisher<visualization_msgs::msg::Marker>("draw_line", 10);
  }

  void run() {
    RCLCPP_INFO(this->get_logger(), "Đang kết nối MoveGroupInterface...");
    auto move_group = moveit::planning_interface::MoveGroupInterface(shared_from_this(), "ur_manipulator");
    moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

    move_group.setMaxVelocityScalingFactor(0.2);
    move_group.setMaxAccelerationScalingFactor(0.2);

    // 1. TẠO MẶT BÀN BẢO VỆ
    moveit_msgs::msg::CollisionObject table;
    table.header.frame_id = move_group.getPlanningFrame();
    table.id = "safety_floor";
    shape_msgs::msg::SolidPrimitive primitive;
    primitive.type = primitive.BOX;
    primitive.dimensions = {2.0, 2.0, 0.02};
    geometry_msgs::msg::Pose table_pose;
    table_pose.orientation.w = 1.0;
    table_pose.position.z = -0.01;
    table.primitives.push_back(primitive);
    table.primitive_poses.push_back(table_pose);
    table.operation = table.ADD;
    planning_scene_interface.applyCollisionObject(table);

   
    // 2. ÉP TƯ THẾ ELBOW UP BẰNG KHÔNG GIAN KHỚP (TRÁNH KẸT GẦM)

    RCLCPP_INFO(this->get_logger(), "Đang giương tay lên tư thế chuẩn bị (Elbow Up)...");
    std::vector<double> joint_group_positions = {
        0.0,        // Base: Quay thẳng tới trước
        -1.5708,    // Shoulder: Ngửa bắp tay lên trời (-90 độ)
        1.5708,     // Elbow: Gập cẳng tay về trước (90 độ)
        -1.5708,    // Wrist 1: Cụp cổ tay xuống mặt bàn (-90 độ)
        -1.5708,    // Wrist 2: Xoay mặt bích đúng chiều (-90 độ)
        0.0         // Wrist 3: 0 độ
    };
    
    move_group.setJointValueTarget(joint_group_positions);
    moveit::planning_interface::MoveGroupInterface::Plan plan_joint;
    if (move_group.plan(plan_joint) == moveit::core::MoveItErrorCode::SUCCESS) {
        move_group.execute(plan_joint);
        rclcpp::sleep_for(std::chrono::seconds(1));
    }

    // 3. VẼ CHỮ V BẰNG CARTESIAN PATH

    std::vector<geometry_msgs::msg::Pose> waypoints;
    auto p = move_group.getCurrentPose().pose; // Lấy tọa độ từ tư thế giương tay
    
    double z_draw = 0.10; 
    double z_lift = 0.25;

    // Điểm 1: Hạ bút đỉnh trái
    p.position.x = 0.25; p.position.y = -0.06; p.position.z = z_draw;
    waypoints.push_back(p);

    // Điểm 2: Đáy chữ V
    p.position.x = 0.32; p.position.y = 0.00; p.position.z = z_draw;
    waypoints.push_back(p);

    // Điểm 3: Đỉnh phải chữ V
    p.position.x = 0.25; p.position.y = 0.06; p.position.z = z_draw;
    waypoints.push_back(p);

    // Điểm 4: Nhấc bút
    p.position.z = z_lift;
    waypoints.push_back(p);

    moveit_msgs::msg::RobotTrajectory trajectory;
    double fraction = move_group.computeCartesianPath(waypoints, 0.01, 0.0, trajectory);

    if (fraction > 0.9) {
        RCLCPP_INFO(this->get_logger(), "Bắt đầu vẽ nét...");
        move_group.execute(trajectory);
        
        
        // 4. BẬT NÉT MỰC ĐỎ TRÊN RVIZ (MARKER)
        
        drawVisualLetterV();
        RCLCPP_INFO(this->get_logger(), "HOÀN TẤT VẼ CHỮ V! Hãy xem nét đỏ trên RViz.");
    }
  }

private:
  rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr marker_pub_;

  void drawVisualLetterV() {
    visualization_msgs::msg::Marker marker;
    marker.header.frame_id = "base_link";
    marker.header.stamp = this->now();
    marker.ns = "letter_v";
    marker.id = 0;
    marker.type = visualization_msgs::msg::Marker::LINE_STRIP; // Vẽ dạng đường nối
    marker.action = visualization_msgs::msg::Marker::ADD;
    marker.scale.x = 0.005; // Độ dày nét bút 5mm
    marker.color.r = 0.0;
    marker.color.g = 1.0;
    marker.color.b = 0.0;   // Màu xanh
    marker.color.a = 1.0;   // Độ đậm 100%

    geometry_msgs::msg::Point p1, p2, p3;
    p1.x = 0.25; p1.y = -0.06; p1.z = 0.10;
    p2.x = 0.32; p2.y = 0.00; p2.z = 0.10;
    p3.x = 0.25; p3.y = 0.06; p3.z = 0.10;

    marker.points.push_back(p1);
    marker.points.push_back(p2);
    marker.points.push_back(p3);

    marker_pub_->publish(marker);
  }
};

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  node_options.automatically_declare_parameters_from_overrides(true);
  auto node = std::make_shared<LetterVWriterNode>(node_options);

  std::thread run_thread([node]() {
    rclcpp::sleep_for(std::chrono::seconds(2));
    node->run();
  });

  rclcpp::spin(node);
  run_thread.join();
  rclcpp::shutdown();
  return 0;
}