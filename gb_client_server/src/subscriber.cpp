#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/UInt8.h"
#include "gb_service/GlassBridge.h" 
#include <iostream>
#include <cstdlib>
#include <ctime>

const char L_CODE = 108; 
const char R_CODE = 114; 

char g_rightSequence[18] = { 0 };
std::string g_userSteps;
ros::Publisher g_pub;

int g_symCounter = 0;
bool g_shutdownFlag = false;



bool serviceHandler(gb_service::GlassBridge::Request &req, gb_service::GlassBridge::Response &res)
{
    if ((req.destination != L_CODE) && (req.destination != R_CODE))
    {
        res.response = "false";
    }
    else if (req.destination == g_rightSequence[g_symCounter])
    {
        std_msgs::UInt8 magic;
        magic.data = req.destination;
        g_pub.publish(magic);

        g_symCounter++;

        if (g_symCounter == 18)
        {
            res.response = "end game";
            g_shutdownFlag = true;
        }
        else 
        {
            res.response = "move next";
        }
    }
    else
    {
        res.response = "dead";
        g_shutdownFlag = true;
    }

    return true;
}


void msgHandler(std_msgs::UInt8 step)
{
    if (g_userSteps.length() < 18)
    {
        g_userSteps.push_back((char)step.data);
        ROS_INFO_STREAM("your previous steps: " << g_userSteps);

        if (g_userSteps.length() == 18)
        {
            ROS_INFO("congratulations, you won");
            g_shutdownFlag = true;
        }
    }

    return;
}



int main(int argc, char **argv)
{
    ros::init(argc, argv, "gb_service_server");
    ros::NodeHandle n;

    std::string topic_name, server_name;
    n.param<std::string>("topic_name", topic_name, "user_steps");
    n.param<std::string>("server_name", server_name, "gb_service");

    ros::ServiceServer service = n.advertiseService(server_name, serviceHandler);
    g_pub = n.advertise<std_msgs::UInt8>(topic_name, 18);
    ros::Subscriber sub = n.subscribe(topic_name, 18, msgHandler);

    ROS_INFO("Ready to start");

    std::srand(std::time(0));

    for (int i = 0; i < 18; i++)
    {
        g_rightSequence[i] = (rand() % 2) ? R_CODE : L_CODE;
    }

    ROS_INFO_STREAM("the right sequence: " << g_rightSequence);

    while (ros::ok()) 
    {
        if (g_shutdownFlag == true)
        {
            ros::shutdown();
        }
        ros::spinOnce();
    }

    return 0;
}
