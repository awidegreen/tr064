#ifndef CPEELEMENTBUILDER_HH
#define CPEELEMENTBUILDER_HH

// tr064
#include "ResourceRetrieval.hh"
#include "CpeElementContainer.hh"
#include "Device.hh"
#include "Service.hh"
#include "ServiceAction.hh"

// pugi xml
#include <pugixml/pugixml.hpp>

namespace tr064
{

class CpeElementBuilder
{
public:
  CpeElementBuilder(const ResourceRetreival::Ptr& resource_retreival,
                    CpeElementContainer* container) :
     _resource_retreival(resource_retreival),
    _container(container)
  { }

  /**
   * Used for paring an tr064desc.xml document, represented as ResourceStream.
   * @param stream The xml desc document.
   */
  Device::Ptr 
  parse_root(const ResourceStream& stream);

  /**
   * Parse a device with given xml element. Note that the device will 
   * be added to the CpeElementContainer, @see _container.
   * 
   * @param device_xml_node Is the xml node that contains the data data.
   */
  Device::Ptr
  parse_cpe_device(const pugi::xml_node& device_xml_node);

  /**
   * Parse a service with given xml element. Note that the service will 
   * be added to the CpeElementContainer, @see _container.
   * 
   * @param owning_device Is the Device that owns the service which
   *        is gonna be parsed.
   * @param service_xml_node Is the xml node that contains the service data.
   */
  Service::Ptr 
  parse_cpe_service(const pugi::xml_node& service_xml_node);


  // TODO some description here
  ServiceAction::Ptr
  parse_cpe_service_action(const pugi::xml_node& action_xml_node);
   
  // TODO some description here
  ServiceActionArg::Ptr
  parse_cpe_service_action_argument(const pugi::xml_node& arg_xml_node);

private:
  const ResourceRetreival::Ptr _resource_retreival;
  CpeElementContainer* _container;
};

}

#endif /* CPEELEMENTBUILDER_HH */
