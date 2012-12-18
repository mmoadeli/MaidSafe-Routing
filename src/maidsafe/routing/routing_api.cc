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

#include "maidsafe/routing/routing_api.h"
#include "maidsafe/routing/routing_impl.h"


namespace maidsafe {

namespace routing {

namespace { typedef boost::asio::ip::udp::endpoint Endpoint; }


Routing::Routing(const Fob& fob, bool client_mode) : pimpl_(new Impl(fob, client_mode)) {}

void Routing::Join(Functors functors, std::vector<Endpoint> peer_endpoints) {
  pimpl_->Join(functors, peer_endpoints);
}

void Routing::DisconnectFunctors() {  // TODO(Prakash) : fix race condition when functors in use
  pimpl_->DisconnectFunctors();
}

int Routing::ZeroStateJoin(Functors functors,
                           const Endpoint& local_endpoint,
                           const Endpoint& peer_endpoint,
                           const NodeInfo& peer_info) {
  return pimpl_->ZeroStateJoin(functors, local_endpoint, peer_endpoint, peer_info);
}

void Routing::Send(const NodeId& destination_id,
                   const NodeId& group_claim,
                   const std::string& data,
                   ResponseFunctor response_functor,
                   const boost::posix_time::time_duration& timeout,
                   const DestinationType &destination_type,
                   const bool &cacheable) {
  pimpl_->Send(destination_id, group_claim, data, response_functor, timeout, destination_type,
               cacheable);
}

NodeId Routing::GetRandomExistingNode() const {
  return pimpl_->GetRandomExistingNode();
}

bool Routing::IsNodeIdInGroupRange(const NodeId& node_id) const {
  return pimpl_->IsNodeIdInGroupRange(node_id);
}


}  // namespace routing

}  // namespace maidsafe
