// tr064
#include <tr064/CpeElementBuilder.hh>
#include <tr064/Device.hh>
#include <tr064/Service.hh>
#include <tr064/ServiceAction.hh>
 
#include "Utils.hh"
#include "Tr064Exception.hh"

// c lib
#include <strings.h>

// std lib
#include <iostream>
#include <sstream>

using namespace tr064;

//------------------------------------------------------------------------------

Device::Ptr 
CpeElementBuilder::parse_root(const ResourceStream& stream)
{
  pugi::xml_document doc;
  auto r = doc.load(*stream);

  if ( r.status != pugi::status_ok )
  {
    std::string msg = "can not read root document: ";
    msg.append(r.description());
    throw Tr064Exception( msg );
  }

  auto xml_device = doc.child("root").child("device");
  if ( xml_device.empty() )
  {
    throw Tr064Exception("Unable to find RootDevice in stream.");
  }
  return parse_cpe_device(xml_device);
}

//------------------------------------------------------------------------------

Device::Ptr 
CpeElementBuilder::parse_cpe_device(const pugi::xml_node& device_xml_node)
{
  Device::Ptr device(new Device());

  if ( strcasecmp("device", device_xml_node.name()) != 0 )
  {
    std::cerr << "this is not a device node." << std::endl;
    return device;
  }

  for ( auto property_node : device_xml_node.children() )
  {
    const char* name = property_node.name();
    //std::cerr << "current propoerty: " << name << std::endl;

    if ( strcasecmp("serviceList", name) == 0 )
    {
      for ( auto sub_device_node : property_node.children() )
      {
        auto service = parse_cpe_service(sub_device_node);
        device->add_service(service);
      }

    }
    else if ( strcasecmp("deviceList", name) == 0 )
    {
      for ( auto sub_device_node : property_node.children() )
      {
        auto sub_device = parse_cpe_device(sub_device_node);
        device->add_device(sub_device);
      }
    }
    else if ( strcasecmp("deviceType", name) == 0)
      device->_name = extract_type_name(property_node.child_value());
    else if ( strcasecmp("modelName", name) == 0)
      device->_model_name = property_node.child_value();
    else if ( strcasecmp("modelDescription", name) == 0)
      device->_model_desc = property_node.child_value();
    else if ( strcasecmp("friendlyName", name) == 0)
      device->_friendly_name = property_node.child_value();
    else 
    {
      // TODO some better logging would be nice, I guess.
      //std::clog << "Device: Ignoring element: " 
                //<< property_node.child_value() 
                //<< " while parsing" << std::endl;
    }
  }

  _container->add_device(device);
  return device;
}

//------------------------------------------------------------------------------

Service::Ptr
CpeElementBuilder::parse_cpe_service(const pugi::xml_node& service_xml_node)
{
  Service::Ptr service(new Service());

  if ( strcasecmp("service", service_xml_node.name()) != 0 )
  {
    std::cerr << "this is not a service node." << std::endl;
    return service;
  }

  for ( auto property_node : service_xml_node.children() )
  {
    const char* name = property_node.name();
    //std::cerr << "current propoerty: " << name << std::endl;

    if ( strcasecmp("serviceType", name) == 0 )
    {  
      auto val = property_node.child_value();
      service->_type = val;
      service->_name = extract_type_name(val);
    }
    else if ( strcasecmp("serviceId", name) == 0 )
      service->_id = property_node.child_value();
    else if ( strcasecmp("controlURL", name) == 0 )
      service->_control_url = property_node.child_value();
    else if ( strcasecmp("eventSubURL", name) == 0 )
      service->_event_subscription_url = property_node.child_value();
    else if ( strcasecmp("SCPDURL", name) == 0 )
      service->_scpd_url = property_node.child_value();
    else 
    {
      std::clog << "Service: Ignoring element: " 
                << property_node.child_value() 
                << " while parsing" << std::endl;
    }
  }

  auto& scpd_url =  service->scpd_url(); 
  if ( service->_scpd_url.empty() )
  {
    std::cerr << "Service does not have a SCPDURL "
              << "porperty, unable to continue init service." << std::endl;
    return service;
  }

  auto&& stream = _resource_retreival->get(scpd_url);

  pugi::xml_document doc;
  auto r = doc.load(*stream);

  if ( r.status != pugi::status_ok )
  {
    std::cerr << r.description() << std::endl;
    //throw Tr064Exception(r.description());
  }

  auto xml_action_list = doc.child("scpd").child("actionList");

  for ( auto xml_action_node : xml_action_list.children() )
  {
    auto action = parse_cpe_service_action(xml_action_node);
    service->add_action(action);
  }

  _container->add_service(service);
  return service;
}
 
//------------------------------------------------------------------------------

ServiceAction::Ptr
CpeElementBuilder::parse_cpe_service_action(const pugi::xml_node& action_xml_node)
{
  ServiceAction::Ptr action(new ServiceAction());

  if ( strcasecmp("action", action_xml_node.name()) != 0 )
  {
    std::cerr << "this is not a action node." << std::endl;
    return action;
  }

  auto name_child = action_xml_node.child("name");
  if ( !name_child.empty() )
  {
    action->_name = (name_child.child_value());
  }

  for ( auto arg_child_node : action_xml_node.child("argumentList").children() )
  {
    // Note: argument action is added to action via action parameter
    auto arg = parse_cpe_service_action_argument(arg_child_node);
    action->add_arg(arg);
  }

  _container->add_action(action);
  return action;
}

//------------------------------------------------------------------------------

ServiceActionArg::Ptr 
CpeElementBuilder::parse_cpe_service_action_argument(const pugi::xml_node& arg_xml_node)
{
  ServiceActionArg::Ptr argument(new ServiceActionArg());

  if ( strcasecmp("argument", arg_xml_node.name()) != 0 )
  {
    std::cerr << "this is not a argument node." << std::endl;
    return argument;
  }

  // set name
  auto child = arg_xml_node.child("name");
  argument->_name = child.child_value();

  // set direction
  child = arg_xml_node.child("direction");
  argument->direction(child.child_value());

  // TODO related State variable should be parsed here.

  return argument;
}
