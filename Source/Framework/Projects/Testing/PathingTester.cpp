#include <pch.h>
#include "PathingTester.h"
#include "Core/Serialization.h"
#include "Agent/AStarAgent.h"
#include <sstream>

#include <fstream>

namespace fs = std::experimental::filesystem;

const std::wstring screenshots[] = { L"Smooth_", L"Rubber_", L"Smooth_Rubber_" };

bool PathTester::initialize()
{
    currentTest = -1;

    needScreenshot = -1;
    testsProcessed = 0;

    clearGuard = false;

    const bool result = load_tests();

    if (result == true)
    {
        goto_next_test();
    }

    return result;
}

void PathTester::set_agent(AStarAgent *a)
{
    agent = a;
}

const std::wstring &PathTester::get_button_text()
{
    return buttonText;
}

const std::wstring &PathTester::get_status_text()
{
    return statusText;
}

const std::wstring &PathTester::get_failed_text()
{
    return failedText;
}

void PathTester::clear()
{
    if (clearGuard == false)
    {
        statusText = L"";
        currentFailed = 0;
        failedData.clear();
    }
}

void PathTester::goto_next_test()
{
    currentTest = (currentTest + 1) % tests.size();

    const auto &name = tests[currentTest].get_name();

    buttonText = std::wstring(name.begin(), name.end());
}

void PathTester::goto_next_failed()
{
    currentFailed = (currentFailed + 1) % failedData.size();
}

bool PathTester::has_multiple_failed_tests()
{
    return failedData.size() > 1;
}

void PathTester::execute_current_test()
{
    if (tickQueue.empty() == true)
    {
        on_test_start();

        auto &test = tests[currentTest];
        totalTests = test.get_num_tests();

        tickQueue.push({ static_cast<size_t>(currentTest), false });

        outputName = test.get_name();
    }
    else
    {
        std::cout << "Attemped to run test while another test is in progress, ignoring request" << std::endl;
    }
}

void PathTester::execute_all_tests()
{
    if (tickQueue.empty() == true)
    {
        on_test_start();

        for (size_t i = 0; i < tests.size(); ++i)
        {
            tickQueue.push({ i, false });
            totalTests += tests[i].get_num_tests();
        }

        outputName = "All_Tests";
    }
    else
    {
        std::cout << "Attemped to run test while another test is in progress, ignoring request" << std::endl;
    }
}

void PathTester::execute_speed_test()
{}

void PathTester::tick()
{
    // check if the previous test needed a screenshot, now that we have rendered it
    if (needScreenshot != -1)
    {
        renderer->output_screenshot(screenshots[needScreenshot]);
        needScreenshot = -1;
    }

    if (tickQueue.empty() == false)
    {
        auto &topQueue = tickQueue.front();

        auto &test = tests[topQueue.index];
        
        // do any prep work needed
        if (topQueue.prepped == false)
        {
            topQueue.prepped = true;
            test.prep(agent);
            results.emplace_back(test.get_name(), test.get_settings());
        }

        auto &result = results.back();

        // run the next test in the test case
        const auto outcome = test.tick(agent, result);

        // see if we need to generate a screenshot for this test, after we render
        if (outcome.screenshot == true)
        {
            const auto &settings = test.get_settings();

            if (settings.smoothing && settings.rubberBanding)
            {
                needScreenshot = 2;
            }
            else if (settings.rubberBanding == true)
            {
                needScreenshot = 1;
            }
            else
            {
                needScreenshot = 0;
            }
        }

        // if this was the final test in this test case
        if (outcome.complete == true)
        {
            // record any failures
            if (result.num_failing_tests() > 0)
            {
                const auto &failures = result.get_failed_tests();
                failedData.insert(failedData.end(), failures.begin(), failures.end());

                
            }

            // pop the queue
            tickQueue.pop();
        }
        else
        {
            // update the status message to show we finished another test
            ++testsProcessed;
            build_status_message();
        }
    }
    else
    {
        on_test_end();
    }
}

