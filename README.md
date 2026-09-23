# Lập trình UR3 vẽ nét chữ bằng ROS 2 & MoveIt 2

## 1. Cài đặt và Biên dịch

```bash
sudo apt update
sudo apt install ros-humble-moveit ros-humble-moveit-ros-planning-interface -y
```

## 2. Clone và Build Workspace
Clone repository này vào thư mục src của ROS 2 Workspace:

```bash
cd ~/ur_gz_ws/src
git clone https://github.com/tuanvu162/th1_ur3_letter.git
cd ~/ur_gz_ws
colcon build --packages-select lab01_ur3_letter
source install/setup.bash
```

## 3. Chạy chương trình

```bash
source ~/ur_gz_ws/install/setup.bash
ros2 launch lab01_ur3_letter draw_letter.launch.py
```
