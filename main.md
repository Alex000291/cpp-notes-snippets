**C++ 新语法、标准库、平台API、代码片段**

阅读本文应将不懂的知识点贴给AI。

目录

### C++11

#### 语法
- [auto 类型推导](#auto)
- [范围 for 循环](#range-for-loop)
- [Lambda 表达式](#lambda)
- [nullptr](#nullptr)
- [enum class（作用域枚举）](#enum-class-scoped-enum)
- [override / final](#override-and-final)
- [统一初始化（花括号初始化）](#brace-initialization)
- [委托构造函数](#delegating-constructors)
- [constexpr](#constexpr)
- [可变参数模板](#variadic-templates)
- [右值引用与移动语义](#rvalue-reference-and-move)
- [static_assert](#static_assert)
- [类型别名（using =）](#type-alias-using)
- [原始字符串字面量](#raw-string-literals)
- [用户自定义字面量](#user-defined-literals)
- [noexcept](#noexcept)

#### 标准库
- [std::unique_ptr / std::shared_ptr / std::weak_ptr](#unique_ptr-shared_ptr-weak_ptr)
- [std::array](#stdarray)
- [std::unordered_map / std::unordered_set](#unordered_map-unordered_set)
- [std::tuple](#stdtuple)
- [std::thread / std::mutex / std::condition_variable](#thread-mutex-condition_variable)
- [std::function / std::bind](#function-bind)
- [std::chrono](#stdchrono)
- [std::regex](#stdregex)
- [std::move / std::forward](#move-forward)

### C++14

#### 语法
- [泛型 Lambda（auto 参数）](#generic-lambda-auto)
- [Lambda 捕获初始化器](#lambda-capture-init)
- [返回类型推导](#return-type-deduction)
- [变量模板](#variable-templates)
- [二进制字面量（0b）](#binary-literal)
- [数字分隔符（1'000'000）](#digit-separator)
- [constexpr 放宽规则](#constexpr-relaxed)

#### 标准库
- [std::make_unique](#make-unique)
- [std::integer_sequence](#integer_sequence)

### C++17

#### 语法
- [结构化绑定](#structured-bindings)
- [if constexpr](#if-constexpr)
- [折叠表达式](#fold-expressions)
- [类模板参数推导（CTAD）](#ctad)
- [嵌套命名空间（A::B::C）](#nested-namespace)
- [inline 变量](#inline-variable)
- [属性：nodiscard、maybe_unused、fallthrough](#attributes-nodiscard-maybe_unused-fallthrough)

#### 标准库
- [std::optional](#optional)
- [std::variant](#variant)
- [std::string_view](#string_view)
- [std::filesystem](#filesystem)
- [std::any](#any)
- [std::apply](#apply)
- [std::invoke](#invoke)
- [并行算法（std::execution）](#parallel-algorithms)

### C++20

#### 语法
- [Concepts 与 requires](#concepts-requires)
- [Modules](#modules)
- [协程（co_await / co_yield）](#coroutines)
- [三路比较（<=>）](#three-way-comparison)
- [consteval / constinit](#consteval-constinit)
- [指定初始化器](#designated-initializers)
- [模板 Lambda](#template-lambda)

#### 标准库
- [std::ranges](#ranges)
- [std::span](#span)
- [std::format](#format)
- [std::jthread](#jthread)
- [std::source_location](#source_location)
- [日历与时区（std::chrono 扩展）](#chrono-calendar-timezone)

### C++23

#### 语法
- [if consteval](#if-consteval)
- [Deducing this](#deducing-this)
- [多维下标运算符](#multidimensional-subscript)
- [static operator()](#static-operator)

#### 标准库
- [std::expected](#expected)
- [std::flat_map / std::flat_set](#flat_map-flat_set)
- [std::print / std::println](#print-println)
- [std::generator](#generator)
- [std::mdspan](#mdspan)
- [std::stacktrace](#stacktrace)

---

## auto
```cpp
auto s = "hello"s;    // std::string（需 using namespace
```

## Range for loop
```cpp
for (auto& x : vec)   // 可修改；用 const auto& 只读
    x *= 2;
```

## Lambda
```cpp
auto f = [](int x) { return x * 2; };
sort(v.begin(), v.end(), [](int a, int b) { return a > b; });
```

## nullptr
```cpp
void f(int* p);
f(nullptr);   // 替代 NULL / 0，类型安全
```

## enum class (scoped enum)
```cpp
enum class Color { Red, Green };
Color c = Color::Red;   // 不污染外层，强类型
```

## override and final
```cpp
struct B { virtual void f(); };
struct D : B { void f() override; };   // 漏写则编译报错
struct NoMore : D { void f() final; };
```

## Brace initialization
```cpp
std::vector<int> v = {1, 2, 3};
int a[]{1, 2, 3};
```

## Delegating constructors
```cpp
struct S {
    S(int x) : S(x, 0) {}
    S(int x, int y) : a(x), b(y) {}
    int a, b;
};
```

## constexpr
```cpp
constexpr int sq(int n) { return n * n; }
int x[sq(3)];   // 编译期常量
```

## Variadic templates
```cpp
template<typename... Ts>
void f(Ts... args) { /* 用 args... 展开 */ }
```

## Rvalue reference and move
```cpp
std::string s = std::move(other);   // 移动，不拷贝
T&& r = get_temp();   // 右值引用
```

## static_assert
```cpp
static_assert(sizeof(int) == 4, "need 32-bit int");
```

## Type alias (using)
```cpp
using Vec = std::vector<int>;
using F = void(*)(int);
```

## Raw string literals
```cpp
const char* r = R"(C:\path\no escape)";
```

## User-defined literals
```cpp
// 需定义 operator""_suffix
auto s = "hi"s;   // std::string（<string>）
```

## noexcept
```cpp
void f() noexcept { }
void g() noexcept(noexcept(f())) { }
```

## unique_ptr shared_ptr weak_ptr
```cpp
auto p = std::make_unique<int>(42);
std::shared_ptr<int> sp = std::make_shared<int>(1);
std::weak_ptr<int> wp = sp;   // 打破循环引用
```

## std::array
```cpp
std::array<int, 3> a = {1, 2, 3};   // 固定大小，不退化
```

## unordered_map unordered_set
```cpp
std::unordered_map<std::string, int> m;
std::unordered_set<int> s;
```

## std::tuple
```cpp
auto t = std::make_tuple(1, "x", 3.0);
int a = std::get<0>(t);
```

## thread mutex condition_variable
```cpp
std::mutex m;
std::condition_variable cv;
bool ready = false;

std::thread t([&] {
    std::unique_lock<std::mutex> lk(m);
    cv.wait(lk, [&] { return ready; });   // 等 ready 为 true
    // 干活...
});
// 主线程
{ std::lock_guard<std::mutex> lk(m); ready = true; }
cv.notify_one();
t.join();
```

## function bind
```cpp
std::function<int(int,int)> f = [](int a, int b) { return a + b; };
auto g = std::bind(f, 1, std::placeholders::_1);
```

## std::chrono
```cpp
auto start = std::chrono::steady_clock::now();
// ...
auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
```

## std::regex
```cpp
std::regex re(R"(\d+)");
std::smatch m;
std::regex_search(s, m, re);
```

## move forward
```cpp
// move：转移所有权，源对象可被置空
std::vector<int> a = {1, 2}, b = std::move(a);   // a 不再保证有元素

// forward：完美转发，保留左/右值性
template<typename T>
void wrap(T&& x) { f(std::forward<T>(x)); }   // 传左值则左值，传右值则右值
```

## Generic Lambda (auto)
```cpp
auto f = [](auto x, auto y) { return x + y; };
f(1, 2);   // 3
f(1.0, 2.0);   // 3.0
```

## Lambda capture init
```cpp
int x = 1;
auto f = [y = x + 1] { return y; };   // 捕获时用表达式初始化
```

## Return type deduction
```cpp
auto f(int a, int b) { return a + b; }   // 返回 int
```

## Variable templates
```cpp
template<typename T>
constexpr T pi = T(3.1415926535897932385);
double d = pi<double>;
```

## Binary literal
```cpp
int n = 0b1100;   // 12
```

## Digit separator
```cpp
long long x = 1'000'000;
int hex = 0xFF'FF'FF'FF;
```

## constexpr relaxed
```cpp
constexpr int f() {
    int x = 0;
    if (true) x = 1;   // C++14 允许多条语句、局部变量等
    return x;
}
```

## make_unique
```cpp
auto p = std::make_unique<Widget>(1, "a");   // 替代 new Widget(1, "a")
```

## integer_sequence
```cpp
template<typename T, T... Is>
void g(std::integer_sequence<T, Is...>) { /* 用 Is... 展开 */ }
auto seq = std::index_sequence_for<int, char, double>();   // 0,1,2
```

## Structured bindings
```cpp
auto [a, b] = std::pair(1, "x");
for (auto [k, v] : map) { }
std::tuple t{1, 2.0}; auto [x, y] = t;
```

## if constexpr
```cpp
template<typename T>
void f(T x) {
    if constexpr (std::is_integral_v<T>) { /* 整数分支，编译期剔除另一支 */ }
    else { }
}
```

## Fold expressions
```cpp
template<typename... Ts>
auto sum(Ts... args) { return (args + ...); }   // 一元右折叠
template<typename... Ts>
bool all(Ts... args) { return (... && args); }
```

## CTAD
```cpp
std::pair p(1, 2.0);           // pair<int, double>
std::vector v = {1, 2, 3};     // vector<int>
std::mutex m;
std::lock_guard lk(m);          // lock_guard<mutex>
```

## Nested namespace
```cpp
namespace A::B::C { }
// 等价 namespace A { namespace B { namespace C { } } }
```

## inline variable
```cpp
// 头文件中可定义，多翻译单元只保留一份
inline int global_counter = 0;
```

## attributes nodiscard maybe_unused fallthrough
```cpp
[[nodiscard]] int must_use();
void g(int x) {
    switch (x) {
        case 1: f(); [[fallthrough]];
        case 2: h(); break;
    }
}
[[maybe_unused]] static int debug_flag;
```

## optional
```cpp
std::optional<int> o;
o = 42;
if (o) use(*o);
int v = o.value_or(0);
```

## variant
```cpp
std::variant<int, std::string> v = 1;
v = "hi";
std::visit([](auto&& x) { }, v);
int i = std::get<int>(v);
```

## string_view
```cpp
void f(std::string_view sv) {   // 只读，不拥有
    size_t n = sv.size();
    char c = sv[0];
}
f("literal"); f(std::string(s));
```

## filesystem
```cpp
namespace fs = std::filesystem;
fs::path p = "a/b/c.txt";
fs::create_directories("out");
for (auto& e : fs::directory_iterator(".")) { }
```

## any
```cpp
std::any a = 42;
a = std::string("x");
int i = std::any_cast<int>(a);
```

## apply
```cpp
auto t = std::make_tuple(1, 2);
std::apply([](int a, int b) { return a + b; }, t);
```

## invoke
```cpp
std::invoke(&Foo::f, obj, 1);   // 成员函数
std::invoke(f, 1, 2);           // 可调用对象
```

## Parallel algorithms
```cpp
#include <execution>
std::sort(std::execution::par, v.begin(), v.end());
std::for_each(std::execution::par_unseq, v.begin(), v.end(), f);
```

## Concepts requires
```cpp
template<typename T>
concept Addable = requires(T a, T b) { a + b; };

template<Addable T>
T sum(T a, T b) { return a + b; }

template<typename T>
requires std::integral<T>
void f(T);
```

## Modules
```cpp
// mymod.ixx
export module mymod;
export void f();
// main
import mymod;
```

## Coroutines
```cpp
generator<int> gen() {
    co_yield 1;
    co_yield 2;
}
task<void> async_f() {
    int x = co_await get_value();
}
```

## Three-way comparison
```cpp
struct S {
    int a;
    auto operator<=>(const S&) const = default;   // 生成 == < > <= >=
};
// 或手写：return a <=> other.a;
```

## consteval constinit
```cpp
consteval int sq(int n) { return n * n; }   // 必须编译期求值
constinit int x = sq(3);   // 静态存储，编译期初始化
```

## Designated initializers
```cpp
struct S { int a; int b; };
S s = { .a = 1, .b = 2 };   // 按成员名，可乱序（C++20）
```

## Template Lambda
```cpp
auto f = []<typename T>(std::vector<T> v) { return v.size(); };
auto g = []<int N>() { return N; };
```

## ranges
```cpp
#include <ranges>
auto evens = v | std::views::filter([](int x) { return x % 2 == 0; });
auto taken = v | std::views::take(5);
std::ranges::sort(v);
for (auto x : std::views::reverse(v)) { }
```

## span
```cpp
void f(std::span<int> s) {   // 非拥有，连续区间
    for (int x : s) { }
}
f(v);   f(std::array<int,3>{1,2,3});
```

## format
```cpp
#include <format>
std::string s = std::format("{} {}", "hi", 42);
std::string err = std::format("{}:{}", std::source_location::current().file_name(), __LINE__);
```

## jthread
```cpp
std::jthread t([](std::stop_token st) {
    while (!st.stop_requested()) { }
});   // 析构时自动 request_stop + join
```

## source_location
```cpp
void log(std::source_location loc = std::source_location::current()) {
    // loc.file_name(), .line(), .function_name()
}
```

## chrono calendar timezone
```cpp
#include <chrono>
using namespace std::chrono;
auto d = year_month_day{2024y/3/15};
auto now = zoned_time{current_zone(), system_clock::now()};
```

## if consteval
```cpp
void f() {
    if consteval { /* 编译期分支 */ }
    else { /* 运行期 */ }
}
```

## Deducing this
```cpp
struct S {
    int get(this auto const& self) { return self.x; }
    int x;
};
```

## Multidimensional subscript
```cpp
struct M {
    int operator[](size_t i, size_t j) const;
};
m[i, j];
```

## static operator()
```cpp
struct F {
    static int operator()(int a, int b) { return a + b; }
};
```

## expected
```cpp
std::expected<int, std::string> e = 42;
if (e) use(*e);
else handle(e.error());
```

## flat_map flat_set
```cpp
std::flat_map<std::string, int> fm;   // 有序， contiguous keys/values
std::flat_set<int> fs;
```

## print println
```cpp
#include <print>
std::print("{} {}", 1, 2);      // 无换行
std::println("{}", x);          // 带换行
```

## generator
```cpp
std::generator<int> gen() {
    co_yield 1;
    co_yield 2;
}
for (int x : gen()) { }
```

## mdspan
```cpp
#include <mdspan>
std::mdspan m(data, 3, 4);   // 多维视图，不拥有
m[i, j] = 0;
```

## stacktrace
```cpp
#include <stacktrace>
auto st = std::stacktrace::current();
for (auto& e : st) { /* e.description(), e.source_file() */ }
```

