#pragma once
#ifndef VEHICLE_CONTROL_H
#define VEHICLE_CONTROL_H

#define BOOST_BIND_NO_PLACEHOLDERS

#include "common.h"
#include "rclcpp/rclcpp.hpp"
#include "pid_controller.h"
#include <std_msgs/msg/bool.hpp>
#include <nav_msgs/msg/path.hpp>

#include "carla_msgs/msg/carla_ego_vehicle_control.hpp"
#include "carla_msgs/msg/carla_status.hpp"
#include <std_msgs/msg/float32.hpp>
#include "carla_msgs/msg/carla_vehicle_target_velocity.hpp"
#include "carla_msgs/msg/carla_ego_vehicle_status.hpp"
#include "derived_object_msgs/msg/object_array.hpp"

#include <tf2/LinearMath/Quaternion.h>
#include <tf2_ros/transform_broadcaster.h>

#include "planning/lattice/lattice.h"
#include "planning/reference_line/reference_line.h"

using std::placeholders::_1;

#define AEB_TEST_CRUISE_SPEED ((double)100.0)
#define AEB_BRAKE_REACTION_TIME ((double)0.525)
#define AEB_BRAKE_DECELERATION ((double)-9.0)

class VehicleControlPublisher : public rclcpp::Node
{
public:
    VehicleControlPublisher();
    ~VehicleControlPublisher();

    void NopRunOnce();

    double PointDistanceSquare(const TrajectoryPointOri &point, const double x, const double y);

    TrajectoryPointOri QueryNearestPointByPosition(const double x, const double y);

    void odomCallback(nav_msgs::msg::Odometry::SharedPtr msg);

    void VehicleControlIterationCallback(); // 收到仿真器返回的状态后，产生控制信号
    // void VehicleControlIterationCallback(carla_msgs::msg::CarlaStatus::SharedPtr msg); // 收到仿真器返回的状态后，产生控制信号

    void VehicleStatusCallback(carla_msgs::msg::CarlaEgoVehicleStatus::SharedPtr msg);

    // for rviz path display
    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr history_path_visualization_publisher;
    nav_msgs::msg::Path history_path;
    geometry_msgs::msg::PoseStamped history_path_points;

    rclcpp::TimerBase::SharedPtr global_path_publish_timer;
    void GlobalPathPublishCallback();

    void ObjectArrayCallback(derived_object_msgs::msg::ObjectArray::SharedPtr msg);

    void SetObjectArrayCallback(derived_object_msgs::msg::ObjectArray::SharedPtr msg);

    void Turn_obstacles_into_squares(visualization_msgs::msg::Marker &marker, const Obstacle *Primitive_obstacle,
                                     const int id);

public:
    double V_set_;
    double T_gap_;

    bool first_record_;
    bool cout_distance_;
    bool cout_speed_;

    int cnt;
    int qos;

    std::vector<std::pair<double, double>> xy_points;
    std::vector<double> v_points;
    std::string _line;
    std::string count_csv;
    std::string timestamp;
    std::string x;
    std::string y;
    std::string z;
    std::string yaw;
    std::string velocity;

    double controller_frequency;

    double acceleration_cmd;
    double steer_cmd;

    std::vector<TrajectoryPointOri> trajectory_points_;
    TrajectoryData planning_published_trajectory;

    // Input
    VehicleState vehicle_state_;

    tf2::Quaternion localization_quaternion_transform;

    rclcpp::TimerBase::SharedPtr vehicle_control_iteration_timer_;

    rclcpp::Publisher<carla_msgs::msg::CarlaEgoVehicleControl>::SharedPtr vehicle_control_publisher;
    carla_msgs::msg::CarlaEgoVehicleControl control_cmd;

    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr vehicle_control_manual_override_publisher;
    std_msgs::msg::Bool vehicle_control_manual_override;

    rclcpp::Publisher<carla_msgs::msg::CarlaVehicleTargetVelocity>::SharedPtr vehicle_control_target_velocity_publisher;
    carla_msgs::msg::CarlaVehicleTargetVelocity vehicle_control_target_velocity;

    rclcpp::Subscription<carla_msgs::msg::CarlaEgoVehicleStatus>::SharedPtr carla_status_subscriber;

    rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr localization_data_subscriber;

    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_gps_vehicle;

    rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr global_path_publisher_;
    nav_msgs::msg::Path global_path;
    geometry_msgs::msg::PoseStamped this_pose_stamped;

    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr rviz_obstacle_pub_;

    // added
    derived_object_msgs::msg::ObjectArray carla_vehicle_objects_;
    rclcpp::Subscription<derived_object_msgs::msg::ObjectArray>::SharedPtr carla_vehicle_object_subscriber;
    bool if_aeb_active_{false};

public:
    // add for nop
    int which_planners{0};
    std::shared_ptr<l5player::planning::PlanningBase> planning_base_;
    Eigen::MatrixXd routing_waypoints_; // 中心点坐标
    l5player::reference_line::referenceLine rl_;
    std::vector<Obstacle> AllObstacle;
};

#endif