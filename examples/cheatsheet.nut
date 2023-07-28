/*
  a multiline comment
 */

// a single line comment

# a bash-style comment (not recommended)

// semi-colons are only needed for two statements on one line, otherwise they are optional
print("one"); print("line")
error("one"); error("line") // print to stderr

// a better print
function println(...) {
    foreach(v in vargv) {
        print(v)
    }
    print("\n")
}

// bool
true; false

// integer
5 // regular integer
0xfa // hex
010  // octal
'a' // characters are integers too: 97

// float
5.2

// string
"Hello World"

// numbers added to a string will be converted to a string and appended
"Hello World" + 5

foreach(c in "HelloWorld") {
    // c is an integer, tochar() converts back to string
    println(c.tochar())
}

// a table
tbl <- { x = 10, y = 10 }
println(tbl.values())
println(tbl.keys())

// new slot operating adding z to the table
tbl.z <- 10
// deleting an entry from the table
delete tbl.z
// alternative way to accessing the table
tbl["y"]
delete tbl["y"]

// an array
arr1 <- [ 1, 2, 3, 4, 5 ]
arr2 <- [ 1 2 3 4 5 ]  // commas are optional

// an array of size 10 filled with null
arr3 <- array(10)
arr3.append(10) // append one element to the array
arr3.extend(arr1) // append another array to an array
el3 <- arr3.remove(3) // remove the third element and return it

// enums
enum MyEnum {
    a // 0
    b // 1
    c // 2
    d = 10 // explicit value
}
println("MyEnum: ", MyEnum.a, " ", MyEnum.b, " ", MyEnum.c, " ", MyEnum.d)

// a local variable only visible in the lexical scope
local a = 5;
local z // this is initialized to null

// a new slot operation that adds an entry to the roottable, aka a global variable
b <- 5;

// a plain C-style loop
for (local i = 0; i < 10; ++i) {
    print(i + "\n")
}

// foreach loop
print("foreach loop:\n")
foreach(k, v in { x = 5, y = 10 }) {
    print(k + " -> " + v + "\n")
}

// foreach loop with values only
foreach(v in { x = 5, y = 10 }) {
    print(v + "\n")
}

// foreach loop over an array
foreach(i, v in ["a" "b" "c"]) {
    print(i + ". " + v + "\n")
}

// a function
function foo() {
    return 10
}

// a lamda function
foo2 <- @() 10  // short style
foo3 <- function(){ return 10 }  // long style

// namespace via a table
foospace <- {}
function foospace::foo() {
    return 45
}

// function with default values
function foobar(x = 45, y = 99, lst = [] /* bad */) {
    // must reassign array here, or it will be shared
    lst = []
}

// function with variable number of arguments
function foobar(x, y, ...) {
    print(x + "\n")
    print(y + "\n")
    foreach(arg in vargv) {
        print(arg)
    }
}

// generator function
function makegenerator() {
    local i = 0;
    while (true) {
        yield i++
    }
}
mygen <- makegenerator()
print("Generator:\n")
print(resume mygen + "\n")
print(resume mygen + "\n")
print(resume mygen + "\n")
print(resume mygen + "\n")

// class
class Base {
    a = 11
    b = 22

    function constructor() {
        print("Base() constructor\n")
    }
}

// single inheritance
class Foo extends Base {
    // member variables with default values
    x = 5
    y = 10
    lst = null

    // these are shared between all class instances
    static z = 10;

    // constructor for additional initialisation
    function constructor() {
        print("Foo() constructor\n")
        base.constructor()

        this.x = 10

        // must create array here, or it'll be shared, same as Python
        lst = []
    }

    // a member function
    function do_it() {
        // both work
        this.x = 10
        x = 10
    }
}

print("Make a Foo()\n")
Foo()

// inspecting class members works similar to tables
foreach(k, v in Foo) {
    println("class: ", k, " ", v)
}

// threads
thread1 <- newthread(function(){ for(local i = 0; i < 5; ++i) {print("+++Thread1 " + i + "\n"); suspend() }})
thread2 <- newthread(function(){ for(local i = 0; i < 5; ++i) {print("---Thread2 " + i + "\n"); suspend() }})
thread1.call()
thread2.call()
while(thread1.getstatus() == "suspended" || thread2.getstatus() == "suspended") {
    if (thread1.getstatus() == "suspended") {
        thread1.wakeup()
    }
    if (thread2.getstatus() == "suspended") {
        thread2.wakeup()
    }
}

// Surprises
local boing = 155
class Bar {
    boing = 999
    function constructor() {
        // the outside local as precedence over the member
        // variable, so this.boing is still 5
        boing = 1111
    }
}
println(Bar().boing)

// asserts
assert(true, "Everything is ok")

// special initialisation syntax, same as Bar().boing = 10
Bar() { boing = 10 }

// functions can be bound to another table, instead of the root table
env <- {}
function bar[env]() {
    x <- 10
    y <- 20
}
bar()
println(env)

function foo() {
    x <- 1010
    y <- 2020
}
// this can also be done for just a single call
foo.pcall(env)
println(env)

/* EOF */
