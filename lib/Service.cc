#include <tr064/Service.hh>
#include <tr064/ResourceRetrieval.hh>

#include "Tr064Exception.hh"
#include "Utils.hh"

// c lib
#include <strings.h>

// boost
#include <boost/foreach.hpp>

// std lib
#include <iostream>
#include <sstream>

using namespace tr064;

//------------------------------------------------------------------------------

Service::Service()
{

}

//------------------------------------------------------------------------------

bool
Service::empty() const
{
  return _type.empty() &&  
         _name.empty() && 
         _id.empty() &&
         _control_url.empty() &&
         _event_subscription_url.empty() &&
         _scpd_url.empty() &&
         _actions.empty();
}

//------------------------------------------------------------------------------

std::string
Service::dump() const
{
  std::stringstream ss;
  ss << "---------- SERVICE ----------" << std::endl
     << "type: " << _type << std::endl
     << "name: " << _name << std::endl
     << "id: " << _id << std::endl
     << "control_url: " << _control_url << std::endl
     << "event_sub_url: " << _event_subscription_url << std::endl
     << "scpd_url: " << _scpd_url << std::endl
     << "---------- END SERVICE ----------" << std::endl;

  return ss.str();
}
//------------------------------------------------------------------------------

Service::ActionSearchResult 
Service::find_action_by_name(const std::string& name) const
{
  for ( auto action : _actions )
  {
    if ( action->name() == name )
    {
      return std::make_tuple(true, action);
    }
  }

  return std::make_tuple(true, ServiceAction::Ptr());
}

//------------------------------------------------------------------------------
// Serializeable
//------------------------------------------------------------------------------

void
Service::serialize_json(boost::property_tree::ptree& parent_pt) const
{
  parent_pt.put("type", _type);
  parent_pt.put("name", _name);
  parent_pt.put("id", _id);
  parent_pt.put("control_url", _control_url);
  parent_pt.put("event_subscription_url", _event_subscription_url);
  parent_pt.put("scpd_url", _scpd_url);

  serialize_json_container_member<ServiceAction::Vector>(
      "actions", _actions, parent_pt);
}

//------------------------------------------------------------------------------

void 
Service::deserialize_json(boost::property_tree::ptree& parent_pt)
{
  /*
   * {
   *   "name" : "value",
   *   "another" : "value",
   *   ...
   * }
   */
  find_and_set_member<Service, std::string>(
      parent_pt, this, 
      &Service::_type, "type");
  find_and_set_member<Service, std::string>(
      parent_pt, this, 
      &Service::_name, "name");
  find_and_set_member<Service, std::string>(
      parent_pt, this, 
      &Service::_id, "id");
  find_and_set_member<Service, std::string>(
      parent_pt, this, 
      &Service::_control_url, "control_url");
  find_and_set_member<Service, std::string>(
      parent_pt, this, 
      &Service::_event_subscription_url, "event_subscription_url");
  find_and_set_member<Service, std::string>(
      parent_pt, this, 
      &Service::_scpd_url, "scpd_url");

  auto actions =
    parent_pt.get_child("actions", boost::property_tree::ptree());

  BOOST_FOREACH(auto& action, actions)
  {
    ServiceAction::Ptr cpeaction(new ServiceAction());
    cpeaction->deserialize_json(action.second);
    if ( !cpeaction->empty() )
    {
      this->add_action(cpeaction);
    }
  }
}
