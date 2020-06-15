#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

//For solving the problem using opencv
//#include <opencv2/opencv.hpp>
//using namespace cv;

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv))
    ROS_ERROR("Failed to call service command_robot");

}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int pixels = img.step;
    ROS_INFO_STREAM("Height: "+ std::to_string(img.height));
    ROS_INFO_STREAM("Step: "+ std::to_string(img.step));
    int index;
    geometry_msgs::Twist motor_command;
    int step = img.step;
    int height = img.height;
    int l = 0;
    int c = 0;
    int r = 0;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    for (int i=0; i<(step/3); i++){
      for(int j=0; j<height; j++){
        if (img.data[j*step + i] == white_pixel)
        l++;}}
    for (int i=(step/3); i<(2*step/3); i++){
      for(int j=0; j<height; j++){
        if (img.data[j*step + i] == white_pixel)
        c++;}}
    for (int i=(2*step/3); i<step; i++){
      for(int j=0; j<height; j++){
        if (img.data[j*step + i] == white_pixel)
        r++;}}

    ROS_INFO_STREAM("l: " + std::to_string(l)+ " c: " + std::to_string(c) + " r: " + std::to_string(r));

    if (l>r && l>c)
    drive_robot(0.1,0.09);
    else if(r>l && r>c)
    drive_robot(0.1,-0.09);
    else if(c>l && c>r)
    drive_robot(0.15,0);
    else if ((c+l+r) == 0)
    drive_robot(0,0);
  }


int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
