#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#include <melon/core/log_configuration.hpp>

int main(int argc, char* argv[])
{
    melon::core::log::setup();

    int result = Catch::Session().run(argc, argv);

    return result;
}
