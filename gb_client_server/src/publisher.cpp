#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/UInt8.h"
#include "gb_service/GlassBridge.h" 
#include <iostream>
#include <string>


int main(int argc, char **argv)
{
    ros::init(argc, argv, "gb_publisher"); 
    ros::NodeHandle n;

    std::string server_name;
    n.param<std::string>("server_name", server_name, "gb_service");

    ros::ServiceClient client = n.serviceClient<gb_service::GlassBridge>(server_name);
    gb_service::GlassBridge srv;

    while (ros::ok()) 
    {
        //Entering
        ROS_INFO_STREAM("Please, enter the destination (l or r): ");
        std::cin >> srv.request.destination;

        //Responsing
        if (client.call(srv))
        {
            ROS_INFO_STREAM(" " << srv.response.response);

            if (srv.response.response == "dead")
            {
                ros::shutdown();
            }
            if (srv.response.response == "end game")
            {
                ROS_INFO("congratulations, you won");
                ros::shutdown();
            }
        }
        else
        {
            ROS_INFO("Failed to call service gb_service");
            ros::shutdown();
        }
    }

    return 0;
}
