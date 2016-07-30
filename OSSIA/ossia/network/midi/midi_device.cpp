#include "midi_device.hpp"
#include <ossia/network/midi/detail/midi_impl.hpp>
#include <ossia/network/midi/midi_protocol.hpp>

namespace ossia
{
namespace net
{

MIDIDevice::MIDIDevice(std::unique_ptr<protocol> prot):
  ossia::net::device{std::move(prot)},
  MIDINode{*this, *this}
{
}

std::string MIDIDevice::getName() const
{ return mName; }

node&MIDIDevice::setName(std::string n)
{
  mName = n;
  return *this;
}

bool MIDIDevice::updateNamespace()
{
  clearChildren();

  try {
    for(int i = 1; i <= 16; i++)
    {
      auto ptr = std::make_unique<MIDIChannel>(i, *this);
      ptr->init();
      mChildren.push_back(std::move(ptr));
    }
  }
  catch(...)
  {
    std::cerr << "refresh failed\n";
    return false;
  }
  return true;
}

}
}
