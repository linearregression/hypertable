

# Introduction #

_"Programs must be written for people to read, and only incidentally for machines to execute."_ - Abelson and Sussman (<a href='http://www.amazon.com/gp/product/0262011530?ie=UTF8&tag=ebagle-20&linkCode=as2&camp=1789&creative=9325&creativeASIN=0262011530'>Structure and Interpretation of Computer Programs - 2nd Edition</a><img src='http://www.assoc-amazon.com/e/ir?t=ebagle-20&l=as2&o=1&a=0262011530' alt='' border='0' width='1' height='1' />)

Most successful software projects have consistent coding convention/guidelines. We're not an exception. The following are rules of thumb that meant to give the project a consistent look and feel and to improve code readability and ultimately maintainability.


# Namespace #

  1. All project symbols should reside in Hypertable or Hyperspace namespace.
  1. Project wide macros (in a header file) should be prefixed with `HT_`.
  1. Avoid "using namespace blah" in the global namespace of a header file. e.g., instead of
```
using namespace boost::spirit;

namespace Hypertable {
  namespace HqlParser {
    ...
  }
}
```
> use a more restrictive style to avoid namespace pollution:
```
namespace Hypertable {
  namespace HqlParser {
    using namespace boost::spirit; // or use explicit qualifiers
    ...
  }
}
```

# Style #
  1. Filename: `CamelCase.h` and `CamelCase.cc` for C++ source; `lower-case.h` and `lower-case.c` for C source.
  1. Namespace, class and type identifiers: use `CamelCase`, e.g.: `Hypertable::TableIdentifier` (Exceptions: use `lower_case` for boost::spirit parser action functors as recommended by the spirit documentation.) Use `CamelCaseT` for template arguments. e.g.: `template <typename TypeFamilyT>`.
  1. Function/method, variable identifiers: use `lower_case`. private member variable is prefixed with `m_` (`ms_` for static members), e.g.: `m_table`. Variable naming should focus on semantics and try to avoid type annotations (like somethingp, something\_ptr etc.) except for special cases like pointer arithmetic code (e.g., `void *buf` and `uint8_t **bufp` in serialization code). Let language syntax and compiler take care of types.
  1. Constants, enums, macros: use `UPPER_CASE`
  1. Indentation: 2 spaces per level. 4 spaces (or more for alignment) for line continuation. [No tabs allowed](http://www.jwz.org/doc/tabs-vs-spaces.html).
    * Emacs: put the following in your .emacs
```
(setq-default indent-tabs-mode nil)
(setq c-basic-offset 2)
```
    * Vim: put the following in your .vimrc
```
set expandtab shiftwidth=2
```
    * KDevelop: goto "Settings"/"Configure Editor...", check "Use spaces" radio button.
    * Eclipse: goto "Preference"/"Java"/"Code Style"/"Formatter", click "Edit..." button, which pops up the "Profile" panel, where you select "Spaces only" Tab policy and set indentation size to 2.
  1. Avoid trailing whitespaces.
  1. Line length: try to stay within 80 characters for [better readability](http://psychology.wichita.edu/surl/usabilitynews/72/columns.asp) ("the optimal number of characters per line is between 45 and 65".) and compatibility with existing tools.
  1. Comment: we use [javadoc](http://java.sun.com/javadoc/writingdoccomments/index.html) style [comment](http://java.sun.com/docs/codeconv/html/CodeConventions.doc4.html)(adapt accordingly), which is supported by doxygen to generate code documentation;
  1. Curly brace placement: examples:
```
Type function_or_method(Argument arg) {
  //...
  try {
    //...
    do {
      if (condition) {
        // ...
      }
      else {
        // ...
      }
      else 
        break;
    } while (condition);
    //...
  }
  catch (Exception &e) {
    //...
  }
  //...
}
```
```
FairlyLongTypeName
function_or_method_name(Argument arg) {
}
```
```
ConstructorWithInitializer(Foo foo, Bar bar)
  : m_foo(foo), m_bar(bar) {
  // ...
}
```
> Note: it's basically [K&R](http://en.wikipedia.org/wiki/Indent_style#K.26R_style), except for the placement of `else`, and the open `{` for functions, which is more consistent :)

# Gotchas avoidance #
  * Avoid none-trivial (other than int/floats/pointers) globals to reduce global initialization/destruction order issues.
  * Make single argument constructors explict (e.g., explict SimpleCtor(int foo);) to avoid unintended automatic conversion.
  * Make destructors virtual for classes with at least one virtual member functions.

# Logging #
  * Use the `HT_<event_type>` etc. logging macros in Common/Logger.h
  * Tip: Both printf style and stream style are supported. Examples:
```
HT_ERRORF("bad range: [%s, %s]", start_row, end_row);
```
```
HT_ERROR_OUT <<"bad range: ["<< start_row <<", "<< end_row <<"]" << HT_END;
```

# Error Handling #

  * Use exceptions where appropriate (see: http://www.parashift.com/c++-faq-lite/exceptions.html)
  * Use Hypertable::Exception only. Try to use HT\_THROW macros (in Common/Error.h) instead of throw Exception ... to help maintain an exception back-trace, which can be displayed with output\_stream << e; Note: this mechanism is more flexible, reliable and portable than using back\_trace function in glibc or libunwind as functions can be aggressively inlined in C++. For example, the following is an output of HT\_INFO\_OUT << e << HT\_END;
```
1213267024 INFO exception_test : (Source/hypertable/src/cc/Common/tests/exception_test.cc:42) Hypertable::Exception: testing ex1 - HYPERTABLE protocol error
        at void ExceptionTest::test_ex0(uint8_t) (Source/hypertable/src/cc/Common/tests/exception_test.cc:29)
        at void ExceptionTest::test_ex1(uint16_t) (Source/hypertable/src/cc/Common/tests/exception_test.cc:23): testing ex2 - External error
        at void ExceptionTest::test_ex2(uint32_t) (Source/hypertable/src/cc/Common/tests/exception_test.cc:19): testing ex3
        at void ExceptionTest::test_ex3(uint64_t) (Source/hypertable/src/cc/Common/tests/exception_test.cc:15): testing ex4
        at void ExceptionTest::test_ex4(double) (Source/hypertable/src/cc/Common/tests/exception_test.cc:11): throw an external error
```
  * Tip: use HT\_TRY to conveniently create an exception trace record, i.e. you can use:
```
HT_TRY("doing something",
  do_a();
  do_b());
```
> instead of a more verbose:
```
  try {
    do_a();
    do_b();
  }
  catch (Exception &e) {
    HT_THROW2(e.code(), "doing something", e);
  }
```

# Compatibility #

We tried to be as portable as possible. But we currently only regularly test on Mac OS X (10.4.x and 10.5.x) and CentOS 5.x (we occasionally test on Ubuntu and Fedora to debug user installation problems)
  * To facilitate portability efforts, always include "Common/Compat.h" before any other include files in the source (.cc files; use Common/compat-c.h for .c files) if the code uses any Hypertable libraries.
  * Tip: the Common/Compat.h not only comes with all the types (uint64\_t etc.) and limits, it has a sweet syntatic sugar added, you now do this (assuming you have a `vector<Schema::ColumnFamily *> columns;`):
```
foreach(Schema::ColumnFamily *cf, columns)
  do_something_to(cf);
```
> instead of:
```
for (std::vector<Schema::ColumnFamily *>::iterator it = columns.begin(); it != columns.end(); ++it)
  do_something_to(*it);
```