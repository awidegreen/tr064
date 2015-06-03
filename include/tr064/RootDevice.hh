#ifndef ROOTDEVICE_HH
#define ROOTDEVICE_HH

#include "CpeElementContainer.hh"
#include "serialize/Serializable.hh"

#include <string>
#include <vector>
#include <tuple>

namespace tr064
{

class RootDevice : 
  public CpeElementContainer,
  public serialize::Serializeable
{
public:
  typedef std::shared_ptr<RootDevice> Ptr;
  typedef std::vector<Ptr> Vector;

  RootDevice();
  RootDevice(const std::string& location);
  RootDevice(const std::string& location, const std::string& server,
             const std::string& cache_ctrl, const std::string& search_type,
             const std::string& usn, const std::string& device_desc_doc,
             const std::string& host, const std::string& port,Device* dev) :
    _location(location),
    _server(server),
    _cache_control(cache_ctrl),
    _search_type(search_type),
    _usn(usn),
    _device_desc_doc(device_desc_doc),
    _host(host),
    _port(port),
    _cpe_device(dev)
  { }

  ~RootDevice();

  std::string dump() const;
  void init(); 

  const std::string& location() const { return _location; }
  const std::string& server() const { return _server; }
  const std::string& cache_ctrl() const { return _cache_control; }
  const std::string& search_type() const { return _search_type; }
  const std::string& usn() const { return _usn; }
  const std::string& device_desc_doc() const { return _device_desc_doc; }
  const std::string& host() const { return _host; }
  const std::string& port() const { return _port; }
  const Device::Ptr& cpe_device() const { return _cpe_device; }

  const Service::Vector services() const;
  const Device::Vector devices() const;
  const ServiceAction::Vector actions() const;

  //--- find functions
  typedef std::tuple<bool, Service::Ptr, ServiceAction::Ptr> ActionSearchResult;
  typedef std::tuple<bool, Service::Ptr> ServiceSearchResult;

  ActionSearchResult find_action_by_name(const std::string& name) const;
  ActionSearchResult find_action_by_name(
      const std::string& service_name, 
      const std::string& action_name) const;
  ServiceSearchResult find_service_by_name(const std::string& name) const;
  //-----

  //--- inherit from CpeElementContainer
  void add_device(const Device::Ptr& /* device */) { }
  void add_service(const Service::Ptr& /* service */) { }
  void add_action(const ServiceAction::Ptr& /* action */) { }
  //-----

  static Ptr create_root_device_from_http_buf(const char* buffer, size_t len);

  //--- Serializeable
  virtual void serialize_json(boost::property_tree::ptree& parent_pt) const;
  virtual void deserialize_json(boost::property_tree::ptree& parent_pt);
  //-----

private:
  std::string _location;
  std::string _server;
  std::string _cache_control;
  std::string _search_type; 
  std::string _usn;
  std::string _device_desc_doc;
  std::string _host;
  std::string _port;
  std::vector<std::string> _extentions;

  Device::Ptr _cpe_device;
};


} // namespace 

#endif /* ROOTDEVICE_HH */
