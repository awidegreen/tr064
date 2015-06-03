#include <tr064/RootDevice.hh>
#include <tr064/ResourceRetrieval.hh>
#include <tr064/CpeElementBuilder.hh>
#include <tr064/trace.hh>

// std lib
#include <algorithm>
#include <iostream>
#include <sstream>

// boost
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>

//------------------------------------------------------------------------------

using namespace tr064;

//------------------------------------------------------------------------------

RootDevice::Ptr
RootDevice::create_root_device_from_http_buf(const char* buffer, size_t len)
{
  Ptr device(new RootDevice);

  std::string data(buffer, len);
  std::vector<std::string> lines;
  boost::char_separator<char> sep("\n");
  boost::tokenizer<boost::char_separator<char> > tokens(data, sep);

  for (auto line : tokens )
  {
    size_t pos = line.find(':');
    if ( pos == std::string::npos )
      continue;

    std::string header_type = line.substr(0, pos);
    std::transform(header_type.begin(), header_type.end(), 
                   header_type.begin(), ::tolower);
    boost::to_lower(header_type);
    std::string header_val = line.substr(pos+1);
    boost::trim(header_val);

    if ( header_type == "location" )
      device->_location = header_val;
    else if ( header_type == "server" )
      device->_server = header_val;
    else if ( header_type == "cache-control" )
      device->_cache_control = header_val;
    else if ( header_type == "st" )
      device->_search_type = header_val;
    else if ( header_type == "usn" )
      device->_usn = header_val;
    //else if ( header_type == "ext" )
      //void(); // nop here
    else
      LOG_DEBUG("header element ignored: " << line);
  }

  device->init();

  return device;
}
//------------------------------------------------------------------------------

RootDevice::RootDevice(const std::string& location) :
  _location(location),
  _cpe_device(new Device())
{
}

//------------------------------------------------------------------------------

RootDevice::RootDevice() : _cpe_device(new Device())
{
}

//------------------------------------------------------------------------------

RootDevice::~RootDevice()
{
}

//------------------------------------------------------------------------------

std::string
RootDevice::dump() const
{
  std::stringstream ss;
  ss <<
    "location: " << _location << std::endl <<
    "server: " << _server << std::endl << 
    "cache_control: " << _cache_control << std::endl <<
    "search_type: " << _search_type << std::endl <<
    "usn: " << _usn << std::endl <<
    "device_desc_doc: " << _device_desc_doc << std::endl <<
    "host: " << _host << std::endl <<
    "# sub_devices: " << _cpe_device->devices().size() << std::endl;
  ss << _cpe_device->dump();


  return ss.str();
}

//------------------------------------------------------------------------------

void
RootDevice::init() 
{
  // set hostname and device description resource
  size_t pos_1 = _location.rfind("/");
  size_t pos_2 = _location.rfind("/", pos_1-1);

  _device_desc_doc = _location.substr(pos_1);
  std::string tmp_host = _location.substr(pos_2+1, pos_1-pos_2-1);

  auto pos = tmp_host.find(':');
  if ( pos != std::string::npos )
  {
    LOG_DEBUG("Seems that host and port are together, splitting them");
    _port = tmp_host.substr(pos+1);
    _host = tmp_host.substr(0, pos);
  }
  else
    _host = tmp_host;
  LOG_DEBUG("after split: location: " << _location << " host: " << _host << " port: " << _port);

  ResourceRetreival::Ptr retrieval(new ResourceRetreivalHttp(_host, _port));

  auto&& stream = retrieval->get(_device_desc_doc);

  CpeElementBuilder builder(retrieval, this);

  _cpe_device = builder.parse_root(stream);
}

//------------------------------------------------------------------------------

const Device::Vector
RootDevice::devices() const
{
  Device::Vector vec;

  if ( !_cpe_device->empty() )
  {
    vec.push_back(_cpe_device);
    auto sub_devs = _cpe_device->devices();
    vec.insert(vec.end(), sub_devs.begin(), sub_devs.end());
  }

  return vec;
}

//------------------------------------------------------------------------------

