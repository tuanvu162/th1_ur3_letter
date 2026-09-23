import os
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    ur_sim_gz_dir = get_package_share_directory('ur_simulation_gz')
    ur_moveit_dir = get_package_share_directory('ur_moveit_config')
    lab_pkg_dir = get_package_share_directory('lab01_ur3_letter')

    rviz_config_file = os.path.join(lab_pkg_dir, 'rviz', 'letter_v.rviz')

    # 1. KHỞI ĐỘNG GAZEBO 
    ur_sim_control_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(ur_sim_gz_dir, 'launch', 'ur_sim_control.launch.py')
        ),
        launch_arguments={
            'ur_type': 'ur3e',
            'use_sim_time': 'true',
            'launch_rviz': 'false'
        }.items()
    )

    # 2. KHỞI ĐỘNG MOVEIT
    ur_moveit_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(ur_moveit_dir, 'launch', 'ur_moveit.launch.py')
        ),
        launch_arguments={
            'ur_type': 'ur3e',
            'use_sim_time': 'true',
            'launch_rviz': 'false'
        }.items()
    )

    # 3. GỌI RVIZ 
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config_file],
        parameters=[{'use_sim_time': True}]
    )

    # 4. GỌI NODE VẼ CHỮ V
    writer_node = Node(
        package='lab01_ur3_letter',
        executable='letter_v_node',
        output='screen',
        parameters=[{'use_sim_time': True}]
    )

    delayed_writer_node = TimerAction(
        period=5.0, 
        actions=[writer_node]
    )

    return LaunchDescription([
        ur_sim_control_launch,
        ur_moveit_launch,
        rviz_node,
        delayed_writer_node
    ])