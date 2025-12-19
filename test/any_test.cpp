#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility/any.hpp>
#include <vector>

#define TEST_CASE(name) \
    std::cout << "--- Running Test: " << name << " ---" << std::endl;
#define TEST_PASSED() \
    std::cout << "--- Test PASSED ---" << std::endl << std::endl;
#define TEST_FAILED(msg) \
    { \
        std::cerr << "!!! Test FAILED: " << msg << std::endl; \
        exit(1); \
    }

struct MyStruct {
    int id;
    std::string name;
    bool moved_from = false;

    MyStruct(int i = 0, std::string n = "default") : id(i), name(n) {
        // std::cout << "MyStruct(" << id << ", " << name << ") constructed." <<
        // std::endl;
    }

    MyStruct(const MyStruct &other) : id(other.id), name(other.name) {
        // std::cout << "MyStruct(" << id << ", " << name << ") copy
        // constructed." << std::endl;
    }

    MyStruct(MyStruct &&other) noexcept
        : id(other.id),
          name(std::move(other.name)) {
        other.id = -1;
        other.name = "MOVED_FROM";
        other.moved_from = true;
        // std::cout << "MyStruct(" << id << ", " << name << ") move
        // constructed." << std::endl;
    }

    MyStruct &operator=(const MyStruct &other) {
        if (this != &other) {
            id = other.id;
            name = other.name;
        }
        // std::cout << "MyStruct(" << id << ", " << name << ") copy assigned."
        // << std::endl;
        return *this;
    }

    MyStruct &operator=(MyStruct &&other) noexcept {
        if (this != &other) {
            id = other.id;
            name = std::move(other.name);
            other.id = -1;
            other.name = "MOVED_FROM";
            other.moved_from = true;
        }
        // std::cout << "MyStruct(" << id << ", " << name << ") move assigned."
        // << std::endl;
        return *this;
    }

    bool operator==(const MyStruct &other) const {
        return id == other.id && name == other.name;
    }

    ~MyStruct() {
        // std::cout << "MyStruct(" << id << ", " << name << ") destructed." <<
        // std::endl;
    }
};

void test_default_construction() {
    TEST_CASE("Default Construction")
    Marcus::any a;
    assert(!a.has_value());
    assert(a.type() == typeid(void));
    TEST_PASSED()
}

void test_value_construction() {
    TEST_CASE("Value Construction")
    Marcus::any a = 123;
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 123);

    Marcus::any b = std::string("hello");
    assert(b.has_value());
    assert(b.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(b) == "hello");

    MyStruct ms(1, "test_struct");
    Marcus::any c = ms;
    assert(c.has_value());
    assert(c.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(c) == ms);

    Marcus::any d = MyStruct(2, "temp_struct");
    assert(d.has_value());
    assert(d.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(d) == MyStruct(2, "temp_struct"));
    TEST_PASSED()
}

void test_copy_construction() {
    TEST_CASE("Copy Construction")
    Marcus::any original = 456;
    Marcus::any copied = original;
    assert(copied.has_value());
    assert(copied.type() == typeid(int));
    assert(Marcus::any_cast<int>(copied) == 456);
    assert(Marcus::any_cast<int>(original) == 456);

    Marcus::any original_str = std::string("copy_me");
    Marcus::any copied_str = original_str;
    assert(copied_str.has_value());
    assert(copied_str.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(copied_str) == "copy_me");
    assert(Marcus::any_cast<std::string>(original_str) == "copy_me");
    TEST_PASSED()
}

void test_move_construction() {
    TEST_CASE("Move Construction")
    Marcus::any original = 789;
    Marcus::any moved = std::move(original);
    assert(moved.has_value());
    assert(moved.type() == typeid(int));
    assert(Marcus::any_cast<int>(moved) == 789);
    assert(!original.has_value());

    Marcus::any original_str = std::string("move_me");
    Marcus::any moved_str = std::move(original_str);
    assert(moved_str.has_value());
    assert(moved_str.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(moved_str) == "move_me");
    assert(!original_str.has_value());

    MyStruct ms_val(3, "move_struct_original");
    Marcus::any original_ms = ms_val;
    Marcus::any moved_ms = std::move(original_ms);
    assert(moved_ms.has_value());
    assert(moved_ms.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(moved_ms) == ms_val);
    assert(!original_ms.has_value());
    TEST_PASSED()
}

