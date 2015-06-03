#include <tr064/RootDeviceDiscovery.hh>
#include <tr064/serialize/SerializerJson.hh>
#include <tr064/CpeQuery.hh>
#include <tr064/RootDevice.hh>

#include <tr064/trace.hh>

// stl
#include <iostream>
#include <fstream>
#include <exception>
 
// c lib
#include <getopt.h>

#define no_argument 0
#define required_argument 1 
#define optional_argument 2

#define DEFAULT_USERNAME "admin"
#define DEFAULT_PASSWORD "gurkensalat"

using namespace tr064;

//------------------------------------------------------------------------------

const struct option longopts[] = 
{
  {"help",    no_argument,         0, 'h'},
  {"list",    no_argument,         0, 'l'},
  {"verbose", no_argument,         0, 'v'},
  {"import",  optional_argument,   0, 'i'},
  {"user",    optional_argument,   0, 'u'},
  {"password",optional_argument,   0, 'p'},
  {"location",optional_argument,   0, 'L'},
  {0,         0,                   0,  0 }
};

//------------------------------------------------------------------------------

void help()
{ 
  std::cout << "Usage: tr064tool [options] service:action [argN:valN]" << std::endl;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

RootDevice::Ptr get_root_dev_from_json(const std::string& file)
{
  RootDevice::Ptr root_device(new RootDevice());

  std::ifstream infile(file);
  if ( !infile.is_open() )
  {
    std::cerr << "Unable to open file: " << file << std::endl;
    return root_device;
  }

  serialize::SerializerJson json_importer;
  json_importer.deserialize(infile, *root_device);

  return root_device;
}

//------------------------------------------------------------------------------

void print_root_device(const RootDevice::Ptr& dev, bool with_params = false)
{
  std::cout << "--------------------------------------------" << std::endl
            << "RootDevice details:" << std::endl
            << "  locaion: " << dev->location() << std::endl
            << "  server: " << dev->server() << std::endl
            << "  cache_ctrl: " << dev->cache_ctrl() << std::endl
            << "  search_type: " << dev->search_type() << std::endl
            << "  usn: " << dev->usn() << std::endl
            << "  device_desc_doc: " << dev->device_desc_doc() << std::endl
            << "  host: " << dev->host() << std::endl
            << "  port: " << dev->port() << std::endl
            << "--------------------------------------------" << std::endl
            << "================ All devices: ==============" << std::endl;

  for ( auto cpe_dev : dev->devices() )
  {
    std::cout << "Device: " << cpe_dev->name() << std::endl;

    for ( auto cpe_service : cpe_dev->services() )
    {
      std::cout << "     Service: " << cpe_service->name() << std::endl;

      for ( auto cpe_action: cpe_service->actions() )
      {
        std::cout << "          Action: " << cpe_action->name() << std::endl;

        if ( !with_params )
          continue;

        for (auto arg : cpe_action->args() )
        { 
          std::cout << "             Param: " << 
            ServiceActionArg::direction2str(arg->direction()) << " " 
                    << arg->name() << std::endl;
        }
      }
    }
  }
  std::cout << "=============  END All devices: ============" << std::endl
            << "--------------------------------------------" << std::endl;
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  RootDevice::Ptr root_device;
  bool is_print = false;
  bool verbose = false;
  std::string user = "admin";
  std::string password = "gurkensalat";

  int index = 0, iarg=0;
  while (iarg != -1 )
  {
    iarg = getopt_long(argc, argv, "hlvi:u:p:L:", longopts, &index);

    switch (iarg)
    {
      case 'h':
        help();
        return 0;
      case 'u':
        {
          user = optarg;
          break;
        }
      case 'p':
        {
          password = optarg;
          break;
        }
      case 'i':
        {
          std::string opt_arg = optarg;
          if ( opt_arg.rfind(".json") != std::string::npos)
          {
            // TODO that kind of json handling is stupid, based on .json!!!!
            //       will be kept just for simplicity right now.
            //std::cout << "That seems to be a json file!" << std::endl;
            root_device = get_root_dev_from_json(opt_arg);
          }
          else 
          {
            std::cerr << "The export type: " << opt_arg 
                      << " is not available!" << std::endl;
            return 1;
          }
          break;
        }
      case 'L':
        {
          std::string location = optarg;
          root_device = RootDevice::Ptr(new RootDevice(location));
          root_device->init();
          break;
        }
      case 'l':
        {
          is_print = true;
          break;
        }
      case 'v':
        {
          verbose = true;
          Logging::getInstance()->level(Logging::LOG_LEVEL_TRACE);
          break;
        }
      default:
        break;
    };
  }

  // if the root device is still empty here, we obviously did not import 
  // anything, therefore perfom the discovey here.
  if ( !root_device )
  {
    std::cout << "Perform RootDevice discovey." << std::endl;

    RootDeviceDiscovery discovey;
    auto devs = discovey.discover();

    auto amount_of_devs = devs.size();

    if ( amount_of_devs > 1 )
    {
      std::cerr << "I got more then one root device; bailing out!";
    }
    else if ( amount_of_devs == 0 )
    {
      std::cerr << "I got more then one root device; bailing out!";
    }
    else
    {
      root_device = devs.at(0);
    }
  }

  // bail out if not root device was set at this point!
  if ( !root_device )
  {
    std::cerr << "No RootDevice set, unable to proceed. "
                 "(non imported, and nothing discovered.)" << std::endl;
    return 1;
  }

  if ( is_print && root_device )
  {
    print_root_device(root_device, verbose);
    return 0;
  }

  // if not service:action param set, we just print the whole device!
  if ( optind+1 > argc )
  {
    if ( root_device )
      print_root_device(root_device);
    else
      help();

    return 0;
  }

  if ( password == DEFAULT_PASSWORD )
  {
    std::cout << "Going to use default password: " 
              << DEFAULT_PASSWORD << std::endl;
  }
  if ( user == DEFAULT_USERNAME )
  {
    std::cout << "Going to use default username: " 
              << DEFAULT_USERNAME << std::endl;
  }

  std::string service_name;
  std::string action_name;

  std::string param = argv[optind];
  auto pos_delimiter = param.find(':');
  if ( pos_delimiter != std::string::npos )
  {
    service_name = param.substr(0,pos_delimiter);
    action_name = param.substr(pos_delimiter+1);

    ++optind;
  }
  else
  {
    std::cerr << "Service and Action need to be separate by ':'" << std::endl;
    help();
    return 1;
  }

  auto r = root_device->find_action_by_name(service_name, action_name);
  if ( std::get<0>(r) )
  { // found action 
    
    LOG_TRACE("Found action in service!");

    Service::Ptr service(std::get<1>(r));
    ServiceAction::Ptr action(std::get<2>(r));

    // perpare all the given arguments in the same format: name:value; not
    // only in args are ecepted here.
    while ( optind < argc )
    {
      LOG_DEBUG("Current optind: "<< optind << " argc: " << argc);
      param = argv[optind];
      pos_delimiter = param.find(':');
      if ( pos_delimiter != std::string::npos )
      {
        std::string name = param.substr(0,pos_delimiter);
        std::string value = param.substr(pos_delimiter+1);
        LOG_DEBUG("Current name: "<< name << " value: " << value);

        auto arg_search = action->find_arg_by_name(name);
        if ( arg_search.first ) 
        {
          if ( arg_search.second->direction() == ServiceActionArg::IN )
          {
            arg_search.second->value(value);
          }
          else 
          {
            std::cerr << "Skip argument (no IN arg): " << name << std::endl;
          }
        }
        else 
        {
          std::cerr << "Skip argument (not found for action): "
                    << name << std::endl;
        }
      }
      else 
      {
        std::cout << "Skip argument (unable to split): " << param << std::endl;
      }

      ++optind;
    }

    CpeQuery::Credentials c = { 
      user,
      password,
      root_device->host(),
      root_device->port()
    };
    try
    {
      CpeQuery query(service, c);
      query.execute(action);

      for (const auto& arg : action->args() )
      {
        if (arg->direction() == ServiceActionArg::OUT )
        {
          std::cout << arg->name() << ": " << arg->value() << std::endl;
        }
      }
    } 
    catch (std::exception e)
    {
      std::cerr << "An error occured: " << e.what() << std::endl;
      return 1;
    }
  }
  else
  {
    std::cerr << "Unable to find combination Service: " << service_name 
      << " with action: " << action_name << std::endl;
    return 1;
  }

  return 0;
}
