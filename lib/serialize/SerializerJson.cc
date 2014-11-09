#include <tr064/serialize/SerializerJson.hh>

// boost 
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace tr064::serialize;

//------------------------------------------------------------------------------

void 
SerializerJson::serialize(std::ostream& out, const Serializeable& root) const
{
  boost::property_tree::ptree pt;

  root.serialize_json(pt);

  boost::property_tree::json_parser::write_json(out, pt);

}

//------------------------------------------------------------------------------

void 
SerializerJson::deserialize(std::istream& in, Serializeable& root) const
{
  boost::property_tree::ptree pt;

  boost::property_tree::json_parser::read_json(in, pt);

  root.deserialize_json(pt);
}

//------------------------------------------------------------------------------