const Service::Vector
RootDevice::services() const
{
  Service::Vector vec;

  for ( auto& device : devices() )
  {
    auto services = device->services();
    vec.insert(vec.end(), services.begin(), services.end());
  }

  return vec;
}

//------------------------------------------------------------------------------

const ServiceAction::Vector
RootDevice::actions() const
{
  ServiceAction::Vector vec;

  for ( auto& service : services() )
  {
    auto actions = service->actions();
    vec.insert(vec.end(), actions.begin(), actions.end());
  }

  return vec;
}

//------------------------------------------------------------------------------

RootDevice::ActionSearchResult 
RootDevice::find_action_by_name(const std::string& name) const
{
  for ( auto service : services() )
  {
    auto r = service->find_action_by_name(name);
    if ( std::get<0>(r) ) // found
    {
      // index 1 is action
      return std::make_tuple(true, service, std::get<1>(r));
    }
  }

  return std::make_tuple(false, Service::Ptr(), ServiceAction::Ptr());
}

//------------------------------------------------------------------------------

RootDevice::ActionSearchResult 
RootDevice::find_action_by_name(
    const std::string& service_name, 
    const std::string& action_name) const
{
  auto r_service = find_service_by_name(service_name);
  if ( std::get<0>(r_service) )
  { // found service
    auto service = std::get<1>(r_service);
    auto r_action = service->find_action_by_name(action_name);
    if ( std::get<0>(r_action) )
    {
      auto action = std::get<1>(r_action);
      return std::make_tuple(true, service, action);
    }
  }

  return std::make_tuple(false, Service::Ptr(), ServiceAction::Ptr());
}

//------------------------------------------------------------------------------

RootDevice::ServiceSearchResult 
RootDevice::find_service_by_name(const std::string& name) const
{
  for ( auto service : services() )
  {
    if ( service->name() == name )
    {
      return std::make_tuple(true, service);
    }
  }

  return std::make_tuple(false, Service::Ptr());
}

//------------------------------------------------------------------------------
// Serializeable
//------------------------------------------------------------------------------

void
RootDevice::serialize_json(boost::property_tree::ptree& parent_pt) const
{
  parent_pt.put("location", _location);
  parent_pt.put("server", _server);
  parent_pt.put("cache_control", _cache_control);
  parent_pt.put("search_type", _search_type);
  parent_pt.put("usn", _usn);
  parent_pt.put("device_desc_doc", _device_desc_doc);
  parent_pt.put("host", _host);
  parent_pt.put("port", _port);

  if ( _extentions.size() )
  {
    boost::property_tree::ptree pt_container_element;
    for ( auto& child : _extentions )
    {
      boost::property_tree::ptree pt_child;
      pt_child.put("", child);
      pt_container_element.push_back(std::make_pair("", pt_child));
    }
    parent_pt.add_child("extentions", pt_container_element);
  }

  if ( !_cpe_device->empty() )
  {
    boost::property_tree::ptree pt_dev;
    _cpe_device->serialize_json(pt_dev);
    parent_pt.add_child("device", pt_dev);
  }
}

//------------------------------------------------------------------------------

void 
RootDevice::deserialize_json(boost::property_tree::ptree& parent_pt)
{
  using namespace boost::property_tree;

  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_location, "location");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_server, "server");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_cache_control, "cache_control");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_search_type, "search_type");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_usn, "usn");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_device_desc_doc, "device_desc_doc");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_host, "host");
  find_and_set_member<RootDevice, std::string>(
      parent_pt, this, &RootDevice::_port, "port");

  auto extentions = parent_pt.get_child("extentions", ptree());
  BOOST_FOREACH(auto& extention, extentions)
  {
    _extentions.push_back(extention.second.data());
  }

  auto it = parent_pt.find("device");
  if ( it != parent_pt.not_found() )
  {
    if ( !_cpe_device )
      _cpe_device = Device::Ptr(new Device());
    if ( !_cpe_device->empty() )
      _cpe_device.reset();

    _cpe_device->deserialize_json(it->second);
  }
}

//------------------------------------------------------------------------------

