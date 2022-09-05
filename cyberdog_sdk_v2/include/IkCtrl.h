//
// Created by wfy on 7/13/22.
//

#ifndef CYBERDOG_SDK_IKCTRL_H
#define CYBERDOG_SDK_IKCTRL_H
#include <chrono>
#include <memory>
#include <CustomInterface.h>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include <cmath>

using namespace std::chrono_literals;

//const int reduction_ratio = 1;                                       // Reduction ratio of the actuator
const float calf = 200, thigh = 200, rshoulder = 80, lshoulder = -80;

// Segment length of the leg parts, shoulder is the offset between the shoulder motor and the axis of rotation
/*float center_position[12] = {-0.0003,-0.0002,-1.693,
                             -8.727e-05,-0.0002,-1.693,
                             -0.0003,-0.0002,-1.693,
                             -8.727e-05,-0.0002,-1.693};
*/
float sq(float len){
    return len*len;
}
class IK_node: public rclcpp::Node
{
public:
    IK_node();

private:
    /*
   float target_X(Leg *leg, float height, float distX);
   float target_Y(Leg *leg, float height, float posY);
   void  target_Z(Leg *leg, float height);
   void  ik_compute(Leg *leg, float pos_z, float pos_x, float pos_y);
   */
    void  ik_manager();
    void  walk();
    void walk_foward();
    float z_dir_mov(float x);
    float x_dir_mov(float x);
    static float polynomial(float num, int deg);
    //void xyzToAngles(Leg *leg, const float xyz[]);
    void newxyzToAngles(Leg *leg, float xyz[], int shoulder);
    //void  manipulate_ctrl();
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr publisher;
    rclcpp::TimerBase::SharedPtr timer;
    int val = -20;

    float fr[3] = {0, -73, -200};
    float fl[3] = {0, 73, -200};
    float br[3] = {0, -73, -200};
    float bl[3] = {0, 73, -200};

    int step=0;
    int prvstep;

};

#endif //CYBERDOG_SDK_IKCTRL_H
