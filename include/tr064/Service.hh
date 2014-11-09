#ifndef SERVICE_HH
#define SERVICE_HH

// tr064
#include "ServiceAction.hh"
#include "serialize/Serializable.hh"

// pugixml
#include <pugixml/pugixml.hpp>

// std lib
#include <vector>
#include <memory>
#include <string>
#include <tuple>

namespace tr064
{

// forward declaration for friend class
class CpeElementBuilder;

class Service : public serialize::Serializeable
{
public:
  typedef std::shared_ptr<Service> Ptr;
  typedef std::vector<Ptr> Vector;

  Service();
  // ctor with init values
  // ctor with init values
  Service(const std::string& type,
             const std::string& name,
             const std::string& id,
             const std::string& control_url,
             const std::string& event_sub_url,
             const std::string& scpd_url) :
    _type(type),
    _name(name),
    _id(id),
    _control_url(control_url),
    _event_subscription_url(event_sub_url),
    _scpd_url(scpd_url)
  { }


  std::string dump() const;
  bool empty() const;

  const ServiceAction::Vector& actions() const 
  { 
    return _actions; 
  }

  void add_action(ServiceAction::Ptr& action) 
  { 
    _actions.push_back( std::move(action) ); 
  }

  //--- getters - no setters needed!
  const std::string& type() const { return _type; }
  const std::string& name() const { return _name; }
  const std::string& id() const { return _id; }
  const std::string& control_url() const { return _control_url; }
  const std::string& event_sub_url() const { return _event_subscription_url; }
  const std::string& scpd_url() const { return _scpd_url; }
  //-----

  //--- find functions
  typedef std::tuple<bool, ServiceAction::Ptr> ActionSearchResult;
  ActionSearchResult find_action_by_name(const std::string& name) const;
  //-----
  
  //--- Serializeable
  virtual void serialize_json(boost::property_tree::ptree& parent_pt) const;
  virtual void deserialize_json(boost::property_tree::ptree& parent_pt);
  //-----
private:
  // hide Assignment operator and cctor
  Service(const Service& );
  Service& operator=(const Service& );

  ServiceAction::Vector _actions;


  std::string _type;
  std::string _name;
  std::string _id;
  std::string _control_url;
  std::string _event_subscription_url;
  std::string _scpd_url;

  friend CpeElementBuilder;
};

}

#endif /* SERVICE_HH */
