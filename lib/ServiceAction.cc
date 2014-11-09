#include <tr064/ServiceAction.hh>

// c lib
#include <strings.h>
 
// boost
#include <boost/foreach.hpp>

// stdlib
#include <iostream>

using namespace tr064;

//------------------------------------------------------------------------------
// CLASS ServiceAction
//------------------------------------------------------------------------------

void 
ServiceAction::add_arg(ServiceActionArg::Ptr& arg)
{
  _args.push_back(std::move(arg)); 
}

//------------------------------------------------------------------------------

bool
ServiceAction::empty() const
{
  bool res = _name.empty();
  res &= _args.empty();

  return res;
}

//------------------------------------------------------------------------------

/* static */
std::string 
ServiceActionArg::direction2str(Direction direction)
{
  switch (direction)
  {
    case IN: return "in";
    case OUT: return "out";
    default: return "not_defined";
  }
}

//------------------------------------------------------------------------------


ServiceAction::ArgSearchResult
ServiceAction::find_arg_by_name(const std::string& arg_name) const
{
  for ( auto arg : _args )
  {
    if ( arg->name() == arg_name )
    {
      return ArgSearchResult(true, arg);
    }
  }

  return ArgSearchResult(false, ServiceActionArg::Ptr());
}

//------------------------------------------------------------------------------
// Serializeable Action
//------------------------------------------------------------------------------

void
ServiceAction::serialize_json(boost::property_tree::ptree& parent_pt) const
{
  parent_pt.put("name", _name);

  serialize_json_container_member<ServiceActionArg::Vector>(
      "arguments", _args, parent_pt);
}

//------------------------------------------------------------------------------

void 
ServiceAction::deserialize_json(boost::property_tree::ptree& parent_pt)
{
  ServiceAction::Ptr action(new ServiceAction());

  find_and_set_member<ServiceAction, std::string>(
      parent_pt, this, 
      &ServiceAction::_name, "name");

  auto args = parent_pt.get_child("arguments", boost::property_tree::ptree());
  BOOST_FOREACH(auto& arg, args)
  {
    ServiceActionArg::Ptr cpeactionarg(new ServiceActionArg());
    cpeactionarg->deserialize_json(arg.second);
    if ( cpeactionarg->name() != "" )
    { // if no name is set, we have a problem anyway.
      this->add_arg(cpeactionarg);
    }
    // else TODO what shall we do if it is not empty?
  }
}

//------------------------------------------------------------------------------
// Serializeable ActionArg
//------------------------------------------------------------------------------

void
ServiceActionArg::serialize_json(boost::property_tree::ptree& parent_pt) const
{
  parent_pt.put("name", _name);
  parent_pt.put("direction", direction2str(_direction));
}

//------------------------------------------------------------------------------

void 
ServiceActionArg::deserialize_json(boost::property_tree::ptree& parent_pt)
{
  find_and_set_member<ServiceActionArg, std::string>(
      parent_pt, this, &ServiceActionArg::_name, "name");

  auto it = parent_pt.find("direction");
  if ( it != parent_pt.not_found() )
  {
    this->direction(it->second.data());
  }
  // TODO State var should be added too
}
