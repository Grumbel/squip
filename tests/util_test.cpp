#include <gtest/gtest.h>

#include <sstream>
#include <vector>

#include <fmt/format.h>
#include <squip/squirrel_util.hpp>
#include <squip/squirrel_vm.hpp>

TEST(SquipUtil, print_stack)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  sq_pushroottable(vm);
  std::ostringstream os;
  squip::print_stack(vm, os);

  // This is rather brittle and will need updating with new Squirrel versions
  EXPECT_EQ(os.str(), "#1  {\"setconsttable\": <native closure:setconsttable>, \"getroottable\": <native closure:getroottable>, \"newthread\": <native closure:newthread>, \"print\": <native closure:print>, \"getstackinfos\": <native closure:getstackinfos>, \"_versionnumber_\": 320, \"_charsize_\": 1, \"enabledebuginfo\": <native closure:enabledebuginfo>, \"_floatsize_\": 4, \"callee\": <native closure:callee>, \"type\": <native closure:type>, \"_intsize_\": 8, \"suspend\": <native closure:suspend>, \"resurrectunreachable\": <native closure:resurrectunreachable>, \"assert\": <native closure:assert>, \"getconsttable\": <native closure:getconsttable>, \"error\": <native closure:error>, \"setroottable\": <native closure:setroottable>, \"dummy\": <native closure:dummy>, \"_version_\": \"Squirrel 3.2 stable\", \"setdebughook\": <native closure:setdebughook>, \"collectgarbage\": <native closure:collectgarbage>, \"array\": <native closure:array>, \"seterrorhandler\": <native closure:seterrorhandler>, \"compilestring\": <native closure:compilestring>}\n");
  sq_poptop(vm);
}

TEST(SquipUtil, print)
{
  squip::SquirrelVM sqvm;
  HSQUIRRELVM vm = sqvm.get_vm();

  std::istringstream is (
    "function myfunc() {};"
    "myanonfunc <- function() {};"
    "class MyClass {};"
    "MyClassClose <- MyClass;"
    "myinstance <- MyClass();"
    "myotherinstance <- MyClass();"
    "mycloneinstance <- myinstance;"
    "mygenerator <- (function() { yield; })();"
    "myarray <- [11, 22, 33];"
    "mytable <- { a = 11, b = 22, c = 33 };"
    "mythread <- newthread(function(){});"
    );
  squip::compile_and_run(vm, is, "<source>");

  std::ostringstream os;

  os = {};
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "<unknown>");

  os = {};
  sq_pushnull(vm);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "null");
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushbool(vm, true);
  sq_pushbool(vm, false);
  squip::print(vm, -2, os);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "truefalse");
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushinteger(vm, 12345);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "12345");
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushfloat(vm, 0.125);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "0.125");
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushstring(vm, "Hello\nWorld", -1);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "Hello\nWorld");
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushstring(vm, "Hello\nWorld", -1);
  squip::repr(vm, -1, os);
  EXPECT_EQ(os.str(), "\"Hello\\nWorld\"");
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushuserpointer(vm, reinterpret_cast<SQUserPointer*>(0xdeadbeef));
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "<userpointer:0xdeadbeef>");
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "mytable", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  // Squirrel tables do not preserve order
  EXPECT_EQ(os.str(), "{\"a\": 11, \"c\": 33, \"b\": 22}");
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "myarray", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "[11, 22, 33]");
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  SQUserPointer userdata = sq_newuserdata(vm, 64);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), fmt::format("<userdata:{:p}:{}>", userdata, nullptr));
  sq_poptop(vm);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "myfunc", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "<closure:myfunc>");
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "myanonfunc", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "<closure:<anonymous>>");
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "print", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), fmt::format("<native closure:print>"));
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "MyClass", -1);
  sq_get(vm, -2);
  sq_pushstring(vm, "MyClassClone", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  std::string mycloneclass = os.str();
  os = {};
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), mycloneclass);
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  {
    sq_pushroottable(vm);

    os = {};
    sq_pushstring(vm, "mycloneinstance", -1);
    sq_get(vm, -2);
    squip::print(vm, -1, os);
    std::string mycloneinstance = os.str();
    sq_pop(vm, 1);

    os = {};
    sq_pushstring(vm, "myotherinstance", -1);
    sq_get(vm, -2);
    squip::print(vm, -1, os);
    std::string myotherinstance = os.str();
    sq_pop(vm, 1);

    os = {};
    sq_pushstring(vm, "myinstance", -1);
    sq_get(vm, -2);
    squip::print(vm, -1, os);
    std::string myinstance = os.str();
    sq_pop(vm, 1);

    EXPECT_EQ(myinstance, mycloneinstance);
    EXPECT_NE(myinstance, myotherinstance);
    EXPECT_NE(mycloneinstance, myotherinstance);
    sq_pop(vm, 1);
    ASSERT_EQ(sq_gettop(vm), 0);
  }

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "mygenerator", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  EXPECT_TRUE(os.str().starts_with("<generator:"));
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "myarray", -1);
  sq_get(vm, -2);
  sq_weakref(vm, -1);
  squip::print(vm, -1, os);
  EXPECT_EQ(os.str(), "<weakref:[11, 22, 33]>");
  sq_pop(vm, 3);
  ASSERT_EQ(sq_gettop(vm), 0);

  os = {};
  sq_pushroottable(vm);
  sq_pushstring(vm, "mythread", -1);
  sq_get(vm, -2);
  squip::print(vm, -1, os);
  EXPECT_TRUE(os.str().starts_with("<thread:"));
  sq_pop(vm, 2);
  ASSERT_EQ(sq_gettop(vm), 0);
}

/* EOF */
