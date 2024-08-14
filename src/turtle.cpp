#include <iostream>
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

using namespace std;

bool mode_holonomic = 0;
bool mode_differential = 1;

class Turtle {
    ros::NodeHandle n;
    ros::Publisher pub;
    ros::Subscriber sub;
public:
    //线速度 单位: m/s; 角速度 单位: rad/s
    double linear_speed_limit=0.4, angular_speed_limit=0.7;
    string robot_simulation;
    string turtle_topic;
    void callback(const sensor_msgs::Joy::ConstPtr &Joy);
    Turtle() {
        n.getParam("robot", robot_simulation);
        if (robot_simulation=="turtlesim") {
            // 小乌龟
            turtle_topic = "/cmd_vel";
        }
        else {
            // turtlebot3
            turtle_topic = "/cmd_vel";
            linear_speed_limit = 0.4;
            angular_speed_limit = 0.7;
        }
        // cout << "robot_simulation: " << robot_simulation << "  turtle_topic: " << turtle_topic << endl;
        pub = n.advertise<geometry_msgs::Twist>(turtle_topic, 1);
        sub = n.subscribe<sensor_msgs::Joy>("joy", 10, &Turtle::callback, this);
    }
};

void Turtle::callback(const sensor_msgs::Joy::ConstPtr &Joy) {
    // cout << "axes("<<Joy->axes.size()<<"): [";
    // for (int i = 0; i < Joy->axes.size(); i++) {
    //     cout << Joy->axes.at(i) << " ,";
    // }
    // cout << "]" << endl << "buttons("<<Joy->buttons.size()<<"): [";
    // for (int i = 0; i < Joy->buttons.size(); i++) {
    //     cout << Joy->buttons.at(i) << " ,";
    // }
    // cout << "]" << endl;
    geometry_msgs::Twist twist;
    if(Joy->buttons[10]){
        mode_holonomic  = !mode_holonomic;
        mode_differential  = !mode_differential;
    }
    if(mode_holonomic){
        twist.linear.x = Joy->axes[1] * linear_speed_limit;
        twist.linear.y = Joy->axes[0] * linear_speed_limit;
        twist.angular.z = 0;
    }    
    if(mode_differential){
        twist.linear.x = Joy->axes[1] * linear_speed_limit;
        twist.angular.z = Joy->axes[2] * angular_speed_limit;
        twist.linear.y = 0;
    }

    // ROS_INFO("linear: %.3f angular: %.3f", twist.linear.x, twist.angular.z);
    // ROS_INFO("y : %.3f ", Joy->axes[0]);
    pub.publish(twist);
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "turtle");
    Turtle turtle;
    ros::spin();
    return 0;
}


