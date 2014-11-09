#ifndef SERVICEACTION_HH
#define SERVICEACTION_HH

#include "serialize/Serializable.hh"

// pugi xml
#include <pugixml/pugixml.hpp>

// stdlib
#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace tr064
{
struct ServiceActionStateVariable
{
  std::string _name;
};

// forward declaration 
class CpeElementBuilder;

//------------------------------------------------------------------------------
// ActionArg
//------------------------------------------------------------------------------

class ServiceActionArg : public serialize::Serializeable
{
public:
  enum Direction { IN, OUT, NOT_DEFINED };
  static std::string direction2str(Direction direction);

  typedef std::shared_ptr<ServiceActionArg> Ptr;
  typedef std::vector<Ptr> Vector;

  ServiceActionArg() : _direction(IN) { }

  ServiceActionArg( const std::string& name,
      Direction direction) :
    _name(name),
    _direction(direction) 
  { }

  const std::string& name() const { return _name; }

  const std::string& value() const { return _value; }
  void value(const std::string& val) { _value = val; }

  void direction(const std::string& direction) 
  {
    if ( direction == "in" )
      _direction = IN;
    else if ( direction == "out" )
      _direction = OUT;
    else // default one
      _direction = IN;
  }
  Direction direction() const { return _direction; }

  void state_variable(const ServiceActionStateVariable& state_var) 
  { _state_var = state_var; }
  const ServiceActionStateVariable& state_variable() const 
  { return _state_var; }

  //--- Serializeable
  virtual void serialize_json(boost::property_tree::ptree& parent_pt) const;
  virtual void deserialize_json(boost::property_tree::ptree& parent_pt);
  //-----
private:
  std::string _name;
  std::string _value;
  Direction _direction;
  ServiceActionStateVariable _state_var;

  friend CpeElementBuilder;
};

//------------------------------------------------------------------------------
// Action
//------------------------------------------------------------------------------

class ServiceAction : public serialize::Serializeable
{
public: 
  typedef std::shared_ptr<ServiceAction> Ptr;
  typedef std::vector<Ptr> Vector;
  typedef std::pair<bool, ServiceActionArg::Ptr> ArgSearchResult;


  ServiceAction() { }
  ServiceAction(const std::string& name) : _name(name) { }

  bool empty() const;

  const std::string& name() const { return _name; }

  void add_arg(ServiceActionArg::Ptr& arg);
  const ServiceActionArg::Vector& args() const { return _args; }

  ArgSearchResult find_arg_by_name(const std::string& arg) const;

  //--- Serializeable
  virtual void serialize_json(boost::property_tree::ptree& parent_pt) const;
  virtual void deserialize_json(boost::property_tree::ptree& parent_pt);
  //-----
private:
  std::string _name;
  ServiceActionArg::Vector _args;

  friend CpeElementBuilder;
};



} // namespace     

#endif /* SERVICEACTION_HH */
