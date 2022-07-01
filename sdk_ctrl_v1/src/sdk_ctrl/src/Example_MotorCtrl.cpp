// Copyright (c) 2021 Beijing Xiaomi Mobile Software Co., Ltd. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <CustomInterface.h>
#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "sensor_msgs/msg/imu.hpp"

using namespace std::chrono_literals;


//MOTOR CTRL Context
class CustomCtrl : public CustomInterface
        {
public:
    CustomCtrl(const double &loop_rate): CustomInterface(loop_rate ){};
    ~CustomCtrl(){};

private:
    bool      first_run = true;
    long long count     = 0;
    float     init_q[ 12 ];
    float     target1_q[ 3 ] = { 0 / 57.3, 80 / 57.3, -135 / 57.3 };
    float     target2_q[ 3 ] = { 0 / 57.3, 45 / 57.3, -90 / 57.3 };
    void      UserCode() {
        float t = ( count / 1500.0 ) > 2 ? 2 : ( count / 1500.0 );
        if ( first_run == true ) {
            for ( int i = 0; i < 12; i++ )
                init_q[ i ] = robot_data.q[ i ];
            if ( init_q[ 2 ] < -0.1 && init_q[ 5 ] < -0.1 && init_q[ 8 ] < -0.1 && init_q[ 11 ] < -0.1 ) {
                first_run = false;
                count     = 0;
            }
        }
        else {
            for ( int i = 0; i < 12; i++ ) {
                if ( t < 1.0 )
                    motor_cmd.q_des[ i ]   = target1_q[ i % 3 ] * t + init_q[ i ] * ( 1 - t );
                else
                    motor_cmd.q_des[ i ]   = target2_q[ i % 3 ] * ( t - 1 ) + target1_q[ i % 3 ] * ( 2 - t );
                motor_cmd.kp_des[ i ]  = 100;
                motor_cmd.kd_des[ i ]  = 3;
                motor_cmd.qd_des[ i ]  = 0;
                motor_cmd.tau_des[ i ] = 0;
            }
        }
        if ( ( count++ ) % 1000 == 0 ) {
            printf( "interval:---------%.4f-------------\n", robot_data.ctrl_topic_interval );
            printf( "rpy [3]:" );
            for ( int i = 0; i < 3; i++ )
                printf( " %.2f", robot_data.rpy[ i ] );
            printf( "\nacc [3]:" );
            for ( int i = 0; i < 3; i++ )
                printf( " %.2f", robot_data.acc[ i ] );
            printf( "\nquat[4]:" );
            for ( int i = 0; i < 4; i++ )
                printf( " %.2f", robot_data.quat[ i ] );
            printf( "\nomeg[3]:" );
            for ( int i = 0; i < 3; i++ )
                printf( " %.2f", robot_data.omega[ i ] );
            printf( "\nq  [12]:" );
            for ( int i = 0; i < 12; i++ )
                printf( " %.2f", robot_data.q[ i ] );
            printf( "\nqd [12]:" );
            for ( int i = 0; i < 12; i++ )
                printf( " %.2f", robot_data.qd[ i ] );
            printf( "\ntau[12]:" );
            for ( int i = 0; i < 12; i++ )
                printf( " %.2f", robot_data.tau[ i ] );
            printf( "\nctrl[12]:" );
            for ( int i = 0; i < 12; i++ )
                printf( " %.2f", motor_cmd.q_des[ i ] );
            printf( "\n\n" );
        }
    }
};
//MOTOR CTRL Context

std::shared_ptr< CustomCtrl > io;
void signal_callback_handler( int signum ) {
    io->Stop();
    ( void )signum;
}

//ROS2 Subscription and Publication
class SubPub : public rclcpp::Node
{
public:
    SubPub() : Node("SubPub")
    {
        publisher_J = this->create_publisher<sensor_msgs::msg::JointState>("JointState",10);
        timer_J = this->create_wall_timer(
                500ms, std::bind(&SubPub::pub_JointState, this));
    }
private:
    void pub_JointState();
    rclcpp::TimerBase::SharedPtr timer_J;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher_J;
};


void SubPub::pub_JointState()
{
    auto state = sensor_msgs::msg::JointState();
    state.name.resize(12);
    state.position.resize(12);
    state.velocity.resize(12);
    state.effort.resize(12);
    for ( int i = 0; i < 12; i++ ) {
        state.name [ i ] = i;
        state.name [ i ] = i;
        state.position[ i ] = io->robot_data.q[ i ];
        state.velocity[ i ] = io->robot_data.qd[ i ];
        state.effort[ i ] = io->robot_data.tau[ i ];
    }
    RCLCPP_INFO(this->get_logger(), "Publishing: JointState");
    RCLCPP_INFO(this->get_logger(),"");
    publisher_J->publish(state);
    }
//ROS2 Subscription and Publication


//MAIN
int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    signal( SIGINT, signal_callback_handler );
    io = std::make_shared< CustomCtrl >( 500 );
    rclcpp::spin(std::make_shared< SubPub >());
    rclcpp::shutdown();
    return 0;
}