void test_in_place_construction() {
    TEST_CASE("In-Place Construction")
    Marcus::any a(Marcus::in_place_type<int>, 100);
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 100);

    Marcus::any b(Marcus::in_place_type<std::string>, 5, 'x'); // "xxxxx"
    assert(b.has_value());
    assert(b.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(b) == "xxxxx");

    Marcus::any c(Marcus::in_place_type<std::vector<int>>, {1, 2, 3});
    assert(c.has_value());
    assert(c.type() == typeid(std::vector<int>));
    std::vector<int> expected_vec = {1, 2, 3};
    assert(Marcus::any_cast<std::vector<int>>(c) == expected_vec);

    Marcus::any d(Marcus::in_place_type<MyStruct>, 4, "in_place_struct");
    assert(d.has_value());
    assert(d.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(d) == MyStruct(4, "in_place_struct"));
    TEST_PASSED()
}

void test_copy_assignment() {
    TEST_CASE("Copy Assignment")
    Marcus::any a = 10;
    Marcus::any b;
    b = a;
    assert(b.has_value());
    assert(b.type() == typeid(int));
    assert(Marcus::any_cast<int>(b) == 10);
    assert(Marcus::any_cast<int>(a) == 10);

    Marcus::any c = std::string("original");
    Marcus::any d = 20.0;
    d = c;
    assert(d.has_value());
    assert(d.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(d) == "original");
    assert(Marcus::any_cast<std::string>(c) == "original");

    Marcus::any e = 50;
    e = e;
    assert(e.has_value());
    assert(e.type() == typeid(int));
    assert(Marcus::any_cast<int>(e) == 50);
    TEST_PASSED()
}

void test_move_assignment() {
    TEST_CASE("Move Assignment")
    Marcus::any a = 10;
    Marcus::any b;
    b = std::move(a);
    assert(b.has_value());
    assert(b.type() == typeid(int));
    assert(Marcus::any_cast<int>(b) == 10);
    assert(!a.has_value());

    Marcus::any c = std::string("original_move");
    Marcus::any d = 20.0;
    d = std::move(c);
    assert(d.has_value());
    assert(d.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(d) == "original_move");
    assert(!c.has_value());

    Marcus::any e = 50;
    e = std::move(e);
    assert(e.has_value());
    assert(e.type() == typeid(int));
    assert(Marcus::any_cast<int>(e) == 50);
    TEST_PASSED()
}

void test_value_assignment() {
    TEST_CASE("Value Assignment")
    Marcus::any a;
    a = 100;
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 100);

    a = std::string("new_value");
    assert(a.has_value());
    assert(a.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(a) == "new_value");

    MyStruct ms(5, "assigned_struct");
    a = ms;
    assert(a.has_value());
    assert(a.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(a) == ms);

    a = MyStruct(6, "temp_struct");
    assert(a.has_value());
    assert(a.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(a) == MyStruct(6, "temp_struct"));
    TEST_PASSED()
}

void test_emplace() {
    TEST_CASE("Emplace")
    Marcus::any a;
    a.emplace<int>(10);
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 10);

    a.emplace<std::string>(3, 'a'); // "aaa"
    assert(a.has_value());
    assert(a.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(a) == "aaa");

    a.emplace<std::vector<double>>({1.1, 2.2, 3.3});
    assert(a.has_value());
    assert(a.type() == typeid(std::vector<double>));
    std::vector<double> expected_vec = {1.1, 2.2, 3.3};
    assert(Marcus::any_cast<std::vector<double>>(a) == expected_vec);

    Marcus::any b = 123;
    b.emplace<MyStruct>(7, "emplaced_struct");
    assert(b.has_value());
    assert(b.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(b) == MyStruct(7, "emplaced_struct"));
    TEST_PASSED()
}

void test_reset() {
    TEST_CASE("Reset")
    Marcus::any a = 100;
    assert(a.has_value());
    a.reset();
    assert(!a.has_value());
    assert(a.type() == typeid(void));

    Marcus::any b;
    b.reset();
    assert(!b.has_value());
    assert(b.type() == typeid(void));
    TEST_PASSED()
}

void test_swap() {
    TEST_CASE("Swap")
    Marcus::any a = 10;
    Marcus::any b = std::string("hello");

    a.swap(b);

    assert(a.has_value());
    assert(a.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(a) == "hello");

    assert(b.has_value());
    assert(b.type() == typeid(int));
    assert(Marcus::any_cast<int>(b) == 10);

    Marcus::any c;
    Marcus::any d = 3.14;

    c.swap(d);

    assert(c.has_value());
    assert(c.type() == typeid(double));
    assert(Marcus::any_cast<double>(c) == 3.14);

    assert(!d.has_value());
    assert(d.type() == typeid(void));
    TEST_PASSED()
}

void test_has_value_and_type() {
    TEST_CASE("has_value() and type()")
    Marcus::any a;
    assert(!a.has_value());
    assert(a.type() == typeid(void));

    Marcus::any b = 10;
    assert(b.has_value());
    assert(b.type() == typeid(int));

    Marcus::any c = std::string("test");
    assert(c.has_value());
    assert(c.type() == typeid(std::string));

    c.reset();
    assert(!c.has_value());
    assert(c.type() == typeid(void));
    TEST_PASSED()
}

