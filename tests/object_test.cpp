#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include <squip/squirrel_vm.hpp>
#include <squip/object.hpp>
#include <squip/util.hpp>

TEST(SquipObject, assign)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_newarray(vm, 0);
  sq_pushinteger(vm, 11); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 22); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 33); sq_arrayappend(vm, -2);
  sq_pushinteger(vm, 44); sq_arrayappend(vm, -2);

  squip::Object orig_myarray(vm, -1);
  ASSERT_EQ(orig_myarray.get_type(), OT_ARRAY);
  ASSERT_EQ(orig_myarray.get_refcount(), 1);
  squip::Object myarray_copy = orig_myarray;
  ASSERT_EQ(myarray_copy.get_type(), OT_ARRAY);
  ASSERT_EQ(orig_myarray.get_refcount(), 2);
  squip::Object myarray_move = std::move(myarray_copy);
  ASSERT_EQ(myarray_move.get_type(), OT_ARRAY);
  ASSERT_EQ(orig_myarray.get_refcount(), 2);
  squip::Object myarray = myarray_move;
  ASSERT_EQ(myarray.get_type(), OT_ARRAY);
  ASSERT_EQ(orig_myarray.get_refcount(), 3);

  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  myarray.push(vm);

  ASSERT_EQ(sq_gettype(vm, -1), OT_ARRAY);
  ASSERT_EQ(sq_getsize(vm, -1), 4);

  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);
}

/* EOF */


