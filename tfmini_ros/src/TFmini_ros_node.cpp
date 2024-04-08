#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/range.hpp"
#include <string>
#include <memory>

class TFmini
{
public:
  TFmini(std::string _name, int _baudRate);
  ~TFmini(){};
  float getDist();
  void closePort();

private:
  std::string portName_;
  int baudRate_;
  int serial_;

  bool readData(unsigned char *_buf, int _nRead);
};

TFmini::TFmini(std::string _name, int _baudRate) :
  portName_(_name), baudRate_(_baudRate)
{
  // Your initialization code here
}

float TFmini::getDist()
{
  // Your getDist() implementation here
}

void TFmini::closePort()
{
  // Your closePort() implementation here
}

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("tfmini_ros_node");
  std::string id = "TFmini";
  std::string portName;
  int baud_rate;
  std::shared_ptr<TFmini> tfmini_obj;

  node->declare_parameter("serial_port", "/dev/ttyUSB0");
  node->declare_parameter("baud_rate", 115200);
  portName = node->get_parameter("serial_port").as_string();
  baud_rate = node->get_parameter("baud_rate").as_int();

  tfmini_obj = std::make_shared<TFmini>(portName, baud_rate);
  auto pub_range = node->create_publisher<sensor_msgs::msg::Range>(id, 1000);
  sensor_msgs::msg::Range TFmini_range;
  TFmini_range.radiation_type = sensor_msgs::msg::Range::INFRARED;
  TFmini_range.field_of_view = 0.04;
  TFmini_range.min_range = 0.3;
  TFmini_range.max_range = 12;
  TFmini_range.header.frame_id = id;
  float dist = 0;
  RCLCPP_INFO(node->get_logger(), "Start processing ...");

  while (rclcpp::ok())
  {
    rclcpp::spin_some(node);
    dist = tfmini_obj->getDist();
    if (dist > 0 && dist < TFmini_range.max_range)
    {
      TFmini_range.range = dist;
      TFmini_range.header.stamp = node->now();
      pub_range->publish(TFmini_range);
    }
    else if (dist == -1.0)
    {
      RCLCPP_ERROR(node->get_logger(), "Failed to read data. TFmini ros node stopped!");
      break;
    }
    else if (dist == 0.0)
    {
      RCLCPP_ERROR(node->get_logger(), "Data validation error!");
    }
  }

  tfmini_obj->closePort();
  rclcpp::shutdown();
  return 0;
}
