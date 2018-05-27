#include "catch/catch.hpp"

#include "smtp-lib-test/scenario_runner.hpp"

TEST_CASE("acceptance test", "[acceptance][server]") {
    SECTION("handle valid mail") {
        ScenarioRunner runner;
        CHECK(runner.run_test_scenario("basic") == 0);
    }

    SECTION("handle multiple mails") {
        ScenarioRunner runner;
        CHECK(runner.run_test_scenario("multiple_mails") == 0);
    }

    SECTION("handle broken token") {
        ScenarioRunner runner;
        CHECK(runner.run_test_scenario("broken_token") == 0);
    }

    SECTION("wrong token order") {
        ScenarioRunner runner;
        CHECK(runner.run_test_scenario("wron_token") == 0);
    }
}