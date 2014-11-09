#ifndef SERIALIZABLE_HH
#define SERIALIZABLE_HH

// boost
#include <boost/property_tree/ptree.hpp>

namespace tr064
{
namespace serialize
{

class Serializeable
{
public:
  virtual void serialize_json(boost::property_tree::ptree& parent_pt) const = 0;
  virtual void deserialize_json(boost::property_tree::ptree& parent_pt) = 0;

  // TODO add xml serialize and deserialize

protected:
  Serializeable() { }
  virtual ~Serializeable() { }

  template<class T, class MemberType> 
  void
  find_and_set_member(
      const boost::property_tree::ptree& pt,
      T* instance,
      MemberType T::*member, 
      const std::string& json_key);

  template<class Container>
  void 
  serialize_json_container_member(const std::string& container_name, 
                                  const Container& container,
                                  boost::property_tree::ptree& parent) const;

private:
  // hide assignement and cctor
  Serializeable(const Serializeable&) { }
  Serializeable& operator=(const Serializeable&) { }
};

//------------------------------------------------------------------------------

template<class T, class MemberType> 
void
Serializeable::find_and_set_member(
    const boost::property_tree::ptree& pt,
    T* instance,
    MemberType T::*member, 
    const std::string& json_key)
{
  auto it = pt.find(json_key);
  if ( it != pt.not_found() )
  {
    instance->*member = it->second.data();
  }
  //else 
  //{
    //std::cerr << "key '" << json_key << "' could not be found" << std::endl;
  //}
}

//------------------------------------------------------------------------------

template<class Container>
void 
Serializeable::serialize_json_container_member(
    const std::string& container_name, 
    const Container& container,
    boost::property_tree::ptree& parent) const
{
  if ( container.size() )
  {
    boost::property_tree::ptree pt_container_element;
    for ( auto& child : container )
    {
      boost::property_tree::ptree pt_child;
      child->serialize_json(pt_child);
      pt_container_element.push_back(std::make_pair("", pt_child));
    }
    parent.add_child(container_name, pt_container_element);
  }
}

//------------------------------------------------------------------------------

} // ns
} // ns

#endif /* SERIALIZABLE_HH */
