//C++ includes, normal
#include <memory>                           // 
#include <chrono>                           // 
#include <functional>                       // 
#include <string>                           // String type variable
#include <unistd.h>                         // usleep 
#include <stdio.h>

//ROS2 includes
#include "rclcpp/rclcpp.hpp"                // General ROS2 stuff
#include "rclcpp_action/rclcpp_action.hpp"  // ROS2 actions info
// Interface package
#include "astra_auto_interfaces/action/navigate_rover.hpp" // NavigateRover action file

#define ACTION_NAME "/astra/auto/navigate_rover"

// Shorthand for the action interface
using NavigateRover = astra_auto_interfaces::action::NavigateRover;

/*
    std::bind does some black magic to provide arguments and placeholders for
    arguments when the function passed is called
    Essentially like creating a second function that calls the desired function
    with custom arguments reliably while also providing user-defined arguments
*/

// Node for the action server
class NavigateRoverServerNode : public rclcpp::Node {
public:
    // A handle to interact with to publish feedback,
    // and mark the goal as aborted, cancelled, succeeded, or in process of
    // executing
    using NavigateRoverGoalHandle = rclcpp_action::ServerGoalHandle<NavigateRover>;

    // Constructor, creates a node with the name "navigate_rover_server"
    explicit NavigateRoverServerNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
    : Node("navigate_rover_server", options) {
        RCLCPP_INFO(this->get_logger(), "Creating action server");
        // Creating action
        this->navigate_rover_server_ = rclcpp_action::create_server<NavigateRover>(
                // ¯\_(ツ)_/¯ Who the fuck knows
                // this->get_node_base_interface(),
                // this->get_node_clock_interface(),
                // this->get_node_logging_interface(),
                // this->get_node_waitables_interface(),
                
                // Node to bind the action to, "this"
                // is the current instance of a ROS Node
                this,
                // Name of the action
                "/astra/auto/navigate_rover",
                // Callback for handling goals: accept, deny, or put off
                std::bind(&NavigateRoverServerNode::goal_callback, this, std::placeholders::_1, std::placeholders::_2),
                // Handle cancellation: accept the cancel or tell the user to buzz off & continue
                std::bind(&NavigateRoverServerNode::cancel_callback, this, std::placeholders::_1),
                // Handle the accepted goal
                std::bind(&NavigateRoverServerNode::handle_accepted_callback, this, std::placeholders::_1)
        );
        RCLCPP_INFO(this->get_logger(), "Action server has been started with name ACTION_NAME");        
    }
    
private:
    rclcpp_action::Server<NavigateRover>::SharedPtr navigate_rover_server_;

    // https://docs.ros2.org/latest/api/rclcpp_action/namespacerclcpp__action.html#ababbbebf29fec7c541687c724fc82ba7
    rclcpp_action::GoalResponse goal_callback(
        const rclcpp_action::GoalUUID &uuid, std::shared_ptr<const NavigateRover::Goal> goal) {
        //to get rid of startup warnings
        (void)uuid;
        (void)goal;
        
        RCLCPP_INFO(this->get_logger(), "Received goal request with navigate type %li", goal->navigate_type);
        
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    // https://docs.ros2.org/latest/api/rclcpp_action/namespacerclcpp__action.html#a27ea2386548de7ad3bef27bb3c0eed05
    rclcpp_action::CancelResponse cancel_callback(
        const std::shared_ptr<NavigateRoverGoalHandle> goal_handle) {
        //to get rid of startup warnings
        (void)goal_handle;

        RCLCPP_INFO(this->get_logger(), "Received request to cancel goal");

        /*
            Do some actual processing to make the goal NOT happen
        */
    
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    // https://docs.ros2.org/latest/api/rclcpp_action/namespacerclcpp__action.html#ae469597b77e40287e19539b806a54619
    // This does not return the rclcpp_action::ResultCode but
    // rather forks and processes, then modifies the shared_ptr goal handle
    void handle_accepted_callback(
        const std::shared_ptr<NavigateRoverGoalHandle> goal_handle) {
        RCLCPP_INFO(this->get_logger(), "Executing the goal");
        
        // We're multithreading, babeeeeee.
        // Open up a new thread and detach from the main process,
        // prevents blocking the current thread's execution and should allow for more goals
        // to continue to be processed
        std::thread(
            std::bind(
                // Call this function with the goal_handle as an argument
                &NavigateRoverServerNode::execute_goal,
                this, std::placeholders::_1),
                goal_handle).detach();
    }

    // Executed in a forked thread
    // Provides feedback and sets the state of the goal
    void execute_goal(
        const std::shared_ptr<NavigateRoverGoalHandle> goal_handle) {
        // Get Request from goal
        // int navigate_type = goal_handle->get_goal()->navigate_type;
        // double gps_lat_target = goal_handle->get_goal()->gps_lat_target;
        // double gps_long_target = goal_handle->get_goal()->gps_long_target;
        // double period = goal_handle->get_goal()->period;

        // Indicate that the server is attempting to execute the goal
        // goal_handle->execute();

        
        // Get the message describing the goal
        auto feedback_msg = std::make_shared<NavigateRover::Feedback>();
        // 
        auto &status_message = feedback_msg->current_status;

        status_message = "HI";

        // Send some data back to the goal
        for (int i = 0; i < 2; i++) goal_handle->publish_feedback(feedback_msg);

        usleep(5000);
        if (rclcpp::ok()) {
            // Publish the result
            auto result = std::make_shared<NavigateRover::Result>();
            result->final_result = 10;

            goal_handle->succeed(result);
        }
    }
};

// RCLCPP_COMPONENTS_REGISTER_NODE(rover_autonomy::NavigateRoverServerNode);

/***********
    MAIN
 ***********/ 
int main(int argc, char **argv) {
    // Initalize ROS2
    rclcpp::init(argc, argv);

    auto navigator_node = std::make_shared<NavigateRoverServerNode>(); 

    rclcpp::spin(navigator_node);
    
    rclcpp::shutdown();
    return 0;
}