void PathTester::bootstrap()
{
    const unsigned numHeuristics = static_cast<unsigned>(Heuristic::NUM_ENTRIES);

    PathRequest::Settings settings;
    settings.debugColoring = true;
    settings.method = Method::ASTAR;
    settings.rubberBanding = false;
    settings.singleStep = false;
    settings.smoothing = false;
    settings.weight = 1.0f;

    for (unsigned h = 0; h < numHeuristics; ++h)
    {
        settings.heuristic = static_cast<Heuristic>(h);

        const auto heuristicName = get_heuristic_text(static_cast<Heuristic>(h));
        const auto smallName = heuristicName + "_Small";
        const auto largeName = heuristicName + "_Large";

        PathingTestCase smallTest;
        smallTest.bootstrap(agent, settings, 10, heuristicName);
        const std::string smallFilename = smallName + ".txt";
        const auto smallFilepath = Serialization::testsPath / smallFilename;
        Serialization::serialize(smallTest, smallFilepath);

        PathingTestCase largeTest;
        largeTest.bootstrap(agent, settings, 50, heuristicName + "++");
        const std::string largeFilename = largeName + ".txt";
        const auto largeFilepath = Serialization::testsPath / largeFilename;
        Serialization::serialize(largeTest, largeFilepath);
    }
}

void PathTester::on_test_start()
{
    clearGuard = true;
    testsProcessed = 0;
    needScreenshot = -1;
    Messenger::send_message(Messages::PATH_TEST_BEGIN);

    results.clear();
    failedData.clear();
}

void PathTester::on_test_end()
{
    clearGuard = false;
    Messenger::send_message(Messages::PATH_TEST_END);
    build_status_message(results);

    std::stringstream filename;
    filename << outputName << "_";

    Serialization::generate_time_stamp(filename);
    filename << ".txt";

    const auto filepath = Serialization::outputPath / filename.str();

    Serialization::serialize(results, filepath);

    if (failedData.size() > 0)
    {
        currentFailed = 0;
        recreate_failed_scenario(failedData.front());
        build_status_message(failedData.front());
    }
}

bool PathTester::load_tests()
{
    std::cout << "    Initializing Path Testing System..." << std::endl;

    const fs::directory_iterator dir(Serialization::testsPath);

    for (auto && entry : dir)
    {
        if (fs::is_regular_file(entry) == true)
        {
            PathingTestCase temp;
            if (Serialization::deserialize(temp, fs::path(entry)) == true)
            {
                tests.emplace_back(std::move(temp));
            }
        }
    }

    Callback clearCB = std::bind(&PathTester::clear, this);
    Messenger::listen_for_message(Messages::PATH_REQUEST_BEGIN, clearCB);
    Messenger::listen_for_message(Messages::MAP_CHANGE, clearCB);

    results.reserve(tests.size());

    return tests.size() > 0;
}

void PathTester::build_status_message(const std::vector<PathingTestResult> &results)
{
    size_t numPassed = 0;
    size_t numFailed = 0;
    size_t numVisual = 0;

    // count the total number of tests run, and their state
    for (const auto & r : results)
    {
        const size_t f = r.num_failing_tests();

        numPassed += r.num_passing_tests();
        numFailed += f;
        numVisual += r.num_visual_tests();
    }

    const size_t total = numPassed + numFailed + numVisual;

    std::wstringstream stream;

    stream << L"Out of " << std::to_wstring(total) << L" tests: ";

    const unsigned elementCount = static_cast<unsigned>(numPassed > 0) +
        static_cast<unsigned>(numFailed > 0) +
        static_cast<unsigned>(numVisual > 0);
    
    if (numPassed > 0)
    {
        stream << std::to_wstring(numPassed) << L" passed";

        if (elementCount == 3)
        {
            stream << L", ";
        }
        else if (elementCount == 2)
        {
            stream << L" and ";
        }
    }

    if (numFailed > 0)
    {
        stream << std::to_wstring(numFailed) << L" failed";

        if (elementCount == 3)
        {
            stream << L", and ";
        }
        else if (elementCount == 2)
        {
            stream << L" and ";
        }

        // set to -1, so the next click brings it to 0
        currentFailed = -1;
    }

    if (numVisual > 0)
    {
        stream << std::to_wstring(numVisual) << L" screenshots were generated for validation";
    }

    statusText = stream.str();
}

void PathTester::build_status_message(const PathingTestData &failed)
{
    const std::string temp(failed.get_message_text());
    failedText = std::wstring(temp.begin(), temp.end());
}

void PathTester::build_status_message()
{
    statusText = std::to_wstring(testsProcessed) + L" of " + std::to_wstring(totalTests) + L" tests have been executed";
}

void PathTester::recreate_failed_scenario(const PathingTestData &failed)
{
    build_status_message(failed);

    // make a copy of the failed test
    PathingTestData local(failed);

    // prevent the map change from wiping our data
    clearGuard = true;

    // run the test and discard the results
    local(agent);

    clearGuard = false;
}

std::experimental::filesystem::path PathTester::build_filename(const std::string & title)
{
    std::stringstream filename;

    filename << title << "_";
    Serialization::generate_time_stamp(filename);
    filename << ".txt";

    return Serialization::outputPath / filename.str();
}
