#ifndef DEVICE_HH
#define DEVICE_HH

#include "Service.hh"
#include "serialize/Serializable.hh"

// pugixml
#include <pugixml/pugixml.hpp>

// std lib
#include <memory>
#include <vector>
#include <string>

namespace tr064
{
// forward declaration for friend
class CpeElementBuilder;

struct DeviceIcon
{
  std::string mime_type;
  std::string width;
  std::string height;
  std::string url;
};

//------------------------------------------------------------------------------t

class Device : public serialize::Serializeable
{
public:   
  typedef std::shared_ptr<Device> Ptr;
  typedef std::vector<Ptr> Vector;

  struct ServiceFindResult
  {
    bool found;
    Service::Ptr service;
  };
  
  Device();
  // ctor to init all values
  Device(const std::string& name, 
            const std::string& model_name = "",
            const std::string& model_description = "",
            const std::string& friendly_name = "") :
    _name(name),
    _model_name(model_name),
    _model_desc(model_description),
    _friendly_name(friendly_name) 
  { }

  void add_service(Service::Ptr& service);
  void add_device(Ptr& device);

  const Service::Vector& services() const { return _services; }
  const Vector devices() const;

  bool empty() const;
  std::string dump() const;

  //--- getters - no setters needed
  const std::string& name() const { return _name; }
  const std::string& model_name() const { return _model_name; }
  const std::string& model_description() const { return _model_desc; }
  const std::string& friendly_name() const { return _friendly_name; }
  //-----

  //--- Serializeable
  virtual void serialize_json(boost::property_tree::ptree& parent_pt) const;
  virtual void deserialize_json(boost::property_tree::ptree& parent_pt);
  //-----
private:
  // hide Assignment operator and cctor
  Device(const Device& );
  Device& operator=(const Device& );

  std::vector<DeviceIcon> _icons;

  Service::Vector _services;
  Vector _devices;

  std::string _name;
  std::string _model_name;
  std::string _model_desc;
  std::string _friendly_name;

  friend CpeElementBuilder;
};

} // namespace

#endif /* DEVICE_HH */
