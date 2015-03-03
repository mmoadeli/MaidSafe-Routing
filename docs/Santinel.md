Put
client -> client_manager {
            auto result(node_acc_.Add(name, value, pair<sender, message_id>));
            if (result) {
              if (node_key_acc_.Has(sender))
                return Result;
            }
          }

client_manager -> nae_manager {
                    if (header.group.key + Id + Authority does not exist)
                      GetGroupKey();
                    auto result(group_acc_.Add(name, value, pair<sender, message_id>))
                    if (result)
                      if (group_key_acc.Check(header.group + message_id))
                        retrun Result;
                  }

nae_manager -> node_manager {
                    if (header.group.key + Id + Authority does not exist)
                      GetGroupKey();
                    auto result(group_acc_.Add(name, value, pair<sender, message_id>))
                    if (result)
                      if (group_key_acc.Check(header.group + message_id))
                        retrun Result;
                  }

node_manager -> node {
                    if (header.group.key + Id + Authority does not exist)
                      GetGroupKey();
                    auto result(group_acc_.Add(name, value, pair<sender, message_id>))
                    if (result)
                      if (group_key_acc.Check(header.group + message_id))
                        retrun Result;
                  }
