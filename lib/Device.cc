#include <tr064/Device.hh>

#include "Utils.hh"
#include "Tr064Exception.hh"

// c lib
#include <strings.h>

// boost
#include <boost/foreach.hpp>

// std lib
#include <iostream>
#include <sstream>

using namespace tr064;

//------------------------------------------------------------------------------

Device::Device()
{
  //std::cout << "in device ctor" << std::endl;
}

//------------------------------------------------------------------------------

bool
Device::empty() const
{
  return _services.empty() &&
         _name.empty() &&
         _model_name.empty() &&
         _model_desc.empty() &&
         _friendly_name.empty() &&
         _devices.empty();
}

//------------------------------------------------------------------------------

std::string
Device::dump() const
{
  std::stringstream ss;

  ss << "---------- DEVICE ----------" << std::endl;
  ss << "name: " << _name << std::endl
     << "model_name: " << _model_name << std::endl
     << "model_description: " << _model_desc << std::endl
     << "friendly_name: " << _friendly_name << std::endl;
  ss << "---------- SERVICES ----------" << std::endl;
  for ( auto& service : _services )
  {
    ss << service->dump();
  }
  ss << "---------- END SERVICES ----------" << std::endl;
  ss << "---------- SUB DEVICES ----------" << std::endl;
  for ( auto& device : _devices )
  {
    ss << device->dump();
  }
  ss << "---------- END SUB DEVICES ----------" << std::endl;

  ss << "---------- END DEVICE ----------" << std::endl;
  return ss.str();
}

//------------------------------------------------------------------------------

void 
Device::add_service(Service::Ptr& service ) 
{ 
  _services.push_back(service); 
}

//------------------------------------------------------------------------------

void 
Device::add_device(Device::Ptr& device) 
{ 
  _devices.push_back(device); 
}

//------------------------------------------------------------------------------

const Device::Vector 
Device::devices() const
{
  Device::Vector vec;

  for ( auto& dev : _devices )
  {
    auto sub_devs = dev->devices();
    vec.insert(vec.end(), sub_devs.begin(), sub_devs.end());
    vec.push_back(dev);
  }

  return vec;
}

//------------------------------------------------------------------------------
// Serializeable
//------------------------------------------------------------------------------

void
Device::serialize_json(boost::property_tree::ptree& parent_pt) const
{
  parent_pt.put("name", _name);
  parent_pt.put("model_name", _model_name);
  parent_pt.put("model_description", _model_desc);
  parent_pt.put("friendly_name", _friendly_name);

  serialize_json_container_member<Device::Vector>("devices", _devices, parent_pt);
  serialize_json_container_member<Service::Vector>("services", _services, parent_pt);
}

//------------------------------------------------------------------------------

void 
Device::deserialize_json(boost::property_tree::ptree& parent_pt)
{
  /*
   ... element that contains all devices ..
   "cpedevice" :
   {
      "name": "the name",
      "model_name": "the model name",
      ... the other properties here ...
      "services" : [
        {
          "name" : ... the service starts here  ...
        }
      ],
      "devices" : [
        {
          "name" : ... the sub device starts here  ...
        }
      ]
   }
  */
  using namespace boost::property_tree;

  find_and_set_member<Device, std::string>(
      parent_pt, this, 
      &Device::_name, "name");

  find_and_set_member<Device, std::string>(
      parent_pt, this, 
      &Device::_model_name, "model_name");

  find_and_set_member<Device, std::string>(
      parent_pt, this, 
      &Device::_model_desc, "model_description");

  find_and_set_member<Device, std::string>(
      parent_pt, this, 
      &Device::_friendly_name, "friendly_name");

  auto services = parent_pt.get_child("services", ptree());

  BOOST_FOREACH(auto& service, services)
  {
    Service::Ptr cpeservice(new Service());
    cpeservice->deserialize_json(service.second);
    if ( !cpeservice->empty() )
    {
      this->add_service(cpeservice);
    }
  }

  auto sub_devices = parent_pt.get_child("devices", ptree());

  BOOST_FOREACH(auto& sub_device, sub_devices)
  {
    Device::Ptr cpedevice(new Device());
    cpedevice->deserialize_json(sub_device.second);
    if ( !cpedevice->empty() )
    {
      this->add_device(cpedevice);
    }
  }

}

//------------------------------------------------------------------------------
