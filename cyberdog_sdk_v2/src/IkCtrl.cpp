//
// Created by ysy on 7/11/22.
// Modified by wfy
//
#include <chrono>
#include <memory>
#include <CustomInterface.h>
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include <cmath>
#include <IkCtrl.h>

IK_node::IK_node(): Node("IK_node") {
    publisher = this->create_publisher<sensor_msgs::msg::JointState>("/joint_states",10);
    std::cout << "establishing..." <<std::endl;
    timer = this->create_wall_timer(10ms,std::bind(&IK_node::ik_manager, this));
}

void IK_node::walk(){

    if(step<100000) {
        fl[2] = -300+z_dir_mov(step/20);
        br[2] = -300+z_dir_mov(step/20);
        fr[2] = -300+z_dir_mov(step/20+PI);
        bl[2] = -300+z_dir_mov(step/20+PI);
        step++;
    }

}

void IK_node::walk_foward(){
    if(step<100000){
        const int mov_f = 3;
        const float mov_z = 1.5;
        const float phase = -PI/2;
        //Z-Coordinates
        fl[2] = -300+mov_z*x_dir_mov(step/20);
        br[2] = -300+mov_z*x_dir_mov(step/20);
        fr[2] = -300+mov_z*x_dir_mov(step/20+PI);
        bl[2] = -300+mov_z*x_dir_mov(step/20+PI);
        //X-Coordinates
        fl[0] = -10+mov_f*x_dir_mov(step/20+phase);
        br[0] = -10+mov_f*x_dir_mov(step/20+phase);
        fr[0] = -10+mov_f*x_dir_mov(step/20+PI+phase);
        bl[0] = -10+mov_f*x_dir_mov(step/20+PI+phase);
        step++;
    }
}

// Calculates the angle of the calf actuator as well as part of the angle of the hip actuator
// Takes a pointer to the struct that cointains the angles for the leg
/*
void IK_node::target_Z(Leg *leg, float height){
  float thetaZ = ( PI/2 - acos( calf*calf + height*height - thigh*thigh) / ( 2*calf*height ) ) * reduction_ratio;
  float phiZ = ( acos( calf*calf + thigh*thigh - height*height) / ( 2*calf*thigh ) )  * reduction_ratio;
  float *theta = &(*leg).theta;
  *theta -= thetaZ;
  float *phi = &(*leg).phi;
  *phi = -phiZ;
}

// Calculates part of the angle of the hip actuator as well as the "Leg length" considering the offset cause by inputing a non 0 target_X coordinate
float IK_node::target_X(Leg *leg, float height, float distX){
  float extraTheta = ( atan( distX / height ) );
  float thetaX = extraTheta * reduction_ratio;
  float newLegLength = ( height / (cos(extraTheta)) );
  //newLegLength = heightRestriction(abs(newLegLength));
  float *theta = &(*leg).theta;
  *theta = thetaX;
  return newLegLength;
}
// Calculates the hip angle
float IK_node::target_Y(Leg *leg, float height, float posY){
  float distY = shoulder + posY;
  float gammaP = atan( distY / height );
  if (std::isnan(gammaP)) gammaP = PI/2;
  float hipHyp = distY / sin( gammaP );
  float lambda = asin (shoulder / hipHyp );
  float gammaY = ( (  - lambda ) + gammaP  ) * reduction_ratio;
  float newNewLegLength = shoulder/tan(lambda);
  //newNewLegLength = heightRestriction(abs(newNewLegLength));
  float *gamma = &(*leg).gamma;
  *gamma = gammaY;
  return newNewLegLength;
}
void IK_node::ik_compute(Leg *leg, float pos_z, float pos_x, float pos_y){
  target_Z(leg, target_X(leg, target_Y(leg, pos_z, pos_y), pos_x));
}

*/


void IK_node::newxyzToAngles(Leg *leg, float xyz[3], int shoulder) {


    const float pi = 3.141593;
    const int L1 = shoulder;
    const int L2 = thigh;
    const int L3 = calf;
    float theta1R, theta2R, theta3R;
    //theta1R = 2 * atan((xyz[2]+sqrt(sq(xyz[1])+sq(xyz[2])-sq(L1)))/(xyz[1]-L1));

    //theta3R = -acos((sq(xyz[0])+sq(xyz[1])+sq(xyz[2])-sq(L1)-sq(L2)-sq(L3))/(2*L2*L3));

    //theta2R = -asin(xyz[0]/sqrt(sq(xyz[0])+sq(xyz[1])+sq(xyz[2])-sq(L1)))-asin(L3*sin(theta3R)/sqrt(sq(xyz[0])+sq(xyz[1])+sq(xyz[2])-sq(L1)));

    theta1R = 2 * atan((xyz[2]+sqrt(sq(xyz[1])+sq(xyz[2])-sq(L1)))/(xyz[1]-L1));
    theta3R = -acos((sq(xyz[0])+sq(xyz[1])+sq(xyz[2])-sq(L1)-sq(L2)-sq(L3))/(2*L2*L3));
    theta2R = -asin(xyz[0]/sqrt(sq(xyz[0])+sq(xyz[1])+sq(xyz[2])-sq(L1)))-asin(L3*sin(theta3R)/sqrt(sq(xyz[0])+sq(xyz[1])+sq(xyz[2])-sq(L1)));

    //theta1R = 0.4313188150974882;
    //theta2R = 0.0009980031204267503;
    //theta3R = -1.3681624635906584;

    leg->theta= theta1R;
    leg->phi = theta2R;
    leg->gamma = theta3R;



}

