// Copyright 2018, Bosch Software Innovations GmbH.
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

#include "rosbag_v2_deserializer.hpp"

#include <memory>
#include <string>

#include "rosbag/message_instance.h"

#include "rosbag2_cpp/converter_interfaces/serialization_format_deserializer.hpp"
#include "rosbag2_cpp/types/introspection_message.hpp"

#include "rosbag2_storage/serialized_bag_message.hpp"

#include "../convert_rosbag_message.hpp"

namespace rosbag2_bag_v2_plugins
{
void RosbagV2Deserializer::deserialize(
  std::shared_ptr<const rosbag2_storage::SerializedBagMessage> serialized_message,
  const rosidl_message_type_support_t * type_support,
  std::shared_ptr<rosbag2_cpp::rosbag2_introspection_message_t> ros_message)
{
  (void) type_support;

  // See rosbag_output_stream.cpp for reasoning: The serialialized data starts with a
  // null-terminated string containing the data_type of the message and then the message itself
  // in serialized form
  auto ros1_data_type = std::string(
    reinterpret_cast<char *>(serialized_message->serialized_data->buffer));

  size_t data_type_zero_terminated_length = ros1_data_type.length() + 1;
  ros::serialization::IStream stream(
    serialized_message->serialized_data->buffer + data_type_zero_terminated_length,
    serialized_message->serialized_data->buffer_length - data_type_zero_terminated_length);

  convert_1_to_2(ros1_data_type, stream, ros_message);

  ros_message->time_stamp = serialized_message->time_stamp;
  rosbag2_cpp::introspection_message_set_topic_name(
    ros_message.get(), serialized_message->topic_name.c_str());
}
}  // namespace rosbag2_bag_v2_plugins

#include "pluginlib/class_list_macros.hpp"  // NOLINT
PLUGINLIB_EXPORT_CLASS(rosbag2_bag_v2_plugins::RosbagV2Deserializer,
  rosbag2_cpp::converter_interfaces::SerializationFormatDeserializer)
