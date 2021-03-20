#include <ampi/tests/ut_helpers.hpp>

#include <ampi/serialize/pfr_tuple.hpp>
#include <ampi/serialize/transmute.hpp>

#include <boost/hana/define_struct.hpp>

#include <map>
#include <set>
#include <unordered_map>

struct hana_test
{
    bool abc;
    int def;
    ampi::string ghijkl;

    bool operator==(const hana_test& other) const = default;
};
BOOST_HANA_ADAPT_STRUCT(hana_test,abc,def,ghijkl);

struct hana_test2
{
    float xyz;
};
BOOST_HANA_ADAPT_STRUCT(hana_test2,xyz);

class hana_test3
{
public:
    explicit hana_test3(int x = 0): x_{x} {}
    int x() const noexcept { return x_; }
    void set_x(int new_x) noexcept { x_ = new_x; }
    bool operator==(const hana_test3& other) const = default;
private:
    int x_;
};

template<typename Class,typename T,auto Getter,auto Setter>
struct accessor_proxy
{
    using proxied_value_type = T;

    Class instance_;

    operator T() const noexcept
    {
        return (instance_.*Getter)();
    }

    accessor_proxy& operator=(T x) noexcept
    {
        (instance_.*Setter)(std::move(x));
        return *this;
    }
};

template<>
struct boost::hana::accessors_impl<hana_test3>
{
    constexpr static auto apply()
    {
        return boost::hana::make_tuple(
            boost::hana::make_pair(
                BOOST_HANA_STRING("x"),
                [](auto&& instance){
                    return accessor_proxy<decltype(instance)&&,int,
                        &hana_test3::x,&hana_test3::set_x>{std::forward<decltype(instance)>(instance)};
                }
            )
        );
    }
};

struct pfr_test
{
    std::string x;
    unsigned y;

    bool operator==(const pfr_test& other) const = default;
};

namespace ampi { namespace
{
    using namespace boost::ut;

    template<typename To,typename From>
    void test_roundtrip(const From& x)
    {
        To y = ampi::transmute<To>(x);
        expect(x==y);
    }

    template<typename T>
    void test_roundtrip(const T& x)
    {
        test_roundtrip<T,T>(x);
    }

    template<typename To,typename From>
    void test_roundtrip_integral(const From& x)
    {
        To y = ampi::transmute<To>(x);
        expect(std::cmp_equal(x,y));
    }

    suite serialize = []{
        "primitive"_test = []{
            test_roundtrip(nullptr);
            test_roundtrip(true);
            test_roundtrip(uint8_t(1));
            test_roundtrip(uint16_t(2));
            test_roundtrip(uint32_t(3));
            test_roundtrip(uint64_t(4));
            test_roundtrip(int8_t(-1));
            test_roundtrip(int16_t(-2));
            test_roundtrip(int32_t(-3));
            test_roundtrip(int64_t(-4));
            test_roundtrip(1.5f);
            test_roundtrip(4.2);

            test_roundtrip_integral<int>(uint16_t(2));
            expect(throws<ampi::structure_error>([]{
                test_roundtrip_integral<unsigned>(-3);
            }));
            expect(throws<ampi::structure_error>([]{
                ampi::transmute<std::int8_t>(0xffffu);
            }));

            {
                using namespace std::chrono_literals;
                test_roundtrip(ampi::timestamp_t{std::chrono::sys_days{2020y/10/15}+18h+40min+36s});
            }

            {
                using namespace std::string_literals;
                test_roundtrip("test"s);
                using namespace std::string_view_literals;
                test_roundtrip<ampi::string>("abcdefghijklmnopqrstuvwxyz"sv);
            }

            test_roundtrip(ampi::vector<std::byte>{std::byte{0x01},std::byte{0x02},std::byte{0x03}});
        };
        "optional"_test = []{
            test_roundtrip(ampi::optional<int>{});
            test_roundtrip(ampi::optional<double>{3.45});
        };
        "variant"_test = []{
            test_roundtrip(ampi::variant<bool,int,double>{42});
        };
        "tuple"_test = []{
            test_roundtrip(std::pair<bool,int>{true,42});
            test_roundtrip(std::tuple<>{});
            test_roundtrip(std::tuple<bool,int,double>{true,42,1.23});
        };
        "sequence"_test = []{
            {
                int a[3] = {1,2,3},b[3];
                ampi::transmute(b,a);
                expect(std::equal(std::begin(a),std::end(a),std::begin(b),std::end(b)));
            }
            test_roundtrip(std::array<int,0>{});
            test_roundtrip(std::array<int,3>{1,2,3});
            test_roundtrip(ampi::vector<int>{1,2,3});
            test_roundtrip(std::set<int>{1,2,3});
            test_roundtrip(std::multiset<int>{1,2,2,3});
            expect(throws<ampi::structure_error>([]{
                ampi::transmute<std::set<int>>(std::multiset<int>{1,2,2,3});
            }));
        };
        "map"_test = []{
            test_roundtrip(std::map<int,double>{{1,2.3},{4,5.6}});
            test_roundtrip(std::unordered_multimap<int,double>{{1,2.3},{1,4.5},{6,7.8}});
            expect(throws<ampi::structure_error>([]{
                ampi::transmute<std::unordered_map<int,double>>(
                    std::unordered_multimap<int,double>{{1,2.3},{1,4.5},{6,7.8}});
            }));
        };
        "hana"_test = []{
            test_roundtrip(hana_test{true,45,"test"});
            expect(throws<ampi::structure_error>([]{
                ampi::transmute<hana_test>(hana_test2{});
            }));
            test_roundtrip(hana_test3{42});
        };
        "pfr"_test = []{
            test_roundtrip(pfr_test{"test",123u});
        };
    };
}}