void IK_node::ik_manager(){
//    std::cout << "entering ik_manager" << std::endl;
  Leg legs[] = { {.id = 0},
                 {.id = 1},
                 {.id = 2},
                 {.id = 3} };


  //int leg_amount = sizeof(legs)/sizeof(legs[0]);
  //float first[3] = {15, 10, 5};
  //float second[3]= {-15,10,5};
  //float third[3] = {10 ,2 ,5};
  //float fourth[3] = {-10,2,5};


  newxyzToAngles(&legs[0], fr,rshoulder);
  newxyzToAngles(&legs[1], fl,lshoulder);
  newxyzToAngles(&legs[2], br,rshoulder);
  newxyzToAngles(&legs[3],bl,lshoulder);
  walk_foward();

/*
  if(val <= 80){
      br[2] = val;
  }
  newxyzToAngles(&legs[3], bl,rshoulder);
  val++;*/
  // for (int i = 0; i < leg_amount; i++){
  //   printf("ID: %d, Theta: %f, Phi: %f, Gamma: %f \n", legs[i].id, legs[i].theta, legs[i].phi, legs[i].gamma);
  // }
  auto ik_joint = sensor_msgs::msg::JointState();
  ik_joint.name.resize(12);
  ik_joint.position.resize(12);
  ik_joint.header.stamp = now();
  for (int i = 0; i < 12; i++) {
      ik_joint.name[ i ] = builtin_name[ i ];
      if ( i % 3 == 0) { ik_joint.position[ i ] = legs[ i / 3 ].theta; }
      if ( i % 3 == 1) { ik_joint.position[ i ] = legs[ i / 3 ].phi; }
      if ( i % 3 == 2) { ik_joint.position[ i ] = legs[ i / 3 ].gamma; }
  }
//  std::cout << "publishing" << std::endl;
  publisher->publish(ik_joint);
}


/*
void IK_node::manipulate_ctrl() {
  auto ik_joint = sensor_msgs::msg::JointState();
  ik_joint.name.resize(12);
  ik_joint.position.resize(12);
  ik_joint.header.stamp = now();
  for (int i = 0; i < 12; i++){
    ik_joint.name[ i ] = builtin_name[ i ];
    ik_joint.position[ i ] = center_position[ i ];
  }
  publisher->publish(ik_joint);
}
*/


float IK_node::polynomial(float num, int deg){
    float result=1.0;
    for(int i=1;i<=deg;i++){
        result *= num;
    }
    return result;
}
float IK_node::z_dir_mov(float x) {
    float f;
    while(x>2*PI){
        x-=2*PI;
    }
    float arr[9] = {
            -3.44513864e-02,  8.68535289e-01, -8.68327936e+00,  4.31106625e+01,
            -1.07286774e+02,  1.14134971e+02, -3.26639131e+01,  3.28866634e+01,
            -3.45073675e-02
    };
    f = arr[0]*polynomial(x,8)+arr[1]*polynomial(x,7)+arr[2]*polynomial(x,6)+arr[3]*polynomial(x,5)
            +arr[4]*polynomial(x,4)+arr[5]*polynomial(x,3)+arr[6]*polynomial(x,2)+arr[7]*polynomial(x,1)
            +arr[8];
    return f;
}
float IK_node::x_dir_mov(float x){
    float f;
    while(x>2*PI){
        x-=2*PI;
    }
    float arr[5] = {
            0.0528375,   0.02926946, -5.69996607, 21.84144992, -1.002331
    };
    f = arr[0]* polynomial(x,4)+arr[1]* polynomial(x,3)
            +arr[2]* polynomial(x,2)+arr[3]* polynomial(x,1)
                                     +arr[4];
    return f;
}
int main(int argc, char * argv[]){
    std::cout << "start" << std::endl;
  rclcpp::init( argc, argv );
  rclcpp::spin(std::make_shared< IK_node >());
  std::cout << "shutting down" << std::endl;
  rclcpp::shutdown();
  return 0;
}