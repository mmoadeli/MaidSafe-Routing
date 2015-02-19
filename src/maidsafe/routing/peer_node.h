/*  Copyright 2012 MaidSafe.net limited

    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,
    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which
    licence you accepted on initial access to the Software (the "Licences").

    By contributing code to the MaidSafe Software, or to this project generally, you agree to be
    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root
    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also
    available at: http://www.maidsafe.net/licenses

    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed
    under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS
    OF ANY KIND, either express or implied.

    See the Licences for the specific language governing permissions and limitations relating to
    use of the MaidSafe Software.                                                                 */

#ifndef MAIDSAFE_ROUTING_PEER_NODE_H_
#define MAIDSAFE_ROUTING_PEER_NODE_H_

#include <memory>

#include "maidsafe/common/convert.h"
#include "maidsafe/crux/socket.hpp"
#include "maidsafe/passport/types.h"
#include "maidsafe/routing/node_info.h"

namespace maidsafe {

namespace routing {

class PeerNode {
 private:
   using error_code = boost::system::error_code;
 public:
   static const size_t MaxMessageSize = 1048576;

 public:
  using PublicPmid = passport::PublicPmid;

  PeerNode(const PeerNode&) = delete;
  PeerNode& operator=(const PeerNode&) = delete;
  PeerNode(PeerNode&&) = default;
  PeerNode& operator=(PeerNode&&) = default;

  PeerNode(NodeInfo node_info, std::shared_ptr<crux::socket> socket)
      : node_info_(std::move(node_info)),
        connected_(true),
        receive_buffer_(MaxMessageSize),
        socket(std::move(socket))
  {}

  template <typename Message, typename Handler>
  void Send(Message msg, const Handler& handler) {

    auto msg_ptr = std::make_shared<Message>(std::move(msg));
    std::weak_ptr<crux::socket> weak_socket = socket;

    socket->async_send(boost::asio::buffer(*msg_ptr),
                       [this, msg_ptr, handler, weak_socket](error_code error, size_t) {
      if (!weak_socket.lock()) {
        // This object was destroyed.
        return handler(asio::error::operation_aborted);
      }

      if (error) { connected_ = false; }

      handler(convert::ToStd(error));
    });
  }

  template <typename Handler>
  void Receive(const Handler& handler) {
    std::weak_ptr<crux::socket> weak_socket = socket;

    socket->async_receive(boost::asio::buffer(receive_buffer_),
        [=](error_code error, size_t) {
          if (!weak_socket.lock()) {
            // This object was destroyed.
            return handler(asio::error::operation_aborted, std::vector<unsigned char>());
          }

          if (error) { return handler(error, receive_buffer_); }

          handler(error, receive_buffer_);
        });
  }

  const NodeInfo& node_info() const { return node_info_; }
  bool connected() const { return connected_; }

 private:
  NodeInfo node_info_;
  // int32_t rank;
  bool connected_;
  std::vector<unsigned char> receive_buffer_;
  std::shared_ptr<crux::socket> socket;
};

}  // namespace routing

}  // namespace maidsafe

#endif  // MAIDSAFE_ROUTING_PEER_NODE_H_
