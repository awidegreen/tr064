#include <tr064/CpeQuery.hh>
#include "soap/Session.hh"

// stl
#include <iostream>

using namespace tr064;

//------------------------------------------------------------------------------

void
CpeQuery::execute(const ServiceAction::Ptr& action) const
{
  soap::Session soap(
      _credentials.host,
      _credentials.port,
      _credentials.username,
      _credentials.password);
  auto body_node = soap.execute_action(_service, action);

  std::string response_node_name = "u:" + action->name() + "Response";

  //std::cerr << "Search for response node: " << response_node_name << std::endl;

  auto response_node = body_node.child(response_node_name.c_str());

  for ( auto out_arg_node : response_node.children() )
  {                                             
    std::string name = out_arg_node.name();
    auto arg_search = action->find_arg_by_name(name);
    if ( !arg_search.first )
    {
      std::cerr << name << " is not part of the Action: "
                << action->name() << std::endl;
    }
    else 
    {
      arg_search.second->value(out_arg_node.child_value());
    }

  }
}

//------------------------------------------------------------------------------


