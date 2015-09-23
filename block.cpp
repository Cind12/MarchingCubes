#include <hpx/hpx.hpp>
#include <hpx/runtime/components/component_factory.hpp>

#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>

#include "block.hpp"

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<server::block> t_object;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(t_object, block);

HPX_REGISTER_ACTION(t_object::wrapped_type::initialize_action, block_initialize_action);
HPX_REGISTER_ACTION(t_object::wrapped_type::getBoilerplate_action, block_getBoilerplate_action);
HPX_REGISTER_ACTION(t_object::wrapped_type::findInfo_action, block_findInfo_action);
HPX_REGISTER_ACTION(t_object::wrapped_type::computeBlock_action, block_computeBlock_action);
HPX_REGISTER_ACTION(t_object::wrapped_type::communicateGlobalIDs_action, block_communicateGlobalIDs_action);
HPX_REGISTER_ACTION(t_object::wrapped_type::sendPassive_action, block_sendPassive_action);
HPX_REGISTER_ACTION(t_object::wrapped_type::sendActive_action, block_sendActive_action);
