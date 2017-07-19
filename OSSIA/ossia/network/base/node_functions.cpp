#include "node_functions.hpp"
#include <ossia/network/common/path.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace ossia
{
namespace net
{
namespace
{
static node_base* find_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto child = node.find_child(address.substr(0, first_slash_index));
    if (child)
    {
      // There are still nodes since we found a slash
      return find_node_rec(*child, address.substr(first_slash_index + 1));
    }
    else
    {
      return nullptr;
    }
  }
  else
  {
    // One of the child may be the researched node.
    return node.find_child(address);
  }
}

static node_base& find_or_create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if (cld)
    {
      // There are still nodes since we found a slash
      return find_or_create_node_rec(
          *cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto& child = *node.create_child(std::string(cur));

      // Recurse on it
      return find_or_create_node_rec(
          child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // One of the child may be the researched node.
    auto n = node.find_child(address);
    if (n)
    {
      return *n;
    }
    else
    {
      // Create and return the node
      return *node.create_child(std::string(address));
    }
  }
}

static node_base& create_node_rec(
    node_base& node,
    ossia::string_view address) // Format a/b/c -> b/c -> c
{
  auto first_slash_index = address.find_first_of('/');

  if (first_slash_index != std::string::npos)
  {
    auto cur = address.substr(0, first_slash_index);
    auto cld = node.find_child(cur);
    if (cld)
    {
      // There are still nodes since we found a slash
      return create_node_rec(*cld, address.substr(first_slash_index + 1));
    }
    else
    {
      // Create a node
      auto child = node.create_child(std::string(cur));
      if(!child)
        throw std::runtime_error{"create_node_rec: cannot create the node"};

      // Recurse on it
      return create_node_rec(*child, address.substr(first_slash_index + 1));
    }
  }
  else
  {
    // Create and return the node
    auto child = node.create_child(std::string(address));
    if(!child)
      throw std::runtime_error{"create_node_rec: cannot create the node"};
    return *child;
  }
}

//! Note : here we modify the string_view only.
//! The original address remains unchanged.
static ossia::string_view sanitize_address(ossia::string_view address)
{
  while (boost::algorithm::starts_with(address, "/"))
    address.remove_prefix(1);
  while (boost::algorithm::ends_with(address, "/"))
    address.remove_suffix(1);
  return address;
}
}

node_base* find_node(node_base& dev, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return &dev;

  // address now looks like a/b/c
  return find_node_rec(dev, address);
}

node_base& find_or_create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    return node;

  // address now looks like a/b/c
  return find_or_create_node_rec(node, address);
}

node_base& create_node(node_base& node, ossia::string_view address)
{
  // TODO validate
  address = sanitize_address(address);
  if (address.empty())
    address = "node";

  // address now looks like a/b/c
  return create_node_rec(node, address);
}

node_base*
find_or_create_node(node_base& dev, string_view address_base, bool create)
{
  if (create)
  {
    return ossia::net::find_node(dev, address_base);
  }
  else
  {
    return &ossia::net::create_node(dev, address_base);
  }
}

std::vector<node_base*> find_nodes(node_base& dev, string_view pattern)
{
  if(auto path = traversal::make_path(std::string(pattern)))
  {
    std::vector<node_base*> nodes{&dev};
    traversal::apply(*path, nodes);
    return nodes;
  }
  else
  {
    return {};
  }
}

}
}