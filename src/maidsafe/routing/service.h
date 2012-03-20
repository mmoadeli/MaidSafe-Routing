/*******************************************************************************
 *  Copyright 2012 maidsafe.net limited                                        *
 *                                                                             *
 *  The following source code is property of maidsafe.net limited and is not   *
 *  meant for external use.  The use of this code is governed by the licence   *
 *  file licence.txt found in the root of this directory and also on           *
 *  www.maidsafe.net.                                                          *
 *                                                                             *
 *  You are not free to copy, amend or otherwise use this source code without  *
 *  the explicit written permission of the board of directors of maidsafe.net. *
 ******************************************************************************/

#ifndef MAIDSAFE_ROUTING_SERVICE_H_
#define MAIDSAFE_ROUTING_SERVICE_H_
#include "rpcs.h"


namespace maidsafe {

namespace routing {
namespace protobuf {  class Message; }

// Handle all incoming requests and send back reply

class Service {
 public:
  Service(std::shared_ptr<Rpcs> rpc_ptr,
          std::shared_ptr<RoutingTable> routing_table);
  void Ping(const protobuf::Message &message);
  void Connect(const protobuf::Message &message);
  void FindNodes(const protobuf::Message &message);
 private:
  std::shared_ptr<Rpcs> rpc_ptr_;
  std::shared_ptr<RoutingTable> routing_table_;
};

}  // namespace routing

}  // namespace maidsafe


#endif  // MAIDSAFE_ROUTING_SERVICE_H_
