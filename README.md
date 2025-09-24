# Treewalk-interpreter-2
An interpreter for the scripting language Lox, built in C++. Inspired by Crafting Interpreters by Robert Nystrom. A toy treewalk interpreter for learning purposes.

The interpreter is available to download in releases. You can simply download it, run it, and start coding using the multi-line REPL, or you can run the interpreter on a file.

# Code Examples
Some example bits of code you can try out are:

# Variables
```
var a = 3;
var b = 7;

print "a + b = " + (a + b);
print "a * b = " + (a * b);
```
# Conditionals
```
if (a < b) {
    print "a is less than b";
} else {
    print "a is not less than b";
}
```
# While loop
```
var count = 0;
while (count < 5) {
    print "Loop iteration: " + count;
    count = count + 1;
}
```
# Block scope
```
var outside = "I exist outside";
{
    var inside = "I exist inside";
    print inside;
    print outside;  # accessible inside the block
}
print outside;      # still accessible
print inside;    # would error: undefined variable
```