void test_any_cast_ref() {
    TEST_CASE("any_cast (by reference)")
    Marcus::any a = 10;
    assert(Marcus::any_cast<int>(a) == 10);
    assert(Marcus::any_cast<const int>(a) == 10);

    const Marcus::any ca = std::string("const_str");
    assert(Marcus::any_cast<std::string>(ca) == "const_str");
    assert(Marcus::any_cast<const std::string>(ca) == "const_str");

    Marcus::any b = MyStruct(8, "cast_ref_struct");
    MyStruct &ms_ref = Marcus::any_cast<MyStruct &>(b);
    ms_ref.id = 9;
    assert(Marcus::any_cast<MyStruct>(b).id == 9);

    Marcus::any c = MyStruct(10, "rvalue_cast_struct");
    MyStruct moved_ms = Marcus::any_cast<MyStruct>(std::move(c));
    assert(moved_ms == MyStruct(10, "rvalue_cast_struct"));

    assert(c.has_value());
    assert(Marcus::any_cast<MyStruct &>(c).moved_from);

    try {
        Marcus::any_cast<double>(a);
        TEST_FAILED("Expected BadAnyCast for wrong type.")
    } catch (const Marcus::BadAnyCast &e) {
        std::cout << "  Caught expected exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        TEST_FAILED(std::string("Caught unexpected exception: ") + e.what())
    }

    Marcus::any empty_any;
    try {
        Marcus::any_cast<int>(empty_any);
        TEST_FAILED("Expected BadAnyCast for empty any.")
    } catch (const Marcus::BadAnyCast &e) {
        std::cout << "  Caught expected exception: " << e.what() << std::endl;
    } catch (const std::exception &e) {
        TEST_FAILED(std::string("Caught unexpected exception: ") + e.what())
    }
    TEST_PASSED()
}

void test_any_cast_ptr() {
    TEST_CASE("any_cast (by pointer)")
    Marcus::any a = 10;
    int *p_int = Marcus::any_cast<int>(&a);
    assert(p_int != nullptr);
    assert(*p_int == 10);
    *p_int = 20;
    assert(Marcus::any_cast<int>(a) == 20);

    const Marcus::any ca = std::string("const_ptr_str");
    const std::string *p_const_str = Marcus::any_cast<std::string>(&ca);
    assert(p_const_str != nullptr);
    assert(*p_const_str == "const_ptr_str");

    double *p_double = Marcus::any_cast<double>(&a);
    assert(p_double == nullptr);

    Marcus::any empty_any;
    int *p_empty = Marcus::any_cast<int>(&empty_any);
    assert(p_empty == nullptr);

    const int *p_const_empty = Marcus::any_cast<int>(&empty_any);
    assert(p_const_empty == nullptr);

    int *p_null_any =
        Marcus::any_cast<int>(static_cast<Marcus::any *>(nullptr));
    assert(p_null_any == nullptr);
    const int *p_const_null_any =
        Marcus::any_cast<int>(static_cast<const Marcus::any *>(nullptr));
    assert(p_const_null_any == nullptr);
    TEST_PASSED()
}

void test_make_any() {
    TEST_CASE("make_any")
    Marcus::any a = Marcus::make_any<int>(100);
    assert(a.has_value());
    assert(a.type() == typeid(int));
    assert(Marcus::any_cast<int>(a) == 100);

    Marcus::any b = Marcus::make_any<std::string>(5, 'y'); // "yyyyy"
    assert(b.has_value());
    assert(b.type() == typeid(std::string));
    assert(Marcus::any_cast<std::string>(b) == "yyyyy");

    Marcus::any c = Marcus::make_any<std::vector<int>>({10, 20, 30});
    assert(c.has_value());
    assert(c.type() == typeid(std::vector<int>));
    std::vector<int> expected_vec = {10, 20, 30};
    assert(Marcus::any_cast<std::vector<int>>(c) == expected_vec);

    Marcus::any d = Marcus::make_any<MyStruct>(11, "made_struct");
    assert(d.has_value());
    assert(d.type() == typeid(MyStruct));
    assert(Marcus::any_cast<MyStruct>(d) == MyStruct(11, "made_struct"));
    TEST_PASSED()
}

int main() {
    std::cout << "Starting Marcus::any tests..." << std::endl << std::endl;

    test_default_construction();
    test_value_construction();
    test_copy_construction();
    test_move_construction();
    test_in_place_construction();
    test_copy_assignment();
    test_move_assignment();
    test_value_assignment();
    test_emplace();
    test_reset();
    test_swap();
    test_has_value_and_type();
    test_any_cast_ref();
    test_any_cast_ptr();
    test_make_any();

    std::cout << "All Marcus::any tests passed successfully!" << std::endl;

    return 0;
}
